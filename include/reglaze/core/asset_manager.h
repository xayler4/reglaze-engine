#ifndef RGLZ_ASSET_MANAGER_H
#define RGLZ_ASSET_MANAGER_H

#include "base.h"
#include "core/image.h"
#include "core/manual_lifetime.h"
#include "containers/fixed_vector.h"
#include "containers/fixed_string.h"
#include "easer/easer.h"
#include <mutex>

#define ASSET_MANAGER_ESR_CHANNEL "asset_manager"

namespace rglz {
	class AssetManager {
	public:
		struct ArchiveHeader {
			ESR_BEGIN();
			ESR_END();
		};
		struct ArchiveFileTable {
			ESR_BEGIN();
			ESR_FIELD(ArchiveHeader, header, ASSET_MANAGER_ESR_CHANNEL);
			ESR_FIELD(ESR_PACK(FixedHashMap<FixedString<RGLZ_FILE_PATH_MAX_LENGTH>, std::size_t, RGLZ_ASSET_MANAGER_MAX_FILES>), file_table, ASSET_MANAGER_ESR_CHANNEL);

			ESR_END();
		};
		struct CustomStream : esr::Stream<std::ios> {

			static consteval std::string_view get_channel() {
				return ASSET_MANAGER_ESR_CHANNEL;
			}
		};
	public:
		AssetManager(bool is_packed, std::string_view archive_path = "");
		~AssetManager();

		Image load_image(std::string_view file_path);

	private:
		bool open_file(std::string_view file_path);
		void close_file();

	private:
		const bool m_is_packed;
		ManualLifetime<std::ifstream> m_file;
		ArchiveFileTable m_archive;
		static std::mutex s_mutex;
	};
}

template<std::size_t S, ESR_REGISTRY_PARAMS>
ESR_REGISTER_PROC_WR(ASSET_MANAGER_ESR_CHANNEL, rglz::FixedString<S>, s, stream, 
		{
			stream << s.length();
			for (auto& data : s) {
				stream << data;
			}
		},
		{
			std::size_t length;
			stream >> length;
			for (uint8_t i = 0; i < length; i++) {
				char data;
				stream >> data;				
				s += data;
			}
		});

template<ESR_REGISTRY_PARAMS>
ESR_REGISTER_PROC_WR(ASSET_MANAGER_ESR_CHANNEL, ESR_PACK(rglz::FixedHashMap<rglz::FixedString<RGLZ_FILE_PATH_MAX_LENGTH>, std::size_t, RGLZ_ASSET_MANAGER_MAX_FILES>), map, stream, 
		{
			stream << map.size();
			for (auto& data : map) {
				stream << data.value;
			}
		},
		ESR_PACK({
			std::size_t size;
			stream >> size;
			for (uint16_t i = 0; i < size; i++) {
				rglz::FixedString<RGLZ_FILE_PATH_MAX_LENGTH> key;
				std::size_t data;

				stream >> key;				
				stream >> data;

				map.insert(key, data);
			}
		}));


#endif	// RGLZ_ASSET_MANAGER_H
