#include "pch.h"
#include "core/engine.h"
#include "memory/memory_block.h"
#include "memory/memory_manager.h"

enum ReturnCode : int {
	OK = 0,
	FATAL_ERROR = -1,
	BAD_ARGUMENTS = -2,
	BAD_ALLOC = -3
};

namespace rglz {
	extern void register_client_app();
	extern std::pair<std::size_t, std::size_t> memory_profile_default_and_min_alloc_size();
} // namespace rglz

static constexpr std::uint32_t s_max_option_lenght = 19;

static const char* cmd_option_value(int argc, const char** argv, const char* option_name) {
	std::uint32_t option_value_index = 0;
	for (std::uint32_t i = 0; i < argc; ++i) {
		if (std::strncmp(argv[i], option_name, s_max_option_lenght) == 0) {
			option_value_index = i + 1;
			break;
		}
	}
	if (option_value_index != 0 && option_value_index < argc) {
		return argv[option_value_index];
	}

	return nullptr;
}

int main(int argc, const char** argv) {
	std::pair<std::size_t, std::size_t> memory_profile = rglz::memory_profile_default_and_min_alloc_size();
	std::size_t allocation_size = memory_profile.first;
	std::size_t min_allocation_size = memory_profile.second;

	{
		const char* allocation_size_str{};

		if ((allocation_size_str = cmd_option_value(argc, argv, "-m")) != nullptr) {
			std::array<char, s_max_option_lenght> tmp_str{};
			char* end_ptr{};
			std::size_t tmp_allocation_size{};

			std::strncpy(tmp_str.data(), allocation_size_str, s_max_option_lenght);
			tmp_allocation_size = std::strtol(tmp_str.data(), &end_ptr, 10);

			if ((tmp_allocation_size != 0U) && errno != ERANGE) {
				allocation_size = tmp_allocation_size;
			}
		}

		if (allocation_size < min_allocation_size) {
			return BAD_ARGUMENTS;
		}
	}

	rglz::memory::MemoryBlock memory_block(allocation_size);
	if (memory_block.is_bad_alloc()) {
		return BAD_ALLOC;
	}

	const std::uint8_t stack_allocator_size_percentage = 70;
	const std::uint8_t single_frame_allocator_size_percentage = 30;

	rglz::memory::MemoryManager::startup(memory_block, stack_allocator_size_percentage, single_frame_allocator_size_percentage);

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
