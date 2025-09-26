#include <iostream>

namespace rglz {

	template<std::size_t UMaxCuncurrentLogInstances>
	Logger<UMaxCuncurrentLogInstances>::Logger(std::string_view file_name, std::string_view logger_name) 
		: m_log_prefix(logger_name.length() + 6)
		, m_file(file_name.data(), std::ios_base::binary)
		, m_buffered_log_streams(make_buffered_log_streams(std::make_index_sequence<UMaxCuncurrentLogInstances>{}))
		, m_buffered_log_write_streams(make_buffered_log_write_streams(std::make_index_sequence<UMaxCuncurrentLogInstances>{}))
		, m_file_write_stream(m_file)
		, m_stdout_write_stream(std::cout) 

	{
		m_log_prefix[0] = '\n';
		m_log_prefix[1] = '[';
		std::memcpy(m_log_prefix.data() + 2, logger_name.data(), logger_name.length());
		m_log_prefix[logger_name.length() + 2] = ' ';
		m_log_prefix[logger_name.length() + 3] = '-';
		m_log_prefix[logger_name.length() + 4] = '>';
		m_log_prefix[logger_name.length() + 5] = ' ';
	}

	template<std::size_t UMaxCuncurrentLogInstances>
	Logger<UMaxCuncurrentLogInstances>::~Logger() {
		static std::atomic<bool> stdout_newline_printed = false;
		std::uint8_t newline = '\n';
		bool expected = false;
		if (stdout_newline_printed.compare_exchange_weak(expected, true)) {
			m_stdout_write_stream.write(&newline, sizeof(newline));
		}
		m_file_write_stream.write(&newline, sizeof(newline));
	}

	template<std::size_t UMaxCuncurrentLogInstances>
	template<std::size_t... USeq>
	std::array<BufferedLogStream, UMaxCuncurrentLogInstances> Logger<UMaxCuncurrentLogInstances>::make_buffered_log_streams(std::index_sequence<USeq...>) {
		return {m_data[USeq]...};
	}

	template<std::size_t UMaxCuncurrentLogInstances>
	template<std::size_t... USeq>
	std::array<BufferedLogWriteStream, UMaxCuncurrentLogInstances> Logger<UMaxCuncurrentLogInstances>::make_buffered_log_write_streams(std::index_sequence<USeq...>) {
		return {(BufferedLogWriteStream{esr::WriteStream<BufferedLogStream>(m_buffered_log_streams[USeq]), false})...};
	}

	// TODO: assign first non-busy buffered_write_stream to log
	//
	template<std::size_t UMaxCuncurrentLogInstances>
	Logger<UMaxCuncurrentLogInstances>::Log Logger<UMaxCuncurrentLogInstances>::log(LoggerSeverity severity) {
		std::uint32_t index = 0;
		bool expected = false;

		while (!m_buffered_log_write_streams[index].second.compare_exchange_weak(expected, true)) {
			if (++index == UMaxCuncurrentLogInstances) {
				index = 0;
			}

			expected = false;
		}
		m_buffered_log_write_streams[index].first << m_log_prefix << severity << "]: ";

		return Log(this, severity, index);
	}

	template<std::size_t UMaxCuncurrentLogInstances>
	template<typename T>
	void Logger<UMaxCuncurrentLogInstances>::log(const Log& log, const T& value) {
		RGLZ_ENGINE_ASSERT(log.m_logger != nullptr);
		m_buffered_log_write_streams[log.write_buffer_index()].first << value;
	}

	template<std::size_t UMaxCuncurrentLogInstances>
	void Logger<UMaxCuncurrentLogInstances>::end_log(Log& log) {
		RGLZ_ENGINE_ASSERT(log.m_logger != nullptr);
		m_buffered_log_write_streams[log.write_buffer_index()].second = false;
		log.m_logger = nullptr;
	}

	template<std::size_t UMaxCuncurrentLogInstances>
	void Logger<UMaxCuncurrentLogInstances>::flush_logs() {
		for (std::uint32_t i = 0; i < m_buffered_log_streams.size(); i++) {
			auto& s = m_buffered_log_streams[i];
			auto& ws = m_buffered_log_write_streams[i];
			auto& d = m_data[i];

			m_file_write_stream.write(d.data, ws.first.get_write_idx());
			m_stdout_write_stream.write(d.data, ws.first.get_write_idx());

			d = LogBuffer();
			s = BufferedLogStream(d);
			ws.first.set_write_idx(0);
		}
	}
}
