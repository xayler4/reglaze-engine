#include "pch.h"
#include "core/preferences.h"
#include "core/memory_profile.h"
#include "memory/memory_block.h"
#include "memory/memory_manager.h"

enum ReturnCode : int {
	OK = 0,
	FATAL_ERROR = -1,
	BAD_ALLOC = -2
};

namespace rglz {
	extern void register_client_app();
	extern MemoryProfile default_app_memory_profile();
} // namespace rglz

rglz::MemoryProfile preferences_memory_profile(const rglz::Preferences& pref);

int main(int argc, const char** argv) {
	rglz::MemoryProfile tmp_memory_profile;
	tmp_memory_profile.allocation_size = 8000;
	tmp_memory_profile.stack_allocator_allocation_size_percent = 0;
	tmp_memory_profile.single_frame_allocator_allocation_size_percent = 100;

	rglz::memory::MemoryBlock memory_block(tmp_memory_profile.allocation_size);
	if (memory_block.is_bad_alloc()) {
		return BAD_ALLOC;
	}

	rglz::memory::MemoryManager::startup(memory_block, tmp_memory_profile);

	rglz::MemoryProfile app_memory_profile = rglz::default_app_memory_profile();

	rglz::Attribute default_app_memory_profile_stack_allocator_max_allocation_size_attribute = 
		app_memory_profile.stack_allocator_max_allocation_size.has_value()
			? rglz::Attribute("MemoryProfileStackAllocatorMaxAllocationSize", static_cast<double>(app_memory_profile.stack_allocator_max_allocation_size.value()))
			: rglz::Attribute("MemoryProfileStackAllocatorMaxAllocationSize");

	rglz::Attribute default_app_memory_profile_single_frame_allocator_max_allocation_size_attribute = 
		app_memory_profile.single_frame_allocator_max_allocation_size.has_value()
			? rglz::Attribute("MemoryProfileSingleFrameAllocatorMaxAllocationSize", static_cast<double>(app_memory_profile.single_frame_allocator_max_allocation_size.value()))
			: rglz::Attribute("MemoryProfileSingleFrameAllocatorMaxAllocationSize");

	{
		rglz::Preferences engine_preferences("rglz_engine_options.cfg", {
			rglz::Attribute("MemoryProfileAllocationSize", static_cast<double>(app_memory_profile.allocation_size)),
			rglz::Attribute("MemoryProfileStackAllocatorAllocationSizePercent", app_memory_profile.stack_allocator_allocation_size_percent),
			rglz::Attribute("MemoryProfileSingleFrameAllocatorAllocationSizePercent", app_memory_profile.single_frame_allocator_allocation_size_percent),
			default_app_memory_profile_stack_allocator_max_allocation_size_attribute,
			default_app_memory_profile_single_frame_allocator_max_allocation_size_attribute
		});

		app_memory_profile = preferences_memory_profile(engine_preferences);
	}

	rglz::Engine::flush_logs();
	rglz::memory::MemoryManager::shutdown();

	memory_block.realloc(app_memory_profile.allocation_size);
	if (memory_block.is_bad_alloc()) {
		return BAD_ALLOC;
	}

	rglz::memory::MemoryManager::startup(memory_block, app_memory_profile);

	try {
		rglz::register_client_app();
		rglz::Engine::startup();
		rglz::Engine::run();
	}
	catch (const rglz::FatalError& e) {
		rglz::Engine::flush_logs();
		rglz::Engine::app().flush_logs();

		return FATAL_ERROR;
	}

	rglz::Engine::shutdown();
	rglz::memory::MemoryManager::shutdown();

	return OK;
}

rglz::MemoryProfile preferences_memory_profile(const rglz::Preferences& preferences) {
	rglz::MemoryProfile out_memory_profile;
	rglz::Attribute memory_profile_stack_allocator_max_allocation_size_attribute = *preferences["MemoryProfileStackAllocatorMaxAllocationSize"];
	rglz::Attribute memory_profile_single_frame_allocator_max_allocation_size_attribute = *preferences["MemoryProfileSingleFrameAllocatorMaxAllocationSize"];

	out_memory_profile.allocation_size = static_cast<std::size_t>(preferences["MemoryProfileAllocationSize"]->double_value());
	out_memory_profile.stack_allocator_allocation_size_percent = preferences["MemoryProfileStackAllocatorAllocationSizePercent"]->double_value();
	out_memory_profile.single_frame_allocator_allocation_size_percent = preferences["MemoryProfileSingleFrameAllocatorAllocationSizePercent"]->double_value();

	if (!memory_profile_stack_allocator_max_allocation_size_attribute.is_null()) {
		out_memory_profile.stack_allocator_max_allocation_size = static_cast<std::size_t>(memory_profile_stack_allocator_max_allocation_size_attribute.double_value());
	}

	if (!memory_profile_single_frame_allocator_max_allocation_size_attribute.is_null()) {
		out_memory_profile.single_frame_allocator_max_allocation_size = static_cast<std::size_t>(memory_profile_single_frame_allocator_max_allocation_size_attribute.double_value());
	}
	
	return out_memory_profile;
}
