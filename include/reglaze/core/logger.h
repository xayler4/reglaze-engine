#ifndef RGLZ_LOGGER_H
#define RGLZ_LOGGER_H

#include "base.h"
#include "easer/easer.h"
#include "containers/fixed_string.h"
#include <charconv>
#include <atomic>
#include <array>

// Engine logger

#if RGLZ_ENGINE_LOGGER_DEBUG_ENABLE == 1
	#define RGLZ_ENGINE_LOG_DEBUG(...) (rglz::Engine::log(rglz::LoggerSeverity::Debug).stream(__VA_ARGS__))
	#define RGLZ_ENGINE_LOG_DEBUG_GET(log_name, ...) auto log_name = (rglz::Engine::log(rglz::LoggerSeverity::Debug).stream( __VA_ARGS__))
	#define RGLZ_ENGINE_LOG_DEBUG_TO(log_name, ...)	\
		RGLZ_ENGINE_ASSERT_MSG(log_name.severity() == rglz::LoggerSeverity::Debug && log_name.logger() == rglz::Engine::logger(), "Log is not owned by engine logger!");	\
		log_name.stream(__VA_ARGS__);
#else
	#define RGLZ_ENGINE_LOG_DEBUG(...)
	#define RGLZ_ENGINE_LOG_DEBUG_GET(log_name, ...)
	#define RGLZ_ENGINE_LOG_DEBUG_TO(log_name, ...)
#endif

#if RGLZ_ENGINE_LOGGER_TRACE_ENABLE == 1
	#define RGLZ_ENGINE_LOG_TRACE(...) (rglz::Engine::log(rglz::LoggerSeverity::Trace).stream(__VA_ARGS__))
	#define RGLZ_ENGINE_LOG_TRACE_GET(log_name, ...) auto log_name = (rglz::Engine::log(rglz::LoggerSeverity::Trace).stream( __VA_ARGS__))
	#define RGLZ_ENGINE_LOG_TRACE_TO(log_name, ...)	\
		RGLZ_ENGINE_ASSERT_MSG(log_name.severity() == rglz::LoggerSeverity::Trace && log_name.logger() == rglz::Engine::logger(), "Log is not owned by engine logger!");	\
		log_name.stream(__VA_ARGS__);
#else
	#define RGLZ_ENGINE_LOG_TRACE(...)
	#define RGLZ_ENGINE_LOG_TRACE_GET(log_name, ...)
	#define RGLZ_ENGINE_LOG_TRACE_TO(log_name, ...)
#endif
	
#if RGLZ_ENGINE_LOGGER_INFO_ENABLE == 1
	#define RGLZ_ENGINE_LOG_INFO(...) (rglz::Engine::log(rglz::LoggerSeverity::Info).stream(__VA_ARGS__))
	#define RGLZ_ENGINE_LOG_INFO_GET(log_name, ...) auto log_name = (rglz::Engine::log(rglz::LoggerSeverity::Info).stream( __VA_ARGS__))
	#define RGLZ_ENGINE_LOG_INFO_TO(log_name, ...)	\
		RGLZ_ENGINE_ASSERT_MSG(log_name.severity() == rglz::LoggerSeverity::Info && log_name.logger() == rglz::Engine::logger(), "Log is not owned by engine logger!");	\
		log_name.stream(__VA_ARGS__);
#else
	#define RGLZ_ENGINE_LOG_INFO(...)
	#define RGLZ_ENGINE_LOG_INFO_GET(log_name, ...)
	#define RGLZ_ENGINE_LOG_INFO_TO(log_name, ...)
#endif
	
#if RGLZ_ENGINE_LOGGER_WARN_ENABLE == 1
	#define RGLZ_ENGINE_LOG_WARN(...) (rglz::Engine::log(rglz::LoggerSeverity::Warn).stream(__VA_ARGS__))
	#define RGLZ_ENGINE_LOG_WARN_GET(log_name, ...) auto log_name = (rglz::Engine::log(rglz::LoggerSeverity::Warn).stream( __VA_ARGS__))
	#define RGLZ_ENGINE_LOG_WARN_TO(log_name, ...)	\
		RGLZ_ENGINE_ASSERT_MSG(log_name.severity() == rglz::LoggerSeverity::Warn && log_name.logger() == rglz::Engine::logger(), "Log is not owned by engine logger!");	\
		log_name.stream(__VA_ARGS__);
#else
	#define RGLZ_ENGINE_LOG_WARN(...)
	#define RGLZ_ENGINE_LOG_WARN_GET(log_name, ...)
	#define RGLZ_ENGINE_LOG_WARN_TO(log_name, ...)
#endif
	
