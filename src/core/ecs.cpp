#include "core/ecs.h"

namespace rglz {
	namespace ecs {
		thread_local ComponentSystemFiber* tls_current_fiber = nullptr;
        static std::atomic<bool> g_fiber_locks[RGLZ_MAX_THREADS];

        static void lock_fiber(uint32_t idx) {
            while (g_fiber_locks[idx].exchange(true, std::memory_order_acquire)) {
                std::this_thread::yield();
            }
        }

        static void unlock_fiber(uint32_t idx) {
            g_fiber_locks[idx].store(false, std::memory_order_release);
        }
		static EntityId world_generate_entity_id(World* world_p);
		static void world_free_entity_id(World* world_p, EntityId entity);
		static void component_system_fiber_add_component_system(ComponentSystemFiber* fiber, ComponentSystemDescriptor* descriptor);
		static void component_system_fiber_merge(ComponentSystemFiber* dest_fiber, ComponentSystemFiber* src_fiber);
		static void component_system_fiber_shift_left_to_fill_empty_slot(ComponentSystemFiber* list, uint32_t count, uint32_t empty_slot_index);
		static void component_system_fiber_thread(void* data_p);
		static void internal_entity_remove_components(World* world_p, EntityId entity, ComponentBitmask component_bitmask);
		static bool do_job_work(ParallelJob* job);

		void world_create(World* world_p) {
			world_p->free_count = RGLZ_MAX_ENTITIES;
			for (uint32_t i = 0; i < RGLZ_MAX_ENTITIES; i++) {
				world_p->free_entity_id_list[i] = RGLZ_MAX_ENTITIES - i - 1;
			}

			for (uint32_t i = 0; i < RGLZ_MAX_COMPONENT_IDS - 1; i++) {
				world_p->component_pool_list[i].component_list = nullptr;
				world_p->component_pool_list[i].count = 0;
				world_p->component_pool_list[i].capacity = 0;
				world_p->component_pool_list[i].size = 0;
			}
			world_p->destroy_entity_queue_count = 0;
			world_p->mutex.create();
		}

		void world_destroy(World* world_p) {
			world_p->mutex.destroy();
		}

		void world_register_component_pool(World* world_p, void* component_list, uint32_t capacity, ComponentId component_id, uint32_t size) {
			RGLZ_ENGINE_ALWAYS_ASSERT_MSG(component_id != 0, "component_id = 0 is reserved");
			ComponentPool* component_pool = &(world_p->component_pool_list[component_id - 1]);
			component_pool->component_list = component_list;
			component_pool->count = 0;
			component_pool->capacity = capacity;
			component_pool->size = size;
		}

		void world_flush_entity_destroy_queue(World* world_p) {
			for (uint32_t i = 0; i < world_p->destroy_entity_queue_count; i++) {
				EntityId entity = world_p->destroy_entity_queue[i];
				ComponentBitmask component_bitset = world_p->component_bitset_list[entity];
				if (component_bitset) {
					internal_entity_remove_components(world_p, entity, component_bitset);
					world_free_entity_id(world_p, entity);
				}
			}

			world_p->destroy_entity_queue_count = 0;
		}

		void* world_get_component_list(World* world_p, ComponentId component_id, uint32_t* count) {
			RGLZ_ENGINE_ALWAYS_ASSERT_MSG(component_id != 0, "component_id = 0 is reserved");
			ComponentPool* component_pool_p = &(world_p->component_pool_list[component_id - 1]);
			*count = component_pool_p->count;

			return component_pool_p->component_list;
		}

		EntityId* world_get_entity_list(World* world_p, ComponentId component_id, uint32_t* count) {
			RGLZ_ENGINE_ALWAYS_ASSERT_MSG(component_id != 0, "component_id = 0 is reserved");

			ComponentPool* component_pool_p = &(world_p->component_pool_list[component_id - 1]);
			*count = component_pool_p->count;

			return component_pool_p->dense_entity_id_list;
		}

