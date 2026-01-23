#include "core/resource.h"

namespace rglz {
	ResourcePath(const FixedString<128>& path) {
		
	}

	bool ResourcePath::is_path_valid(const FixedString<128>& path) {
		bool slash_found = true;
		bool back_slash_found = false;
		char prev_c = path[0];

		if (prev_c == '/') {
			slash_found = true;				
		}
		if (prev_c == '\\') {
			back_slash_found = true;				
		}
		for (std::uint8_t i = 1; i < path.length(); i++) {
			char c = path[i];
			prev_c = path[i - 1];

			if (c == '/') {
				RGLZ_ASSERT(back_slash_found);
				slash_found = true;				
			}
			else if (c == '\\') {
				RGLZ_ASSERT(slash_found);
				back_slash_found = true;				
			}
		}
	}

	Resource::Resource(const ResourcePath& resource_path) {
		
	}
}
