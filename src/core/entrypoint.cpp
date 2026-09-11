#include "pch.h"
#include "core/preferences.h"
#include "core/memory_profile.h"
#include "memory/memory_block.h"
#include "memory/memory_manager.h"

#define MEMORY_PROFILE_ALLOCATION_SIZE_ATTRIBUTE_NAME "MemoryProfileAllocationSize"
#define MEMORY_PROFILE_CORE_STACK_ALLOCATOR_ALLOCATION_SIZE_PERCENT_ATTRIBUTE_NAME "MemoryProfileCoreStackAllocatorAllocationSizePercent"
#define MEMORY_PROFILE_SCENE_STACK_ALLOCATOR_ALLOCATION_SIZE_PERCENT_ATTRIBUTE_NAME "MemoryProfileSceneStackAllocatorAllocationSizePercent"
#define MEMORY_PROFILE_FREE_LIST_ALLOCATOR_ALLOCATION_SIZE_PERCENT_ATTRIBUTE_NAME "MemoryProfileFreeListAllocatorAllocationSizePercent"
#define MEMORY_PROFILE_SINGLE_FRAME_ALLOCATOR_ALLOCATION_SIZE_PERCENT_ATTRIBUTE_NAME "MemoryProfileSingleFrameAllocatorAllocationSizePercent"
#define MEMORY_PROFILE_CORE_STACK_ALLOCATOR_MAX_ALLOCATION_SIZE_ATTRIBUTE_NAME "MemoryProfileCoreStackAllocatorMaxAllocationSize"
#define MEMORY_PROFILE_SCENE_STACK_ALLOCATOR_MAX_ALLOCATION_SIZE_ATTRIBUTE_NAME "MemoryProfileSceneStackAllocatorMaxAllocationSize"
#define MEMORY_PROFILE_FREE_LIST_ALLOCATOR_MAX_ALLOCATION_SIZE_ATTRIBUTE_NAME "MemoryProfileFreeListAllocatorMaxAllocationSize"
#define MEMORY_PROFILE_SINGLE_FRAME_ALLOCATOR_MAX_ALLOCATION_SIZE_ATTRIBUTE_NAME "MemoryProfileSingleFrameAllocatorMaxAllocationSize"
#define ASSET_MANAGER_DOES_READ_FROM_ARCHIVE_ATTRIBUTE_NAME "AssetManagerDoesReadFromArchive"
#define ASSET_MANAGER_ARCHIVE_NAME_ATTRIBUTE_NAME "AssetManagerArchiveName"

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
	bool asset_manager_does_read_from_archive = false;
	std::string_view asset_manager_archive_name = "";

	rglz::MemoryProfile tmp_memory_profile;
	tmp_memory_profile.allocation_size = 8000;
	tmp_memory_profile.core_stack_allocator_allocation_size_percent = 0;
	tmp_memory_profile.scene_stack_allocator_allocation_size_percent = 0;
	tmp_memory_profile.free_list_allocator_allocation_size_percent = 0;
	tmp_memory_profile.single_frame_allocator_allocation_size_percent = 100;

	rglz::memory::MemoryBlock memory_block(tmp_memory_profile.allocation_size);
	if (memory_block.is_bad_alloc()) {
		return BAD_ALLOC;
	}

	rglz::memory::MemoryManager::startup(memory_block, tmp_memory_profile, "Bootstrap MemoryManager");

	rglz::MemoryProfile app_memory_profile = rglz::default_app_memory_profile();

	rglz::Attribute default_app_memory_profile_stack_allocator_max_allocation_size_attribute =
		app_memory_profile.stack_allocator_max_allocation_size.has_value()
			? rglz::Attribute(
				MEMORY_PROFILE_CORE_STACK_ALLOCATOR_MAX_ALLOCATION_SIZE_ATTRIBUTE_NAME,
				static_cast<double>(app_memory_profile.stack_allocator_max_allocation_size.value()))
			: rglz::Attribute(MEMORY_PROFILE_CORE_STACK_ALLOCATOR_MAX_ALLOCATION_SIZE_ATTRIBUTE_NAME);

	rglz::Attribute default_app_memory_profile_scene_stack_allocator_max_allocation_size_attribute =
		app_memory_profile.scene_stack_allocator_max_allocation_size.has_value()
			? rglz::Attribute(
				MEMORY_PROFILE_SCENE_STACK_ALLOCATOR_MAX_ALLOCATION_SIZE_ATTRIBUTE_NAME,
				static_cast<double>(app_memory_profile.scene_stack_allocator_max_allocation_size.value()))
			: rglz::Attribute(MEMORY_PROFILE_SCENE_STACK_ALLOCATOR_MAX_ALLOCATION_SIZE_ATTRIBUTE_NAME);

	rglz::Attribute default_app_memory_profile_free_list_allocator_max_allocation_size_attribute =
		app_memory_profile.free_list_allocator_max_allocation_size.has_value()
			? rglz::Attribute(
				MEMORY_PROFILE_FREE_LIST_ALLOCATOR_MAX_ALLOCATION_SIZE_ATTRIBUTE_NAME,
				static_cast<double>(app_memory_profile.free_list_allocator_max_allocation_size.value()))
			: rglz::Attribute(MEMORY_PROFILE_FREE_LIST_ALLOCATOR_MAX_ALLOCATION_SIZE_ATTRIBUTE_NAME);

	rglz::Attribute default_app_memory_profile_single_frame_allocator_max_allocation_size_attribute =
		app_memory_profile.single_frame_allocator_max_allocation_size.has_value()
			? rglz::Attribute(
				MEMORY_PROFILE_SINGLE_FRAME_ALLOCATOR_MAX_ALLOCATION_SIZE_ATTRIBUTE_NAME,
				static_cast<double>(app_memory_profile.single_frame_allocator_max_allocation_size.value()))
			: rglz::Attribute(MEMORY_PROFILE_SINGLE_FRAME_ALLOCATOR_MAX_ALLOCATION_SIZE_ATTRIBUTE_NAME);

		rglz::Attribute default_asset_manager_does_read_from_archive_attribute(
	ASSET_MANAGER_DOES_READ_FROM_ARCHIVE_ATTRIBUTE_NAME,
	false);

		rglz::Attribute default_asset_manager_archive_name_attribute(
	ASSET_MANAGER_ARCHIVE_NAME_ATTRIBUTE_NAME,
	"archive.rglz");

	try {
	{
		rglz::Preferences engine_preferences("rglz_engine_options.cfg", {
			rglz::Attribute(
				MEMORY_PROFILE_ALLOCATION_SIZE_ATTRIBUTE_NAME,
				static_cast<double>(app_memory_profile.allocation_size)),
			rglz::Attribute(
				MEMORY_PROFILE_CORE_STACK_ALLOCATOR_ALLOCATION_SIZE_PERCENT_ATTRIBUTE_NAME,
				app_memory_profile.core_stack_allocator_allocation_size_percent),
			rglz::Attribute(
				MEMORY_PROFILE_SCENE_STACK_ALLOCATOR_ALLOCATION_SIZE_PERCENT_ATTRIBUTE_NAME,
				app_memory_profile.scene_stack_allocator_allocation_size_percent),
			rglz::Attribute(
				MEMORY_PROFILE_FREE_LIST_ALLOCATOR_ALLOCATION_SIZE_PERCENT_ATTRIBUTE_NAME,
				app_memory_profile.free_list_allocator_allocation_size_percent),
			rglz::Attribute(
				MEMORY_PROFILE_SINGLE_FRAME_ALLOCATOR_ALLOCATION_SIZE_PERCENT_ATTRIBUTE_NAME,
				app_memory_profile.single_frame_allocator_allocation_size_percent),
			default_app_memory_profile_stack_allocator_max_allocation_size_attribute,
			default_app_memory_profile_scene_stack_allocator_max_allocation_size_attribute,
			default_app_memory_profile_free_list_allocator_max_allocation_size_attribute,
			default_app_memory_profile_single_frame_allocator_max_allocation_size_attribute,
			default_asset_manager_does_read_from_archive_attribute,
			default_asset_manager_archive_name_attribute
		});

		app_memory_profile = preferences_memory_profile(engine_preferences);
		asset_manager_does_read_from_archive =
			engine_preferences[ASSET_MANAGER_DOES_READ_FROM_ARCHIVE_ATTRIBUTE_NAME]->boolean_value();
		asset_manager_archive_name =
			engine_preferences[ASSET_MANAGER_ARCHIVE_NAME_ATTRIBUTE_NAME]->string_value();
	}

	rglz::Engine::flush_logs();
	rglz::memory::MemoryManager::shutdown();

	memory_block.realloc(app_memory_profile.allocation_size);
	if (memory_block.is_bad_alloc()) {
		return BAD_ALLOC;
	}

	rglz::memory::MemoryManager::startup(memory_block, app_memory_profile, "MemoryManager");

		rglz::register_client_app();
		rglz::Engine::startup(asset_manager_does_read_from_archive, asset_manager_archive_name);
		rglz::Engine::run();
	}
	catch (const rglz::FatalError& e) {
		rglz::Engine::flush_logs();
		if (rglz::Engine::is_app_valid()) {
			rglz::Engine::app_logger().flush_logs();
		}

		return FATAL_ERROR;
	}

	rglz::Engine::shutdown();
	rglz::memory::MemoryManager::shutdown();

	return OK;
}

