#include <iostream>

namespace rglz {
	template<std::size_t ULogLenght, std::size_t UMaxCuncurrentLogInstances>
	Logger<ULogLenght, UMaxCuncurrentLogInstances>::Logger(std::string_view file_name, std::string_view logger_name) : 
		m_log_prefix(logger_name.length() + 5),
		m_file(file_name.data(), std::ios_base::binary),
		m_buffered_write_streams(make_array(std::make_index_sequence<UMaxCuncurrentLogInstances>{})),
		m_file_write_stream(m_file),
		m_stdout_write_stream(std::cout) {

		m_log_prefix[0] = '[';
		std::memcpy(m_log_prefix.data() + 1, logger_name.data(), logger_name.length());
		m_log_prefix[logger_name.length() + 1] = ' ';
		m_log_prefix[logger_name.length() + 2] = '-';
		m_log_prefix[logger_name.length() + 3] = '>';
		m_log_prefix[logger_name.length() + 4] = ' ';
	}

	template<std::size_t ULogLenght, std::size_t UMaxCuncurrentLogInstances>
	template<std::size_t... USeq>
	std::array<typename Logger<ULogLenght, UMaxCuncurrentLogInstances>::BufferedWriteStream, UMaxCuncurrentLogInstances> Logger<ULogLenght, UMaxCuncurrentLogInstances>::make_array(std::index_sequence<USeq...>) {
		return {(BufferedWriteStream{esr::WriteStream<BufferedLogStream>(m_data[USeq].data(), m_data[USeq].size()), false})...};
	}

	// TODO: assign first non-busy buffered_write_stream to log
	//
	template<std::size_t ULogLenght, std::size_t UMaxCuncurrentLogInstances>
	Logger<ULogLenght, UMaxCuncurrentLogInstances>::Log Logger<ULogLenght, UMaxCuncurrentLogInstances>::log(LoggerSeverity severity) {
		std::uint32_t index = 0;
		bool expected = false;

		while (!m_buffered_write_streams[index].second.compare_exchange_weak(expected, true)) {
			if (++index == UMaxCuncurrentLogInstances) {
				index = 0;
			}

			expected = false;
		}
		m_buffered_write_streams[index].first << m_log_prefix << severity << "]: ";

		return Log(this, severity, index);
	}

	template<std::size_t ULogLenght, std::size_t UMaxCuncurrentLogInstances>
	template<typename T>
	void Logger<ULogLenght, UMaxCuncurrentLogInstances>::log(const Log& log, const T& value) {
		RGLZ_ENGINE_ASSERT(log.m_logger != nullptr);
		m_buffered_write_streams[log.write_buffer_index()].first << value;
	}

	template<std::size_t ULogLenght, std::size_t UMaxCuncurrentLogInstances>
	void Logger<ULogLenght, UMaxCuncurrentLogInstances>::end_log(Log& log) {
		RGLZ_ENGINE_ASSERT(log.m_logger != nullptr);
		m_buffered_write_streams[log.write_buffer_index()].first << "\n";
		m_buffered_write_streams[log.write_buffer_index()].second = false;
		log.m_logger = nullptr;
	}

	template<std::size_t ULogLenght, std::size_t UMaxCuncurrentLogInstances>
	void Logger<ULogLenght, UMaxCuncurrentLogInstances>::flush_logs() {
		for (std::uint32_t i = 0; i < m_buffered_write_streams.size(); i++) {
			auto& s = m_buffered_write_streams[i];
			auto& d = m_data[i];
			std::uint32_t size = s.first.get_write_ptr() - d.data();

			m_file_write_stream.write(d.data(), size);
			m_stdout_write_stream.write(d.data(), size);

			s.first = esr::WriteStream<BufferedLogStream>(d.data(), d.size());
		}
	}
}
