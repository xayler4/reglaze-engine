#include "core/asset_manager.h"

namespace rglz {
	AssetManager::AssetManager(bool is_packed, std::string_view archive_path) 
		: m_is_packed(is_packed)
	{
		if (m_is_packed) {
			m_file.create(archive_path, std::ios::binary);
			if (m_file.get().is_open()) {
				esr::ReadStream<CustomStream> istream(m_file);
				istream >> m_archive;

			}

			RGLZ_ENGINE_LOG_ERROR("Failed to open archive file at: ", archive_path);
		}
	}

	AssetManager::~AssetManager() {
		if (m_is_packed) {
			m_file.destroy();
		}
	}

	Image AssetManager::load_image(std::string_view file_path) {
		Image out_image;
		open_file(file_path);

		close_file();
		return out_image;
	}


	bool AssetManager::open_file(std::string_view file_path) {
		if (!m_is_packed) {
			m_file.create(file_path.data(), std::ios::binary);

			if (m_file.get().is_open()) {
				return true;
			}

			RGLZ_ENGINE_LOG_ERROR("Failed to open file at: ", file_path);
			return false;
		}
		else if (m_file.get().is_open()){
			std::size_t* offset = m_archive.file_table[file_path];
			if (offset != nullptr) {
				m_file.get().seekg(*offset);
				return true;
			}

			RGLZ_ENGINE_LOG_ERROR("Failed to find ", file_path, " in archive");
			return false;
		}

		return false;
	}

	void AssetManager::close_file() {
		if (m_is_packed) {
			return;
		}

		m_file.destroy();
	}
}
