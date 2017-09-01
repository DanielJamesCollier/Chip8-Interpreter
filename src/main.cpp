// my
#include "chip8.hpp"
#include "sdl_module.hpp"

// std
#include <chrono>
#include <thread>
#include <iostream>
#include <array>
#include <cstdint>

using namespace std::chrono_literals;

//--------------------------------------
int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false); 
    
    auto scale {10};
    auto width {64};
    auto height {32};

    std::string rom {"INVADERS"}; 
    
    Chip8 chip8(rom);
    SDL_Module sdl(width, height, scale);
    std::array<std::uint32_t, 64 * 32> RGBA_buffer; 
    
    auto start_system_timers = std::chrono::system_clock::now();
    auto start_second_counter = start_system_timers;
    auto start_clock_speed = start_system_timers;
    
    unsigned frame = 0;

    while(sdl.event_loop(chip8.keys)) {
        
        auto now = std::chrono::system_clock::now();
        auto duration_system_timers = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_system_timers);
        auto duration_second_counter = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_second_counter);
        auto duration_clock_speed = std::chrono::duration_cast<std::chrono::microseconds>(now - start_clock_speed);
        
        // code happens once per 16ms inside here - updates chip8 system timers
        if (duration_system_timers >= 16ms) {
            start_system_timers = std::chrono::system_clock::now();
            chip8.updateSystemTimers();
        } 
        
        // code happens once per second inside here
        if (duration_second_counter >= 1s) {
            start_second_counter = std::chrono::system_clock::now();
            std::cout << "FPS: " << frame << std::endl; 
            frame = 0;
        }
        
        // clock speed limiter [1m == 1000hz] [2m == 500hz]
        if (duration_clock_speed >= 1ms) {
            chip8.tick();
            start_clock_speed = std::chrono::system_clock::now();
        }
        
        // gfx
        if (chip8.draw_flag) {
            chip8.renderTo(RGBA_buffer);
            sdl.draw(RGBA_buffer);
            chip8.draw_flag = false;
        }

        // audio 
        if (chip8.beep_flag) {
            sdl.play_beep();
            chip8.beep_flag = false;
        }
       

        frame++;
    }
    return 0;
}
