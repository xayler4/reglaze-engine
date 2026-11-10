#include "systems/sprite_extractor_system.h"
#include "gfx/renderer.h"

namespace rglz {
	void SpriteExtractorSystem::on_process(Scene& scene) {

		// std::span<SpriteComponent> sprite_component_view = get_component_view<SpriteComponent>(scene);
		// std::span<TransformComponent> transform_component_view = get_component_view<TransformComponent>(scene);
		std::span<ecs::EntityId> sprite_entity_list = get_entity_list<const SpriteComponent>(scene);

		parallel_for(scene, sprite_entity_list.size(), 128, [&sprite_entity_list](ReadOnlyScene scene, uint32_t start_idx, uint32_t end_idx) {
			FixedVector<gfx::DrawSpriteCommand, 128> draw_sprite_command_list;
			for (uint32_t i = start_idx; i < end_idx; i++) {
				ecs::EntityId entity = sprite_entity_list[i];

				if (!scene.has_components<const TransformComponent>(entity)) {
					continue;
				}
				const TransformComponent& transform_component = scene.get_component<const TransformComponent>(entity);
				const SpriteComponent& sprite_component = scene.get_component<const SpriteComponent>(entity);

				draw_sprite_command_list.push_back({transform_component.transform_mat, sprite_component.image});

			}

			Engine::app_renderer().push_draw_sprite_command_list(draw_sprite_command_list);
		});

		// for (ecs::EntityId entity : sprite_entity_list) {
		// 	if (!has_components<const TransformComponent>(scene, entity)) {
		// 		continue;
		// 	}
		// 	const TransformComponent& transform_component = get_component<const TransformComponent>(scene, entity);
		// 	const SpriteComponent& sprite_component = get_component<const SpriteComponent>(scene, entity);

		// 	Engine::app_renderer().push_draw_sprite_command(transform_component.transform_mat, sprite_component.image);
		// }
	}
}