		EntityId entity_create(World* world_p, ComponentDescriptor* component_descriptor_list, uint32_t count) {
			std::lock_guard<std::mutex> lock(world_p->mutex);
			RGLZ_ENGINE_ALWAYS_ASSERT_MSG(count < RGLZ_MAX_COMPONENT_IDS, "Max component ids limit exceeded");

			EntityId out_entity = world_generate_entity_id(world_p);
			world_p->component_bitset_list[out_entity] = 1;

			entity_add_components(world_p, out_entity, component_descriptor_list, count);

			return out_entity;
		}

		void entity_destroy(World* world_p, EntityId entity) {
			std::lock_guard<std::mutex> lock(world_p->mutex);
			world_p->destroy_entity_queue[world_p->destroy_entity_queue_count++] = entity;
		}

		void entity_add_components(World* world_p, EntityId entity, ComponentDescriptor* component_descriptor_list, uint32_t count) {
				RGLZ_ENGINE_ALWAYS_ASSERT_MSG(count < RGLZ_MAX_ENTITIES , "Max entities limit exceeded");

			for (uint32_t i = 0; i < count; i++) {
				ComponentDescriptor* current_descriptor = &(component_descriptor_list[i]);
				ComponentId component_id = current_descriptor->component_id;

				RGLZ_ENGINE_ALWAYS_ASSERT_MSG(component_id != 0, "component_id = 0 is reserved");
				RGLZ_ENGINE_ALWAYS_ASSERT_MSG(!entity_has_components(world_p, entity, current_descriptor->component_bitmask), "Entity already owns one or more of the specified components to add");
				ComponentPool* component_pool = &(world_p->component_pool_list[component_id - 1]);
				uint32_t index = component_pool->count++;

				RGLZ_ENGINE_ALWAYS_ASSERT_MSG(index < component_pool->capacity, "Max component pool capacity exceeded");

				component_pool->dense_entity_id_list[index] = entity;
				component_pool->sparse_entity_id_list[entity] = index;

				memcpy((char*)(component_pool->component_list) + index * component_pool->size, current_descriptor->component_p, component_pool->size);
				world_p->component_bitset_list[entity] |= current_descriptor->component_bitmask;
			}
		}

		void entity_remove_components(World* world_p, EntityId entity, ComponentBitmask component_bitmask) {
			RGLZ_ENGINE_ALWAYS_ASSERT_MSG(entity_has_components(world_p, entity, component_bitmask), "Entity does not own all specified components to remove");

			world_p->component_bitset_list[entity] &= ~(component_bitmask);
			RGLZ_ENGINE_ALWAYS_ASSERT_MSG(!(component_bitmask & 1), "component_id = 0 can only be removed from entity on entity destruction");

			// Ignore alive bit
			component_bitmask >>= 1;

			uint16_t n_bit = 0;
			do {
				if (component_bitmask & ((ComponentBitmask)1)) {
					ComponentPool* component_pool = &(world_p->component_pool_list[n_bit]);
					EntityId index = component_pool->sparse_entity_id_list[entity];
					component_pool->dense_entity_id_list[index] = component_pool->dense_entity_id_list[--component_pool->count];
				}
				n_bit++;
			} while ((component_bitmask >>= 1));
		}

		void internal_entity_remove_components(World* world_p, EntityId entity, ComponentBitmask component_bitmask) {
			RGLZ_ENGINE_ALWAYS_ASSERT_MSG(entity_has_components(world_p, entity, component_bitmask), "Entity does not own all specified components to remove");

			world_p->component_bitset_list[entity] &= ~(component_bitmask);

			// Ignore alive bit
			component_bitmask >>= 1;

			uint16_t n_bit = 0;
			do {
				if (component_bitmask & ((ComponentBitmask)1)) {
					ComponentPool* component_pool = &(world_p->component_pool_list[n_bit]);
					EntityId index = component_pool->sparse_entity_id_list[entity];
					component_pool->dense_entity_id_list[index] = component_pool->dense_entity_id_list[--component_pool->count];
				}
				n_bit++;
			} while ((component_bitmask >>= 1));
		}

		void* entity_get_component(World* world_p, EntityId entity, ComponentId component_id) {
			RGLZ_ENGINE_ALWAYS_ASSERT_MSG(entity_has_component(world_p, entity, component_id), "Can\'t get component from entity because entity does not own such component");

			RGLZ_ENGINE_ALWAYS_ASSERT_MSG(component_id != 0, "component_id = 0 is reserved");
			ComponentPool* component_pool = &(world_p->component_pool_list[component_id - 1]);
			uint32_t index = component_pool->sparse_entity_id_list[entity];
			return (char*)component_pool->component_list + index * component_pool->size;
		}

