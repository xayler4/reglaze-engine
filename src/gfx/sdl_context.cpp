#include "pch.h"
#include "core/application.h"
#include "core/events.h"
#include "gfx/sdl_context.h"
#include "memory/memory_manager.h"

namespace rglz {
	namespace gfx {
		SDLContext::SDLContext(const Context& context) 
			: Context(context) 
			, window(nullptr)
			, m_moved(false)

		{
			SDL_SetMemoryFunctions(
				[](std::size_t s) -> void* {
					return rglz::memory::MemoryManager::free_list_allocator().allocate(s);
				},
				[](std::size_t n, std::size_t s) -> void* {
					void* out_ptr = rglz::memory::MemoryManager::free_list_allocator().allocate(n*s);
					if (out_ptr != nullptr) {
						memset(out_ptr, 0, n*s);
					}
					return out_ptr;
				},
				[](void* p, std::size_t s) -> void* {
					return rglz::memory::MemoryManager::free_list_allocator().reallocate(p, s);
				},
				[](void* p) {
					rglz::memory::MemoryManager::free_list_allocator().deallocate(p);
				}
			);

			if (!SDL_Init(SDL_INIT_VIDEO)) {
				RGLZ_ENGINE_LOG_ERROR("Failed to initialize SDL: ", std::string_view(SDL_GetError()));
			}

			window = SDL_CreateWindow(name.data(), width, height, 0);
			if (window == nullptr) {
				RGLZ_ENGINE_LOG_ERROR("Failed to create SDL Window: ", std::string_view(SDL_GetError()));
			}
		}

		void SDLContext::on_event_poll(Application* application) {
			SDL_Event event;			
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
					case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
					case SDL_EVENT_WINDOW_DESTROYED:
					case SDL_EVENT_QUIT: {
						application->on_event(Event{EventType::Quit});
					} break;
				}
			}
		}

		SDLContext::SDLContext(SDLContext&& sdl_context) 
			: Context(std::move(sdl_context))
			, window(sdl_context.window)
			, m_moved(sdl_context.m_moved)
		{
			sdl_context.window = nullptr;
			sdl_context.m_moved = true;
		}

		SDLContext& SDLContext::operator= (SDLContext&& sdl_context) {
			Context::operator = (std::move(sdl_context));

			window = sdl_context.window;
			m_moved = sdl_context.m_moved;

			sdl_context.window = nullptr;
			sdl_context.m_moved = true;

			return *this;
		}

		SDLContext::~SDLContext() {
			if (!m_moved) {
				if (window != nullptr) {
					SDL_DestroyWindow(window);
				}

				SDL_Quit();
			}
		}
	}

}
