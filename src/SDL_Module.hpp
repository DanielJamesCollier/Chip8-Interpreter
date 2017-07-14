#ifndef SDL_MODULE_HPP
#define SDL_MODULE_HPP

// deps
#include "SDL2/SDL.h"

// std
#include <array>
#include <cstdint>

class SDL_Module final {
public /* RAII */:
    SDL_Module(int width, int height, int scale);
    SDL_Module(SDL_Module const &) = delete;
    SDL_Module(SDL_Module &&) = delete;
    ~SDL_Module();
   
public /* functions */:
   bool event_loop(std::array<std::uint8_t, 16> & keys);
   void draw(std::array<std::uint32_t, 64 * 32> const & pixels);

private /* data */:
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    SDL_Event m_event;
    SDL_Texture *m_renderTexture;
};
#endif // SDL_MODULE_HPP
