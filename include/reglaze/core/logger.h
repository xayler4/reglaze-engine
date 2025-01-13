#ifndef RGLZ_LOGGER_H
#define RGLZ_LOGGER_H

#include "base.h"
#include "easer/easer.h"
#include <string_view>
#include <charconv>
#include <atomic>
#include <system_error>
#include <array>
#include <iostream>

namespace rglz {
	class BufferedLogStream : public esr::Stream<BufferedLogStream> {
	public:
		static consteval std::string_view get_channel() {
			return "Logger";
		}
	};

	enum class LoggerSeverity {
		Debug = 0,
		Trace,
		Info,
		Warn,
		Error,
		Fatal
	};

	template<std::size_t ULogLenght, std::size_t UMaxCuncurrentLogInstances>
	class Logger;

	template<std::size_t ULogLenght = 512, std::size_t UMaxCuncurrentLogInstances = 3>
	class Logger {
	public:
		using BufferedWriteStream = std::pair<esr::WriteStream<BufferedLogStream>, std::atomic<bool>>;

		class Log {
		public:
			~Log() {
				if (m_logger != nullptr) {
					m_logger->end_log(*this);
				}
			}

			Log(Log&& log) : 
				m_logger(log.m_logger),
				m_severity(log.m_severity),
				m_write_buffer_index(log.m_write_buffer_index)
			{
				log.m_logger = nullptr;
			} 

			inline Log& operator= (Log&& log) {
				m_logger = log.m_logger;
				m_severity = log.m_severity;
				m_write_buffer_index = log.m_write_buffer_index;

				log.m_logger = nullptr;

				return *this;
			}

			inline std::uint32_t write_buffer_index() const {
				return m_write_buffer_index;
			}

			template<typename T>
			inline Log operator<< (const T& value) {
				return (m_logger->log(std::move(*this), value));
			}

			Log(const Log&) = delete;
			Log& operator= (const Log&) = delete;
			
		private:
			Log(Logger<ULogLenght, UMaxCuncurrentLogInstances>* logger, LoggerSeverity severity, std::uint32_t write_buffer_index) :
				m_logger(logger),
				m_severity(severity),
				m_write_buffer_index(write_buffer_index) {

			}

		private:
			Logger<ULogLenght, UMaxCuncurrentLogInstances>* m_logger;
			LoggerSeverity m_severity;
			std::uint32_t m_write_buffer_index;

			friend class Logger;
		};

	public:
		Logger(const char* file_name, const char* logger_name);

		inline Log log(LoggerSeverity severity);
		inline void end_log(const Log& log);
		inline void flush();

		template<typename T>
		inline Log log(Log&& log, const T& value);

	private:
		template<std::size_t... USeq>
		std::array<BufferedWriteStream, UMaxCuncurrentLogInstances> make_array(std::index_sequence<USeq...>);

	private:
		const char* m_name;
		std::ofstream m_file;
		std::array<std::array<std::uint8_t, ULogLenght>, UMaxCuncurrentLogInstances> m_data;
		std::array<BufferedWriteStream, UMaxCuncurrentLogInstances> m_buffered_write_streams;
		esr::WriteStream<> m_file_write_stream;
		esr::WriteStream<> m_stdout_write_stream;
	};
}

template<ESR_REGISTRY_PARAMS>
ESR_REGISTER_PROC_W("Logger", const char*, str, stream,
	ESR_PACK({
		std::uint32_t i = 0;
		while (str[i] != '\0') {
			std::uint8_t uc = static_cast<std::uint8_t>(str[i]);
			stream.write(&uc, sizeof(uc));
			++i;
		}
	})
);

template<ESR_REGISTRY_PARAMS>
ESR_REGISTER_PROC_W("Logger", char*, str, stream,
	ESR_PACK({
		std::uint32_t i = 0;
		while (str[i] != '\0') {
			std::uint8_t uc = static_cast<std::uint8_t>(str[i]);
			stream.write(&uc, sizeof(uc));
			++i;
		}
	})
);

template<ESR_REGISTRY_PARAMS>
ESR_REGISTER_PROC_W("Logger", char, c, stream,
	ESR_PACK({
		if (c != '\0') {
			std::uint8_t uc = static_cast<std::uint8_t>(c);
			stream.write(&uc, sizeof(uc));
		}
	})
);

template<typename T>
concept Number = (std::is_arithmetic_v<T> && !std::is_same_v<std::remove_cvref_t<T>, char>);

template<Number T, ESR_REGISTRY_PARAMS>
ESR_REGISTER_PROC_W("Logger", T, v, stream, 
	ESR_PACK({
		std::array<char, 32> buffer;

		auto result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), v);
		if (result.ec == std::errc()) {
			std::string_view str(buffer.data(), result.ptr - buffer.data());
			stream << str;
		}
	})
);

template<ESR_REGISTRY_PARAMS>
ESR_REGISTER_PROC_W("Logger", rglz::LoggerSeverity, s, stream,
	ESR_PACK({
		switch(s) {
			case rglz::LoggerSeverity::Debug:
				stream << "DEBUG";
				break;
			case rglz::LoggerSeverity::Trace:
				stream << "TRACE";
				break;
			case rglz::LoggerSeverity::Info:
				stream << "INFO";
				break;
			case rglz::LoggerSeverity::Warn:
				stream << "WARN";
				break;
			case rglz::LoggerSeverity::Error:
				stream << "ERROR";
				break;
			case rglz::LoggerSeverity::Fatal:
				stream << "FATAL";
				break;
		}
	})
);

#include "core/logger.tpp"

#endif
