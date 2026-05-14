#ifndef RGLZ_RESOURCE_H
#define RGLZ_RESOURCE_H

#include "base.h"
#include "containers/fixed_vector.h"

namespace rglz {
	class ResourcePath {
	public:
		ResourcePath(const FixedString<128>& path);

	private:
		bool is_path_valid(const FixedString<128>& path);

	private:
		FixedString<128> m_path;
	};

	class Resource {
	public:
		Resource(const ResourcePath& resource_path);
		virtual ~Resource();

		virtual bool load();
	private:

	};

	class Resource
}

#endif	// RGLZ_RESOURCE_H