		void component_descriptor_create(ComponentDescriptor* component_descriptor_p, void* component_p, ComponentId component_id) {
			component_descriptor_p->component_p = component_p;
			component_descriptor_p->component_id = component_id;
			component_descriptor_p->component_bitmask = component_id_to_bitmask(component_id);
		}

		bool entity_has_component(World* world_p, EntityId entity, ComponentId component_id) {
			if (world_p->component_bitset_list[entity] & component_id_to_bitmask(component_id)) {
				return true;
			}
			return false;
		}

		bool entity_has_components(World* world_p, EntityId entity, ComponentBitmask component_bitmask) {
			if ((world_p->component_bitset_list[entity] & component_bitmask) == component_bitmask) {
				return true;
			}
			return false;
		}

		bool entity_is_alive(World* world_p, EntityId entity) {
			return entity_has_component(world_p, entity, 0);
		}

		ComponentBitmask component_id_to_bitmask(ComponentId component_id) {
			ComponentBitmask component_bitmask_out;

			component_bitmask_out = (ComponentBitmask)(1 << component_id);

			return component_bitmask_out;
		}

		ComponentId component_bitmask_to_id(ComponentBitmask component_bitmask) {
			ComponentId n_bit = 0;
			while (component_bitmask != 1) {
				component_bitmask <<= 1;
				n_bit++;

			RGLZ_ENGINE_ALWAYS_ASSERT_MSG(n_bit == RGLZ_MAX_COMPONENT_IDS, "component_bitmask has more than one bit set or is equal to 0");
			}

			return n_bit;
		}

		void component_system_descriptor_create(ComponentSystemDescriptor* component_system_descriptor_p, ComponentSystemFunc func, ComponentBitmask component_write_bitmask, ComponentBitmask component_read_bitmask, void* data_p) {
			component_system_descriptor_p->func = func;
			component_system_descriptor_p->data_p = data_p;
			component_system_descriptor_p->component_write_bitmask = component_write_bitmask;
			component_system_descriptor_p->component_read_bitmask = component_read_bitmask;
		}

