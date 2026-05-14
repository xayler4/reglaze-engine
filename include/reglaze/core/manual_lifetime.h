#ifndef RGLZ_MANUAL_LIFETIME_H
#define RGLZ_MANUAL_LIFETIME_H

#include "base.h"

namespace rglz {
	template<typename T>
	class ManualLifetime {
	public:		
		template<typename... TArgs>
		void create(TArgs&&... args) {
			std::construct_at(ptr(), std::forward<TArgs>(args)...);
		}

		void destroy() {
			std::destroy_at(ptr());
		}

		inline T& get() {
			return *ptr();
		}

		inline const T& get() const {
			return *ptr();
		}

		inline operator T&() {
			return *ptr();
		}

		inline operator const T&() const {
			return *ptr();
		}

	private:
		inline T* ptr() {
			return std::launder(reinterpret_cast<T*>(m_data.data()));
		}

		inline const T* ptr() const {
			return std::launder(reinterpret_cast<const T*>(m_data.data()));
		}

	private:
		alignas(T) std::array<uint8_t, sizeof(T)> m_data;
	};
}


#endif	// RGLZ_MANUAL_LIFETIME_H
