#include "pch.h"
#include "core/action_manager.h"

namespace rglz {
	
	ActionId ActionManager::create(ActionType type) {
		ActionId id = gen_id();
		Action action(type);

		m_action_map.insert(id, action);

		return id;
	}

	void ActionManager::press(ActionId id) {
		m_action_map[id]->press();
	}

	void ActionManager::release(ActionId id) {
		m_action_map[id]->release();
	}

	void ActionManager::set_value(ActionId id, float value) {
		m_action_map[id]->value = value;
	}

	float ActionManager::value(ActionId id) const {
		return m_action_map[id]->value;
	}

	ActionType ActionManager::type(ActionId id) const {
		return m_action_map[id]->type;
	}

	bool ActionManager::is_down(ActionId id) const {
		return m_action_map[id]->is_down();
	}

	bool ActionManager::is_pressed(ActionId id) const {
		return m_action_map[id]->is_pressed();
	}

	bool ActionManager::is_released(ActionId id) const {
		return m_action_map[id]->is_released();
	}

	void ActionManager::update() {
		for (auto& pair : m_action_map) {
			pair.value.clear_press_and_release();
		}
	}

	ActionId ActionManager::gen_id() {
		static uint32_t id = 0;
		return ++id;
	}
}