		uint16_t component_system_fiber_list_create(ComponentSystemDescriptor* component_system_descriptor_list, uint32_t component_system_descriptor_list_count, ComponentSystemFiber* component_system_fiber_list, uint16_t max_component_system_fiber_list_count) {

			// init fibers
			for (uint16_t i = 0; i < max_component_system_fiber_list_count; i++)
			{
				component_system_fiber_list[i].world_p = NULL;
				component_system_fiber_list[i].data_p = NULL;
				component_system_fiber_list[i].component_write_bitmask = 0;
				component_system_fiber_list[i].component_read_bitmask = 0;
				component_system_fiber_list[i].count = 0;
				component_system_fiber_list[i].thread_state.store((uint32_t)COMPONENT_SYSTEM_FIBER_INACTIVE);

			}

			uint32_t i = 0;
			uint16_t j = 0;
			bool is_component_system_fiber_list_count_out_max = false;
			uint16_t max_component_system_fiber_list_count_minus_parallel = (max_component_system_fiber_list_count > 2) ? (max_component_system_fiber_list_count - 2) : max_component_system_fiber_list_count;

			// a complete iteration of the loop will sort all component systems with an overlapping write bitmask with the first component system processed in the fiber
			while (i < component_system_descriptor_list_count) {
				ComponentSystemFiber* current_fiber = component_system_fiber_list + j;
				ComponentSystemDescriptor* current_descriptor = component_system_descriptor_list + i;
				ComponentSystemFiber* component_system_assigned_fiber = NULL;
				// iterate from k = 0 to k = used_fiber_list_count - 1
				uint32_t used_fiber_nr = max_component_system_fiber_list_count * is_component_system_fiber_list_count_out_max + j  * !is_component_system_fiber_list_count_out_max;
				for (uint16_t k = 0; k < used_fiber_nr; k++) {
					if (k != j) {
						ComponentSystemFiber* other_fiber = component_system_fiber_list + k;
						if (
								(other_fiber->component_write_bitmask & current_descriptor->component_write_bitmask) ||
								(other_fiber->component_write_bitmask & current_descriptor->component_read_bitmask) ||
								(other_fiber->component_read_bitmask & current_descriptor->component_write_bitmask)
							) {
							if (component_system_assigned_fiber != NULL) {
								component_system_fiber_merge(component_system_assigned_fiber, other_fiber);
								component_system_fiber_shift_left_to_fill_empty_slot(component_system_fiber_list, used_fiber_nr, other_fiber - component_system_fiber_list);
								j--;
							}
							else {
								component_system_fiber_add_component_system(other_fiber, current_descriptor);
								component_system_assigned_fiber = other_fiber;
								i++;
							}
						}
					}

				}

				if (component_system_assigned_fiber == NULL) {
					component_system_fiber_add_component_system(current_fiber, current_descriptor);
					i++;
					j++;
					if (j == max_component_system_fiber_list_count_minus_parallel) {
						is_component_system_fiber_list_count_out_max = true;
						j = 0;
					}
				}
			}
			
			uint16_t component_system_fiber_list_count_out = max_component_system_fiber_list_count;

			for (uint16_t k = 0; k < component_system_fiber_list_count_out; k++) {
				component_system_fiber_list[k].fiber_list = std::span<ComponentSystemFiber>(component_system_fiber_list, component_system_fiber_list_count_out);
				component_system_fiber_list[k].current_job.store(nullptr);
				component_system_fiber_list[k].thread_state.store((uint32_t)COMPONENT_SYSTEM_FIBER_IDLING, std::memory_order_relaxed);
			}

			uint16_t component_system_fiber_list_count_out_minus_one = (component_system_fiber_list_count_out - 1) * (bool)component_system_fiber_list_count_out;
			for (uint16_t k = 0; k < component_system_fiber_list_count_out_minus_one; k++) {
				component_system_fiber_list[k].thread.create(component_system_fiber_thread, (void*)(component_system_fiber_list + k));
			}

			return component_system_fiber_list_count_out;
		}

		void component_system_descriptor_list_sort(ComponentSystemDescriptor* component_system_descriptor_list, uint32_t count) {
			bool swapped;

			for (uint32_t i = 0; i < count - 1; i++) {
				swapped = false;
				for (uint32_t j = 0; j < count - i - 1; j++) {
					ComponentSystemDescriptor* current = component_system_descriptor_list + j;
					ComponentSystemDescriptor* next = component_system_descriptor_list + j + 1;

					uint8_t current_value = 
						1 +
						(bool)(current->component_write_bitmask) -
						(bool)(current->component_read_bitmask);

					uint8_t next_value = 
						1 + 
						(bool)(next->component_write_bitmask) -
						(bool)(next->component_read_bitmask);

					if (current_value < next_value) {
						ComponentSystemDescriptor tmp = *current;
						*current = *next;
						*next = tmp;
						swapped = true;
					}
				}
				if (!swapped) {
					break;
				}
			}
		}

		void component_system_fiber_list_destroy(ComponentSystemFiber* fiber_list, uint16_t count) {

			uint16_t count_minus_one = (count - 1) * (bool)count;
			for (uint16_t i = 0; i < count_minus_one; i++) {
                ComponentSystemFiber* current_fiber = fiber_list + i;
                current_fiber->thread_state.store((uint32_t)COMPONENT_SYSTEM_FIBER_INACTIVE, std::memory_order_release);
            }
			for (uint16_t i = 0; i < count_minus_one; i++) {
				ComponentSystemFiber* current_fiber = fiber_list + i;
				if (current_fiber->thread.get().joinable()) {
                    current_fiber->thread.get().join(); 
                }
				current_fiber->thread.destroy();
			}
		}