#if RGLZ_ENGINE_LOGGER_ERROR_ENABLE == 1
	#define RGLZ_ENGINE_LOG_ERROR(...) (rglz::Engine::log(rglz::LoggerSeverity::Info).stream(__VA_ARGS__))
	#define RGLZ_ENGINE_LOG_ERROR_GET(log_name, ...) auto log_name = (rglz::Engine::log(rglz::LoggerSeverity::Info).stream( __VA_ARGS__))
	#define RGLZ_ENGINE_LOG_ERROR_TO(log_name, ...)	\
		RGLZ_ENGINE_ASSERT_MSG(log_name.severity() == rglz::LoggerSeverity::Info && log_name.logger() == rglz::Engine::logger(), "Log is not owned by engine logger!");	\
		log_name.stream(__VA_ARGS__);
#else
	#define RGLZ_ENGINE_LOG_ERROR(...)
	#define RGLZ_ENGINE_LOG_ERROR_GET(log_name, ...)
	#define RGLZ_ENGINE_LOG_ERROR_TO(log_name, ...)
#endif

// App logger

#if RGLZ_LOGGER_DEBUG_ENABLE == 1
	#define RGLZ_LOG_DEBUG(...) (rglz::Engine::app().log(rglz::LoggerSeverity::Debug).stream(__VA_ARGS__))
	#define RGLZ_LOG_DEBUG_GET(log_name, ...) auto log_name = (rglz::Engine::app().log(rglz::LoggerSeverity::Debug).stream( __VA_ARGS__))
	#define RGLZ_LOG_DEBUG_TO(log_name, ...)	\
		RGLZ_ASSERT_MSG(log_name.severity() == rglz::LoggerSeverity::Debug && log_name.logger() == &rglz::Engine::app(), "Log is not owned by app logger!");	\
		rglz::Engine::app().flush_logs();	\
		log_name.stream(__VA_ARGS__)
#else
	#define RGLZ_LOG_DEBUG(...)
	#define RGLZ_LOG_DEBUG_GET(log_name, ...)
	#define RGLZ_LOG_DEBUG_TO(log_name, ...)
#endif


#if RGLZ_LOGGER_TRACE_ENABLE == 1
	#define RGLZ_LOG_TRACE(...) (rglz::Engine::app().log(rglz::LoggerSeverity::Trace).stream(__VA_ARGS__))
	#define RGLZ_LOG_TRACE_GET(log_name, ...) auto log_name = (rglz::Engine::app().log(rglz::LoggerSeverity::Trace).stream( __VA_ARGS__))
	#define RGLZ_LOG_TRACE_TO(log_name, ...)	\
		RGLZ_ASSERT_MSG(log_name.severity() == rglz::LoggerSeverity::Trace && log_name.logger() == &rglz::Engine::app(), "Log is not owned by app logger!");	\
		rglz::Engine::app().flush_logs();	\
		log_name.stream(__VA_ARGS__)
#else
	#define RGLZ_LOG_TRACE(...)
	#define RGLZ_LOG_TRACE_GET(log_name, ...)
	#define RGLZ_LOG_TRACE_TO(log_name, ...)
#endif
	

#if RGLZ_LOGGER_INFO_ENABLE == 1
	#define RGLZ_LOG_INFO(...) (rglz::Engine::app().log(rglz::LoggerSeverity::Info).stream(__VA_ARGS__))
	#define RGLZ_LOG_INFO_GET(log_name, ...) auto log_name = (rglz::Engine::app().log(rglz::LoggerSeverity::Info).stream( __VA_ARGS__))
	#define RGLZ_LOG_INFO_TO(log_name, ...)	\
		RGLZ_ASSERT_MSG(log_name.severity() == rglz::LoggerSeverity::Info && log_name.logger() == &rglz::Engine::app(), "Log is not owned by app logger!");	\
		rglz::Engine::app().flush_logs();	\
		log_name.stream(__VA_ARGS__)
#else
	#define RGLZ_LOG_INFO(...)
	#define RGLZ_LOG_INFO_GET(log_name, ...)
	#define RGLZ_LOG_INFO_TO(log_name, ...)
#endif

#if RGLZ_LOGGER_WARN_ENABLE == 1
	#define RGLZ_LOG_WARN(...) (rglz::Engine::app().log(rglz::LoggerSeverity::Warn).stream(__VA_ARGS__))
	#define RGLZ_LOG_WARN_GET(log_name, ...) auto log_name = (rglz::Engine::app().log(rglz::LoggerSeverity::Warn).stream( __VA_ARGS__))
	#define RGLZ_LOG_WARN_TO(log_name, ...)	\
		RGLZ_ASSERT_MSG(log_name.severity() == rglz::LoggerSeverity::Warn && log_name.logger() == &rglz::Engine::app(), "Log is not owned by app logger!");	\
		rglz::Engine::app().flush_logs();	\
		log_name.stream(__VA_ARGS__)
