#ifndef RGLZ_ECS_H
#define RGLZ_ECS_H

#include "base.h"
#include "manual_lifetime.h"
#include <thread>

#define DECLARE_COMPONENT_ID(type) \
	extern const ComponentId _ecs_id##type;

#define DEFINE_COMPONENT_ID(type) \
	const ComponentId _ecs_id##type = __COUNTER__ + 1;

#define COMPONENT_ID(type) \
	_ecs_id##type

namespace rglz {
	namespace ecs {

		typedef uint32_t ComponentBitmask;
		typedef uint32_t EntityId;
		typedef uint8_t ComponentId;

		typedef enum {
			COMPONENT_SYSTEM_FIBER_INACTIVE = 0,
			COMPONENT_SYSTEM_FIBER_IDLING,
			COMPONENT_SYSTEM_FIBER_RUNNING
		} ComponentSystemFiberState;

		typedef struct ComponentPool {
			EntityId sparse_entity_id_list[RGLZ_MAX_ENTITIES];
			EntityId dense_entity_id_list[RGLZ_MAX_ENTITIES];
			void* component_list;
			uint32_t count;
			uint32_t capacity;
			uint32_t size;
		} ComponentPool;

		typedef struct World {
			EntityId destroy_entity_queue[RGLZ_MAX_ENTITIES];
			ComponentBitmask component_bitset_list[RGLZ_MAX_ENTITIES];
			EntityId free_entity_id_list[RGLZ_MAX_ENTITIES];
			ComponentPool component_pool_list[RGLZ_MAX_COMPONENT_IDS - 1];
			uint32_t free_count;
			uint32_t destroy_entity_queue_count;
			ManualLifetime<std::mutex> mutex;
		} World;

		typedef void(*ComponentSystemFunc)(World*, void*, void*);

		typedef struct ComponentDescriptor {
			void* component_p;
			ComponentId component_id;
			ComponentBitmask component_bitmask;
		} ComponentDescriptor;

		typedef struct ComponentSystemDescriptor {
			ComponentSystemFunc func;
			void* data_p;
			ComponentBitmask component_write_bitmask;
			ComponentBitmask component_read_bitmask;
		} ComponentSystemDescriptor;

		typedef struct ComponentSystemFiber {
			uint16_t count;
			World* world_p;
			void* data_p;
			ComponentBitmask component_write_bitmask;
			ComponentBitmask component_read_bitmask;
			ManualLifetime<std::thread> thread;
			std::atomic<uint32_t> thread_state;
			ComponentSystemFunc func_list[RGLZ_MAX_FIBER_COMPONENT_SYSTEM_FUNCS];
			void* data_list[RGLZ_MAX_FIBER_COMPONENT_SYSTEM_FUNCS];
		} ComponentSystemFiber;

		void world_create(World* world_p);

		void world_destroy(World* world_p);

		void world_register_component_pool(World* world_p, void* component_list, uint32_t capacity, ComponentId component_id, uint32_t size);

		void* world_get_component_list(World* world_p, ComponentId component_id, uint32_t* count);

		EntityId* world_get_entity_list(World* world_p, ComponentId component_id, uint32_t* count);

		void world_flush_entity_destroy_queue(World* world_p);

		EntityId entity_create(World* world_p, ComponentDescriptor* component_descriptor_list, uint32_t count);
		void entity_destroy(World* world_p, EntityId entity);

		void entity_add_components(World* world_p, EntityId entity, ComponentDescriptor* component_descriptor_list, uint32_t count);

		void entity_remove_components(World* world_p, EntityId entity, ComponentBitmask component_bitmask);

		void* entity_get_component(World* world_p, EntityId entity, ComponentId component_id);

		void component_descriptor_create(ComponentDescriptor* component_descriptor_p, void* component_p, ComponentId component_id);

		bool entity_has_component(World* world_p, EntityId entity, ComponentId component_id);

		bool entity_is_alive(World* world_p, EntityId entity);

		bool entity_has_components(World* world_p, EntityId entity, ComponentBitmask component_bitmask);

		ComponentBitmask component_id_to_bitmask(ComponentId component_id);

		// component_bitmask must have only one bit set
		ComponentId component_bitmask_to_id(ComponentBitmask component_bitmask);

		void component_system_descriptor_create(ComponentSystemDescriptor* component_system_descriptor_p, ComponentSystemFunc func, ComponentBitmask component_write_bitmask, ComponentBitmask component_read_bitmask, void* data_p);

		void component_system_descriptor_list_sort(ComponentSystemDescriptor* component_system_descriptor_list, uint32_t count);

		// expects component_system_descriptor_list to be sorted (w->wr->r)
		// returns component_system_fiber_list count
		uint16_t component_system_fiber_list_create(ComponentSystemDescriptor* component_system_descriptor_list, uint32_t component_system_descriptor_list_count, ComponentSystemFiber* component_system_fiber_list, uint16_t max_component_system_fiber_list_count);

		void component_system_fiber_list_destroy(ComponentSystemFiber* fiber_list, uint16_t count);

		void component_system_fiber_list_run(ComponentSystemFiber* fiber_list, uint16_t count, World* world_p, void* shared_data_p);
	}
}


#endif	// RGLZ_ECS_H
