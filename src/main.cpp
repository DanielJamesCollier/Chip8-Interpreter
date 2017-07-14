// my
#include "Chip8.hpp"
#include "SDL_Module.hpp"

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
    std::string rom("INVADERS");

    // allow the user to specify a ROM as a command line argument    
    if (argc == 2) {
        rom = argv[1]; 
    } else {
        std::cout << "\nTo select a ROM pass it as a command line argument as follows\nChip8-Interpreter ROM_NAME\nDefault ROM: Space Invaders\n";
    } 

    Chip8 chip8(rom);
    SDL_Module sdl(64, 32, 10);
    std::array<std::uint32_t, 64 * 32> RGBA_buffer; 
    std::cout << sizeof(Chip8) << std::endl;

    while(sdl.event_loop(chip8.keys)) {
        chip8.tick();    
        chip8.updateSystemTimers();

        if (chip8.draw_flag) {
            chip8.renderTo(RGBA_buffer);
            sdl.draw(RGBA_buffer);
            chip8.draw_flag = false;
        }
        std::this_thread::sleep_for(400us); // ugly hack: replace
    }
    return 0;
}