		void component_system_fiber_list_run(ComponentSystemFiber* fiber_list, uint16_t count, World* world_p, void* data_p) {
			RGLZ_ENGINE_ASSERT(count);
			uint16_t count_minus_one = (count - 1) * (bool)count;
			for (uint16_t i = 0; i < count_minus_one; i++) {
				ComponentSystemFiber* current_fiber = fiber_list + i;
				current_fiber->world_p = world_p;
				current_fiber->data_p = data_p;
				current_fiber->thread_state.store((uint32_t)COMPONENT_SYSTEM_FIBER_RUNNING);
			}

            ComponentSystemFiber* main_fiber = fiber_list + count_minus_one;
            tls_current_fiber = main_fiber; 
            
            for (uint16_t i = 0; i < main_fiber->count; i++) {
                main_fiber->func_list[i](world_p, main_fiber->data_list[i], data_p);
            }

            for (uint16_t i = 0; i < count_minus_one; i++) {
                ComponentSystemFiber* wait_fiber = fiber_list + i;
                while (wait_fiber->thread_state.load(std::memory_order_acquire) == COMPONENT_SYSTEM_FIBER_RUNNING) {
                    bool stole = false;
                    for (auto& other_fiber : main_fiber->fiber_list) {
						if (&other_fiber == main_fiber) continue;
                        
                        uint16_t other_idx = &other_fiber - main_fiber->fiber_list.data();
                        lock_fiber(other_idx);
                        ParallelJob* job = other_fiber.current_job.load(std::memory_order_relaxed);
                        if (job) {
                            job->active_workers.fetch_add(1, std::memory_order_acquire);
                        }
                        unlock_fiber(other_idx);

                        if (job) {
                            if (other_fiber.current_job.load(std::memory_order_acquire) == job) {
                                if (do_job_work(job)) stole = true;
                            }
                            job->active_workers.fetch_sub(1, std::memory_order_release);
                            if (stole) break;
                        }
                    }
                    if (!stole) std::this_thread::yield();
                }
            }
            tls_current_fiber = nullptr;
		}

		EntityId world_generate_entity_id(World* world_p) {
			RGLZ_ENGINE_ALWAYS_ASSERT_MSG(world_p->free_count != 0, "No more entity ids available");
			return world_p->free_entity_id_list[--world_p->free_count];
		}

		void world_free_entity_id(World* world_p, EntityId entity) {
			world_p->free_entity_id_list[world_p->free_count++] = entity;
		}

		void component_system_fiber_add_component_system(ComponentSystemFiber* fiber, ComponentSystemDescriptor* descriptor) {
			RGLZ_ENGINE_ALWAYS_ASSERT_MSG(fiber->count < RGLZ_MAX_FIBER_COMPONENT_SYSTEM_FUNCS, "Max fiber component system funcs limit exceeded");

			fiber->func_list[fiber->count] = descriptor->func;
			fiber->data_list[fiber->count] = descriptor->data_p;
			fiber->component_write_bitmask |= descriptor->component_write_bitmask;
			fiber->component_read_bitmask |= descriptor->component_read_bitmask;
			fiber->component_read_bitmask &= ~(fiber->component_write_bitmask);
			fiber->count++;
		}

		void component_system_fiber_merge(ComponentSystemFiber* dest_fiber, ComponentSystemFiber* src_fiber) {
			RGLZ_ENGINE_ALWAYS_ASSERT_MSG(dest_fiber->count + src_fiber->count <= RGLZ_MAX_FIBER_COMPONENT_SYSTEM_FUNCS, "Couldn\'t merge fibers because merged system funcs count exceeds max fiber component system funcs");

			for (uint32_t i = 0; i < src_fiber->count; i++) {
				dest_fiber->func_list[dest_fiber->count + i] = src_fiber->func_list[i];
				dest_fiber->data_list[dest_fiber->count + i] = src_fiber->data_list[i];
			}

			dest_fiber->component_write_bitmask |= src_fiber->component_write_bitmask;
			dest_fiber->component_read_bitmask |= src_fiber->component_read_bitmask;
			dest_fiber->component_read_bitmask &= ~(dest_fiber->component_write_bitmask);
			dest_fiber->count += src_fiber->count;

			src_fiber->component_write_bitmask = 0;
			src_fiber->component_read_bitmask = 0;
			src_fiber->count = 0;
		}

