#ifndef RGLZ_ACTION_MANAGER_H
#define RGLZ_ACTION_MANAGER_H

#include "base.h"
#include "containers/fixed_hash_map.h"

namespace rglz {
	class ActionManager;

	enum class ActionType {
		Button,
		Value
	};

	struct Action {
		inline Action(ActionType type) {
			this->type = type;
			value = 0.0f;
			m_bitmask = 0;
		}

		enum BitmaskFlag {
			PressedBitmask = (uint8_t)1,
			ReleasedBitmask = (uint8_t)1 << 1,
			DownBitmask = (uint8_t)1 << 2
		};

		inline void press() {
			if (!is_down()) {
				m_bitmask |= PressedBitmask;
			}

			m_bitmask |= DownBitmask;
		}

		inline void release() {
			m_bitmask &= ~(DownBitmask);
			m_bitmask |= ReleasedBitmask;
		}

		inline void clear_press_and_release() {
			m_bitmask &= ~(PressedBitmask | ReleasedBitmask);
		}

		inline bool is_pressed() const {
			return m_bitmask & PressedBitmask;
		}

		inline bool is_released() const {
			return m_bitmask & ReleasedBitmask;
		}

		inline bool is_down() const {
			return m_bitmask & DownBitmask;
		}

		ActionType type;	
		float value;

	private:
		uint8_t m_bitmask;
	};

	class ActionId {
	public:
		ActionId() 
			: m_id(0) 
		{}

		inline operator uint32_t() const {
			return m_id;
		}

	private:
		ActionId(uint32_t id) 
			: m_id(id)
		{}

	private:
		uint32_t m_id;

		friend class ActionManager;
	};

	class ActionManager {
	public:
		ActionId create(ActionType type);

		void press(ActionId id);
		void release(ActionId id);
		void set_value(ActionId id, float value);

		float value(ActionId id) const;
		ActionType type(ActionId id) const;

		bool is_down(ActionId id) const;
		bool is_pressed(ActionId id) const;
		bool is_released(ActionId id) const;

	private:
		void update();
		ActionId gen_id();

	private:
		FixedHashMap<ActionId, Action, RGLZ_ACTION_MANAGER_MAX_ACTIONS> m_action_map;

		friend class Engine;
	};
}

#endif	// RGLZ_ACTION_MANAGER_H
