#include "containers/fixed_hash_map.h"

namespace rglz {
	std::size_t to_hash(std::string_view str) {
		std::size_t hash = 2166136261u;  // FNV offset basis (32-bit)
        for (char c : str) {
            hash ^= static_cast<unsigned char>(c);
            hash *= 16777619u;  // FNV prime (32-bit)
        }
        return hash;
	}
}