#else
	#define RGLZ_LOG_WARN(...)
	#define RGLZ_LOG_WARN_GET(log_name, ...)
	#define RGLZ_LOG_WARN_TO(log_name, ...)
#endif
	
#if RGLZ_LOGGER_ERROR_ENABLE == 1
	#define RGLZ_LOG_ERROR(...) (rglz::Engine::app().log(rglz::LoggerSeverity::Error).stream(__VA_ARGS__))
	#define RGLZ_LOG_ERROR_GET(log_name, ...) auto log_name = (rglz::Engine::app().log(rglz::LoggerSeverity::Error).stream( __VA_ARGS__))
	#define RGLZ_LOG_ERROR_TO(log_name, ...)	\
		RGLZ_ASSERT_MSG(log_name.severity() == rglz::LoggerSeverity::Error && log_name.logger() == &rglz::Engine::app(), "Log is not owned by app logger!");	\
		rglz::Engine::app().flush_logs();	\
		log_name.stream(__VA_ARGS__)
#else
	#define RGLZ_LOG_ERROR(...)
	#define RGLZ_LOG_ERROR_GET(log_name, ...)
	#define RGLZ_LOG_ERROR_TO(log_name, ...)
#endif


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

	class Alignln {
	};

	constexpr Alignln alignln;

	template<std::size_t ULogLenght, std::size_t UMaxCuncurrentLogInstances>
	class Logger;

	template<std::size_t ULogLenght = 512, std::size_t UMaxCuncurrentLogInstances = 3>
	class Logger {
	public:
		using BufferedWriteStream = std::pair<esr::WriteStream<BufferedLogStream>, std::atomic<bool>>;
		using LoggerType = Logger<ULogLenght, UMaxCuncurrentLogInstances>;

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
			inline Log&& operator<< (const T& value) {
				m_logger->log(*this, value);

				return std::move(*this);
			}

			inline Log&& operator<< (Alignln) {
				std::uint32_t nspaces = m_logger->log_prefix_length() + map_severity_to_length() + 3;
				m_logger->log(*this, '\n');
				for (std::uint32_t i = 0; i < nspaces; i++) {
					m_logger->log(*this, ' ');
				}
				
				return std::move(*this);
			}

			template<typename TFirst, typename... TArgs>
			inline Log&& stream(const TFirst& value, const TArgs&... rest) {
				*this << value;

				return stream(rest...);
			}

			template<typename TLast>
			inline Log&& stream(const TLast& value) {
				*this << value;

				return std::move(*this);
			}

			LoggerSeverity severity() const {
				return m_severity;
			}

			const LoggerType* logger() const {
				return m_logger;
			}

			Log(const Log&) = delete;
			Log& operator= (const Log&) = delete;
			
		private:
			Log(Logger<ULogLenght, UMaxCuncurrentLogInstances>* logger, LoggerSeverity severity, std::uint32_t write_buffer_index) :
				m_logger(logger),
				m_severity(severity),
				m_write_buffer_index(write_buffer_index) {

			}

			std::uint8_t map_severity_to_length() const {
				switch (m_severity) {
				
					case rglz::LoggerSeverity::Debug: {
						constexpr std::string_view str = "DEBUG";
						return str.length();
					}
					case rglz::LoggerSeverity::Trace: {
						constexpr std::string_view str = "TRACE";
						return str.length();
					}
					case rglz::LoggerSeverity::Info: {
						constexpr std::string_view str = "INFO";
						return str.length();
					}
					case rglz::LoggerSeverity::Warn: {
						constexpr std::string_view str = "WARN";
						return str.length();
					}
					case rglz::LoggerSeverity::Error: {
						constexpr std::string_view str = "ERROR";
						return str.length();
					}
					case rglz::LoggerSeverity::Fatal: {
						constexpr std::string_view str = "FATAL";
						return str.length();
					}
					default:
						return 0;
				}
			}

		private:
			LoggerType* m_logger;
			LoggerSeverity m_severity;
			std::uint32_t m_write_buffer_index;

			friend class Logger;
		};

	public:
		Logger(std::string_view file_name, std::string_view logger_name);

		inline Log log(LoggerSeverity severity);
		inline void end_log(Log& log);
		inline void flush_logs();
		inline std::uint32_t log_prefix_length() const {
			return m_log_prefix.length();
		}

		template<typename T>
		inline void log(const Log& log, const T& value);

	private:
		template<std::size_t... USeq>
		std::array<BufferedWriteStream, UMaxCuncurrentLogInstances> make_array(std::index_sequence<USeq...>);

	private:
		FixedString<> m_log_prefix;
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
