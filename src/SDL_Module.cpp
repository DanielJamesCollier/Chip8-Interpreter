#include "SDL_Module.hpp"
#include <cstdlib>
#include <iostream>

std::uint8_t keymap[16] = {
        SDLK_x,
        SDLK_1,
        SDLK_2,
        SDLK_3,
        SDLK_q,
        SDLK_w,
        SDLK_e,
        SDLK_a,
        SDLK_s,
        SDLK_d,
        SDLK_z,
        SDLK_c,
        SDLK_4,
        SDLK_r,
        SDLK_f,
        SDLK_v,
};

/*public RAII*/

SDL_Module::SDL_Module(int width, int height, int scale) {
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        std::exit(EXIT_FAILURE); 
    }
    
    auto flags = SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI; 
    auto position = SDL_WINDOWPOS_CENTERED;

    m_window = SDL_CreateWindow("Chip8 Interpretor", position, position, width * scale, height * scale, flags);
    
    if (m_window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow failed - m_renderTexture: %s", SDL_GetError());
        std::exit(EXIT_FAILURE); 
    }
    
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);

    if (m_renderer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer failed - m_renderTexture: %s", SDL_GetError());
        std::exit(EXIT_FAILURE);
    };

    m_renderTexture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    
    if (m_renderTexture == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer failed - m_renderTexture: %s", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }

    // set the screen the black
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(m_renderer);
    SDL_RenderPresent(m_renderer);
}

SDL_Module::~SDL_Module() {
    SDL_DestroyTexture(m_renderTexture); 
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

bool
SDL_Module::event_loop(std::array<std::uint8_t, 16> & keys) {
    while (SDL_PollEvent(&m_event)) {
        if (m_event.type == SDL_QUIT) {
            return false;
        }   

        if (m_event.type == SDL_KEYDOWN) {
            for (int i = 0; i < 16; i++) {
                if (m_event.key.keysym.sym == keymap[i]) {
                    keys[i] = 1;
                }
            }
        }
        
        if (m_event.type == SDL_KEYUP) {
            for (int i = 0; i < 16; i++) {
                if (m_event.key.keysym.sym == keymap[i]) {
                    keys[i] = 0;
                }
            }
        }
    }

    return true;
}

void
SDL_Module::draw(std::array<std::uint8_t, 64 * 32> const & pixels) {
    
   std::array<std::uint8_t, (64 * 32) * 4> local_pixels; 
    
   std::size_t local_index = 0;

   for (auto i = 0; i < 64 * 32; i++) {
        
        // todo: remove this branch  
        std::uint8_t pix{}; 
        if (pixels[i] == 1) pix = 255;  
       
        local_pixels[local_index + 0] = pix;
        local_pixels[local_index + 1] = pix;
        local_pixels[local_index + 2] = pix;      
        local_pixels[local_index + 3] = SDL_ALPHA_OPAQUE;
        local_index += 4;
   }
    
   SDL_UpdateTexture(m_renderTexture, NULL, local_pixels.data(), 64 * 4); 
   SDL_RenderClear(m_renderer); 
   SDL_RenderCopy(m_renderer, m_renderTexture, NULL, NULL); 
   SDL_RenderPresent(m_renderer);
}