		void component_system_fiber_shift_left_to_fill_empty_slot(ComponentSystemFiber* list, uint32_t count, uint32_t empty_slot_index) {
			for (uint32_t i = empty_slot_index; i < count - 1; i++) {
				ComponentSystemFiber* dest_fiber = list + i;
				ComponentSystemFiber* src_fiber = dest_fiber + 1;

				for (uint32_t j = 0; j < src_fiber->count; j++) {
					dest_fiber->func_list[j] = src_fiber->func_list[j];
					dest_fiber->data_list[j] = src_fiber->data_list[j];
				}

				dest_fiber->component_write_bitmask = src_fiber->component_write_bitmask;
				dest_fiber->component_read_bitmask = src_fiber->component_read_bitmask;
				dest_fiber->count = src_fiber->count;
			}
		}

		void component_system_fiber_thread(void* data_p) {
			ComponentSystemFiber* fiber = (ComponentSystemFiber*)data_p;
            tls_current_fiber = fiber; 

            while (fiber->thread_state.load(std::memory_order_acquire) != COMPONENT_SYSTEM_FIBER_INACTIVE) {
                uint32_t state = fiber->thread_state.load(std::memory_order_acquire);
                
                if (state == COMPONENT_SYSTEM_FIBER_RUNNING) {
                    for (uint32_t i = 0; i < fiber->count; i++) {
                        fiber->func_list[i](fiber->world_p, fiber->data_list[i], fiber->data_p);
                    }
                    uint32_t expected = COMPONENT_SYSTEM_FIBER_RUNNING;
                    fiber->thread_state.compare_exchange_strong(expected, COMPONENT_SYSTEM_FIBER_STEALING);
                } 
                else if (state == COMPONENT_SYSTEM_FIBER_STEALING || state == COMPONENT_SYSTEM_FIBER_IDLING) {
                    bool stole = false;
                    for (auto& other_fiber : fiber->fiber_list) {
                        if (&other_fiber == fiber) continue;

						uint32_t other_idx = &other_fiber - fiber->fiber_list.data();
                        
                        lock_fiber(other_idx);
                        ParallelJob* job = other_fiber.current_job.load(std::memory_order_relaxed);
                        if (job) {
                            job->active_workers.fetch_add(1, std::memory_order_acquire);
                        }
                        unlock_fiber(other_idx);

                        if (job) {
                            if (other_fiber.current_job.load(std::memory_order_acquire) == job) {
                                if (do_job_work(job)) stole = true;
                            }
                            job->active_workers.fetch_sub(1, std::memory_order_release);
                            if (stole) break; 
                        }
                    }
                    
                    if (!stole) std::this_thread::yield();
                }
            }
		}

		void parallel_for(ParallelForFunc func, void* data, uint32_t count, uint32_t chunk_size) {
            if (count == 0) return;
            
            ComponentSystemFiber* current_fiber = tls_current_fiber;
            RGLZ_ENGINE_ASSERT(current_fiber != nullptr);
            
            ParallelJob job;
            job.func = func;
            job.data = data;
            job.total_count = count;
            job.chunk_size = chunk_size > 0 ? chunk_size : 64;
            job.total_chunks = (count + job.chunk_size - 1) / job.chunk_size;
            job.current_idx.store(0, std::memory_order_relaxed);
            job.completed_chunks.store(0, std::memory_order_relaxed);
            job.active_workers.store(0, std::memory_order_relaxed);
            
			uint16_t fiber_idx = current_fiber - current_fiber->fiber_list.data();
            current_fiber->current_job.store(&job, std::memory_order_release);
            
            while (do_job_work(&job));
            
            while (job.completed_chunks.load(std::memory_order_acquire) < job.total_chunks) {
                std::this_thread::yield();
            }
            
			lock_fiber(fiber_idx);
            current_fiber->current_job.store(nullptr, std::memory_order_relaxed);
            unlock_fiber(fiber_idx);

            while (job.active_workers.load(std::memory_order_acquire) > 0) {
                std::this_thread::yield();
            }
        }

		bool do_job_work(ParallelJob* job) {
            uint32_t start_idx = job->current_idx.fetch_add(job->chunk_size, std::memory_order_relaxed);
            if (start_idx >= job->total_count) {
                return false; // No more work available
            }
            uint32_t end_idx = start_idx + job->chunk_size;
            if (end_idx > job->total_count) {
                end_idx = job->total_count;
            }
            
            job->func(job->data, start_idx, end_idx);
            job->completed_chunks.fetch_add(1, std::memory_order_release);
            return true;
        }
	}
}
