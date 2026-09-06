#include "core/asset_manager.h"
#include <external/stb_image.h>

static int stream_read(void* user, char* data, int size);
static void stream_skip(void* user, int n);
static int stream_eof(void* user);

namespace rglz {
	std::mutex AssetManager::s_mutex;

	AssetManager::AssetManager(bool is_packed, std::string_view archive_path) 
		: m_is_packed(is_packed)
	{
		if (m_is_packed) {
			m_file.create(archive_path.data(), std::ios::binary);
			if (m_file.get().is_open()) {
				esr::ReadStream<CustomStream> istream(m_file);
				istream >> m_archive;

			}
			else {
				RGLZ_ENGINE_LOG_ERROR("Failed to open archive file at: ", archive_path);
			}

		}
	}

	AssetManager::~AssetManager() {
		if (m_is_packed) {
			m_file.destroy();
		}
	}

	Image AssetManager::load_image(std::string_view file_path) {
		static const stbi_io_callbacks callbacks {
			stream_read,
			stream_skip,
			stream_eof
		};

		Image out_image{};
		if (!open_file(file_path)) {
			return out_image;
		}
		
		out_image.data = stbi_load_from_callbacks(&callbacks, &m_file.get(), &out_image.width, &out_image.height, &out_image.n_channels, 0);
		
		close_file();
		return out_image;
	}


	bool AssetManager::open_file(std::string_view file_path) {
		s_mutex.lock();
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
		s_mutex.unlock();
	}
}

static int stream_read(void* user, char* data, int size) {
    auto* stream = static_cast<std::istream*>(user);
    stream->read(data, size);
    return static_cast<int>(stream->gcount());
}

static void stream_skip(void* user, int n) {
    auto* stream = static_cast<std::istream*>(user);
    stream->seekg(n, std::ios_base::cur);
}

static int stream_eof(void* user) {
    auto* stream = static_cast<std::istream*>(user);
    return stream->eof() ? 1 : 0;
}
