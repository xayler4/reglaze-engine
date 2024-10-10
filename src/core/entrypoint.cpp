#include "base.h"
#include "core/engine.h"
#include "memory/memory_manager.h"
#include <cstring>
#include <cstdlib>

namespace rglz {
	extern void register_client_app();
	extern const std::size_t memory_profile_default_mem_alloc_size();
	extern const std::size_t memory_profile_min_mem_alloc_size();
}

static constexpr std::uint32_t s_max_option_lenght = 64;

const char* cmd_option_value(int argc, const char** argv, const char* option_name) {
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

	std::uint32_t total_allocation_size = rglz::memory_profile_default_mem_alloc_size();

	{
		const char* total_allocation_size_str;

		if (total_allocation_size_str = cmd_option_value(argc, argv, "-m")) {
			char tmp_str[s_max_option_lenght];
			char* end_ptr;
			std::uint32_t tmp_total_allocation_size;

			std::strncpy(tmp_str, total_allocation_size_str, s_max_option_lenght);
			
			if (tmp_total_allocation_size && errno != ERANGE) {
				total_allocation_size = tmp_total_allocation_size;
			}
		}

		if (total_allocation_size < rglz::memory_profile_min_mem_alloc_size()) {
			return -1;
		}
	}

	rglz::memory::MemoryBlock memory_block(total_allocation_size);

	rglz::memory::MemoryManager::startup(memory_block);
	rglz::register_client_app();
	rglz::Engine::startup();
	rglz::Engine::run();
	rglz::Engine::shutdown();
	rglz::memory::MemoryManager::shutdown();

	return 0;
}