rglz::MemoryProfile preferences_memory_profile(const rglz::Preferences& preferences) {
	rglz::MemoryProfile out_memory_profile;

	rglz::Attribute memory_profile_stack_allocator_max_allocation_size_attribute =
		*preferences[MEMORY_PROFILE_CORE_STACK_ALLOCATOR_MAX_ALLOCATION_SIZE_ATTRIBUTE_NAME];

	rglz::Attribute memory_profile_scene_stack_allocator_max_allocation_size_attribute =
		*preferences[MEMORY_PROFILE_SCENE_STACK_ALLOCATOR_MAX_ALLOCATION_SIZE_ATTRIBUTE_NAME];

	rglz::Attribute memory_profile_free_list_allocator_max_allocation_size_attribute =
		*preferences[MEMORY_PROFILE_FREE_LIST_ALLOCATOR_MAX_ALLOCATION_SIZE_ATTRIBUTE_NAME];

	rglz::Attribute memory_profile_single_frame_allocator_max_allocation_size_attribute =
		*preferences[MEMORY_PROFILE_SINGLE_FRAME_ALLOCATOR_MAX_ALLOCATION_SIZE_ATTRIBUTE_NAME];

	out_memory_profile.allocation_size =
		static_cast<std::size_t>(
			preferences[MEMORY_PROFILE_ALLOCATION_SIZE_ATTRIBUTE_NAME]->float_value());

	out_memory_profile.core_stack_allocator_allocation_size_percent =
		preferences[MEMORY_PROFILE_CORE_STACK_ALLOCATOR_ALLOCATION_SIZE_PERCENT_ATTRIBUTE_NAME]->float_value();

	out_memory_profile.scene_stack_allocator_allocation_size_percent =
		preferences[MEMORY_PROFILE_SCENE_STACK_ALLOCATOR_ALLOCATION_SIZE_PERCENT_ATTRIBUTE_NAME]->float_value();

	out_memory_profile.free_list_allocator_allocation_size_percent =
		preferences[MEMORY_PROFILE_FREE_LIST_ALLOCATOR_ALLOCATION_SIZE_PERCENT_ATTRIBUTE_NAME]->float_value();

	out_memory_profile.single_frame_allocator_allocation_size_percent =
		preferences[MEMORY_PROFILE_SINGLE_FRAME_ALLOCATOR_ALLOCATION_SIZE_PERCENT_ATTRIBUTE_NAME]->float_value();

	if (!memory_profile_stack_allocator_max_allocation_size_attribute.is_null()) {
		out_memory_profile.stack_allocator_max_allocation_size =
			static_cast<std::size_t>(
				memory_profile_stack_allocator_max_allocation_size_attribute.float_value());
	}

	if (!memory_profile_scene_stack_allocator_max_allocation_size_attribute.is_null()) {
		out_memory_profile.scene_stack_allocator_max_allocation_size =
			static_cast<std::size_t>(
				memory_profile_scene_stack_allocator_max_allocation_size_attribute.float_value());
	}

	if (!memory_profile_free_list_allocator_max_allocation_size_attribute.is_null()) {
		out_memory_profile.free_list_allocator_max_allocation_size =
			static_cast<std::size_t>(
				memory_profile_free_list_allocator_max_allocation_size_attribute.float_value());
	}

	if (!memory_profile_single_frame_allocator_max_allocation_size_attribute.is_null()) {
		out_memory_profile.single_frame_allocator_max_allocation_size =
			static_cast<std::size_t>(
				memory_profile_single_frame_allocator_max_allocation_size_attribute.float_value());
	}

	return out_memory_profile;
}
