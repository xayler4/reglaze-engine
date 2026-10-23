#include "systems/sprite_extractor_system.h"
#include "gfx/renderer.h"

namespace rglz {
	void SpriteExtractorSystem::on_process(Scene& scene) {

		// std::span<SpriteComponent> sprite_component_view = get_component_view<SpriteComponent>(scene);
		// std::span<TransformComponent> transform_component_view = get_component_view<TransformComponent>(scene);
		std::span<ecs::EntityId> sprite_entity_list = get_entity_list<const SpriteComponent>(scene);

		for (ecs::EntityId entity : sprite_entity_list) {
			if (!has_components<const TransformComponent>(scene, entity)) {
				continue;
			}
			const TransformComponent& transform_component = get_component<const TransformComponent>(scene, entity);
			const SpriteComponent& sprite_component = get_component<const SpriteComponent>(scene, entity);

			Engine::app_renderer().push_draw_sprite_command(transform_component.transform_mat, sprite_component.image);
		}
	}
}
