// my
#include "Chip8.hpp"

// std
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstdlib>

/* public RAII */
//--------------------------------------
Chip8::Chip8(std::string rom_name)
:   opcode{0}
,   program_counter{chip8_consts::rom_start}
,   address_register{0}
,   stack_pointer{0}
,   stack{}
,   registers{}
,   ram{}
,   pixels{}
,   delay_timer{0}
,   sound_timer{0}
,   keys{}
,   draw_flag{false}
{
    // load font into RAM
    std::uint8_t chip8_fontset[80] {
         0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
         0x20, 0x60, 0x20, 0x20, 0x70, // 1
         0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
         0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
         0x90, 0x90, 0xF0, 0x10, 0x10, // 4
         0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
         0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
         0xF0, 0x10, 0x20, 0x40, 0x40, // 7
         0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
         0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
         0xF0, 0x90, 0xF0, 0x90, 0x90, // A
         0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
         0xF0, 0x80, 0x80, 0x80, 0xF0, // C
         0xE0, 0x90, 0x90, 0x90, 0xE0, // D
         0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
         0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    for (auto i = 0; i < 80; i++) {
        ram[i] = chip8_fontset[i]; 
    }
    
    /* ROM LOADING SECTINO */

    std::string rom_path{"./roms/" + rom_name}; 
    std::ifstream rom(rom_path, std::ios::binary | std::ios::in);
    
    // if file not found handle that
    while (!rom.is_open()) {
        rom.close();
        std::cout << "ROM: " << rom_path << " failed to open.\n";
        std::cout << "would you like to try again y/n?\n";
        std::string yes_no_maybe_i_dont_know;
        std::cin >> yes_no_maybe_i_dont_know;

        if (yes_no_maybe_i_dont_know == "n") {
            std::exit(EXIT_SUCCESS);
        } else if (yes_no_maybe_i_dont_know == "y") {
            std::cout << "ROM name: ";
            std::cin >> rom_name;
            rom_path = "./roms/" + rom_name;
            rom.open(rom_path, std::ios::binary | std::ios::in); 
        } else {
            std::cout << yes_no_maybe_i_dont_know << " does not equal y or n\n";
        } 
    }
   
   // load ROM into RAM 
   std::size_t i = 0;
   while (rom.good()) {
        ram[0x200 + i++] = rom.get();
   }
}

/* public functions*/
//--------------------------------------
void
Chip8::tick() {
        
    auto mask = [this](std::uint16_t mask) -> std::uint16_t {
        return (opcode & mask);
    };

    auto mask_shift = [this](std::uint16_t mask, std::uint16_t right_shift) -> std::uint16_t {
        return (opcode & mask) >> right_shift;
    };

    auto opcode_error_print = [this](std::string custom_error_string) {
        std::cerr << "Error: " << custom_error_string << "\nopcode: " << std::hex << std::showbase << opcode << '\n'; 
        std::exit(EXIT_FAILURE);
    };
    
    // Fetch op code
    opcode = ram[program_counter] << 8 | ram[program_counter + 1];   // Op code is two bytes
    
    switch(mask(0xF000)){
            // 00E_
        case 0x0000:
            switch (mask(0x000F)) {
                    // 00E0 - Clear screen
                case 0x0000:
                    pixels.fill(0);
                    draw_flag = true;
                    program_counter += 2;
                    break;
                    
                    // 00EE - Return from subroutine
                case 0x000E:
                    program_counter = stack[--stack_pointer];
                    program_counter += 2;
                    break;
                    
                default:
                    opcode_error_print("section 0x0000");
            }
            break;
            
            // 1NNN - Jumps to address NNN
        case 0x1000:
            program_counter = mask(0x0FFF);
            break;
            
            // 2NNN - Calls subroutine at NNN
        case 0x2000:
            stack[stack_pointer++] = program_counter;
            program_counter = mask(0x0FFF);
            break;
            
            // 3XNN - Skips the next instruction if VX equals NN.
        case 0x3000:
            if (registers[mask_shift(0x0F00, 8)] == mask(0x00FF))
                program_counter += 4;
            else
                program_counter += 2;
            break;
            
            // 4XNN - Skips the next instruction if VX does not equal NN.
        case 0x4000:
            if (registers[mask_shift(0x0F00, 8)] != mask(0x00FF))
                program_counter += 4;
            else
                program_counter += 2;
            break;
            
            // 5XY0 - Skips the next instruction if VX equals VY.
        case 0x5000:
            if (registers[mask_shift(0x0F00, 8)] == registers[mask_shift(0x00F0, 4)])
                program_counter += 4;
            else
                program_counter += 2;
            break;
            
            // 6XNN - Sets VX to NN.
        case 0x6000:
            registers[mask_shift(0x0F00, 8)] = mask(0x00FF);
            program_counter += 2;
            break;
            
            // 7XNN - Adds NN to VX.
        case 0x7000:
            registers[mask_shift(0x0F00, 8)] += mask(0x00FF);
            program_counter += 2;
            break;
            
            // 8XY_
        case 0x8000:
            switch (mask(0x000F)) {
                    // 8XY0 - Sets VX to the value of VY.
                case 0x0000:
                    registers[mask_shift(0x0F00, 8)] = registers[mask_shift(0x00F0, 4)];
                    program_counter += 2;
                    break;
                    
                    // 8XY1 - Sets VX to (VX OR VY).
                case 0x0001:
                    registers[mask_shift(0x0F00, 8)] |= registers[mask_shift(0x00F0, 4)];
                    program_counter += 2;
                    break;
                    
                    // 8XY2 - Sets VX to (VX AND VY).
                case 0x0002:
                    registers[mask_shift(0x0F00, 8)] &= registers[mask_shift(0x00F0, 4)];
                    program_counter += 2;
                    break;
                    
                    // 8XY3 - Sets VX to (VX XOR VY).
                case 0x0003:
                    registers[mask_shift(0x0F00, 8)] ^= registers[mask_shift(0x00F0, 4)];
                    program_counter += 2;
                    break;
                    
                    // 8XY4 - Adds VY to VX. VF is set to 1 when there's a carry,
                    // and to 0 when there isn't.
                case 0x0004:
                    registers[mask_shift(0x0F00, 8)] += registers[mask_shift(0x00F0, 4)];

                    if (registers[mask_shift(0x00F0, 4)] > (0xFF - registers[mask_shift(0x0F00, 8)])) {
                        registers[0xF] = 1; //carry
                    } else {
                        registers[0xF] = 0; 
                    }
                    program_counter += 2;
                    break;
                    
                    // 8XY5 - VY is subtracted from VX. VF is set to 0 when
                    // there's a borrow, and 1 when there isn't.
                case 0x0005:
                    if (registers[mask_shift(0x00F0, 4)] > registers[mask_shift(0x0F00, 8)]) {
                        registers[0xF] = 0; // there is a borrow
                    } else {
                        registers[0xF] = 1; 
                    }
                    registers[mask_shift(0x0F00, 8)] -= registers[mask_shift(0x00F0, 4)];
                    program_counter += 2;
                    break;
                    
                    // 0x8XY6 - Shifts VX right by one. VF is set to the value of
                    // the least significant bit of VX before the shift.
                case 0x0006:
                    registers[0xF] = registers[mask_shift(0x0F00, 8)] & 0x1;
                    registers[mask_shift(0x0F00, 8)] >>= 1;
                    program_counter += 2;
                    break;
                    
                    // 0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's
                    // a borrow, and 1 when there isn't.
                case 0x0007:
                    if (registers[mask_shift(0x0F00, 8)] > registers[mask_shift(0x00F0, 4)]) {	
                        registers[0xF] = 0; // borrow
                    } else {
                        registers[0xF] = 1; 
                    }
                    registers[mask_shift(0x0F00 , 8)] = registers[mask_shift(0x00F0, 4)] - registers[mask_shift(0x0F00, 8)];
                    program_counter += 2;
                    break;
                    
                    // 0x8XYE: Shifts VX left by one. VF is set to the value of
                    // the most significant bit of VX before the shift.
                case 0x000E:
                    registers[0xF] = registers[mask_shift(0x0F00, 8)] >> 7;
                    registers[mask_shift(0x0F00, 8)] <<= 1;
                    program_counter += 2;
                    break;
                    
                default:
                    opcode_error_print("section 0x8000");
            }
            break;
            
            // 9XY0 - Skips the next instruction if VX doesn't equal VY.
        case 0x9000:
            if (registers[mask_shift(0x0F00, 8)] != registers[mask_shift(0x00F0, 4)]) {
                program_counter += 4;
            } else {
                program_counter += 2; 
            }
            break;
            
            // ANNN - Sets I to the address NNN.
        case 0xA000:
            address_register = mask(0x0FFF);
            program_counter += 2;
            break;
            
            // BNNN - Jumps to the address NNN plus V0.
        case 0xB000:
            program_counter = mask(0x0FFF) + registers[0];
            break;
            
            // CXNN - Sets VX to a random number, masked by NN.
        case 0xC000:
            registers[mask_shift(0x0F00, 8)] = (rand() % (0xFF + 1)) & mask(0x00FF);
            program_counter += 2;
            break;
            
            // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8
            // pixels and a height of N pixels.
            // Each row of 8 pixels is read as bit-coded starting from memory
            // location I;
            // I value doesn't change after the execution of this instruction.
            // VF is set to 1 if any screen pixels are flipped from set to unset
            // when the sprite is drawn, and to 0 if that doesn't happen.
        case 0xD000:
        {
            std::uint16_t x = registers[mask_shift(0x0F00, 8)];
            std::uint16_t y = registers[mask_shift(0x00F0, 4)];
            std::uint16_t height = mask(0x000F);
            std::uint16_t pixel;
            
            registers[0xF] = 0;

            for (auto yy = 0; yy < height; yy++) {
                pixel = ram[address_register + yy];
                
                for (auto xx = 0; xx < 8; xx++) {
                    if ((pixel & (0x80 >> xx)) != 0) {
                        auto index = x + xx  + ((y + yy) * 64);

                        if (pixels[index] == 1) {
                            registers[0xF] = 1;
                        }
                        pixels[index] ^= 1;
                    }
                }
            }
            
            draw_flag = true;
            program_counter += 2;
        }
            break;
            
            // EX__
        case 0xE000:
            
            switch (mask(0x00FF)) {
                    // EX9E - Skips the next instruction if the key stored
                    // in VX is pressed.
                case 0x009E:
                    if (keys[registers[mask_shift(0x0F00, 8)]] != 0) {
                        program_counter += 4;
                    } else {
                        program_counter += 2; 
                    }
                    break;
                    
                    // EXA1 - Skips the next instruction if the key stored
                    // in VX isn't pressed.
                case 0x00A1:
                    if (keys[registers[mask_shift(0x0F00, 8)]] == 0) {
                        program_counter += 4;
                    } else {
                        program_counter += 2;
                    }
                    break;
                    
                default:
                    opcode_error_print("section 0xE000");
            }
            break;
            
            // FX__
        case 0xF000:
            switch(mask(0x00FF)) {
                // FX07 - Sets VX to the value of the delay timer
            case 0x0007:
                registers[mask_shift(0x0F00, 8)] = delay_timer;
                program_counter += 2;
                break;
                
                // FX0A - A key press is awaited, and then stored in VX
            case 0x000A:
            {
                bool key_pressed = false;
                
                for (auto i = 0; i < 16; ++i) {
                    if(keys[i] != 0) {
                        registers[mask_shift(0x0F00, 8)] = i;
                        key_pressed = true;
                    }
                }
                
                // If no key is pressed, return. the program_counter isnt incremented 
                // so the opcode will be called again until a key is pressed
                if(!key_pressed) return;
                
                program_counter += 2;
            }
                break;
                
                // FX15 - Sets the delay timer to VX
            case 0x0015:
                delay_timer = registers[mask_shift(0x0F00, 8)];
                program_counter += 2;
                break;
                
                // FX18 - Sets the sound timer to VX
            case 0x0018:
                sound_timer = registers[mask_shift(0x0F00, 8)];
                program_counter += 2;
                break;
                
                // FX1E - Adds VX to I
            case 0x001E:
                // VF is set to 1 when range overflow (I+VX>0xFFF), and 0
                // when there isn't.
                if (address_register + registers[mask_shift(0x0F00, 8)] > 0xFFF) {
                    registers[0xF] = 1;
                } else {
                    registers[0xF] = 0;
                }
                address_register += registers[mask_shift(0x0F00, 8)];
                program_counter += 2;
                break;
                
                // FX29 - Sets I to the location of the sprite for the
                // character in VX. Characters 0-F (in hexadecimal) are
                // represented by a 4x5 font
            case 0x0029:
                address_register = registers[mask_shift(0x0F00, 8)] * 0x5;
                program_counter += 2;
                break;
                
                // FX33 - Stores the Binary-coded decimal representation of VX
                // at the addresses I, I plus 1, and I plus 2
            case 0x0033:
                ram[address_register]     =  registers[mask_shift(0x0F00, 8)] / 100;
                ram[address_register + 1] = (registers[mask_shift(0x0F00, 8)] / 10) % 10;
                ram[address_register + 2] = (registers[mask_shift(0x0F00, 8)] % 100) % 10;
                program_counter += 2;
                break;
                
                // FX55 - Stores V0 to VX in memory starting at address I
            case 0x0055:
                for (auto i = 0; i <= mask_shift(0x0F00, 8); ++i) {
                    ram[address_register + i] = registers[i];
                }
                
                // On the original interpreter, when the
                // operation is done, address_register= I + X + 1.
                address_register += mask_shift(0x0F00, 8) + 1;
                program_counter += 2;
                break;
                
            case 0x0065:
                for (auto i = 0; i <= mask_shift(0x0F00, 8); ++i) {
                    registers[i] = ram[address_register + i];
                }
                
                // On the original interpreter,
                // when the operation is done, I = I + X + 1.
                address_register += mask_shift(0x0F00, 8) + 1;
                program_counter += 2;
                break;
                
            default:
                opcode_error_print("section 0xF000");
        }
            break;
            
        default:
            opcode_error_print("optcode not implemented");
    }
}

//--------------------------------------
void
Chip8::renderTo(std::array<std::uint32_t, 64 * 32> &RGBA_buffer) {
    for (auto i = 0; i < chip8_consts::num_pixels; ++i) {
        auto const & pixel = pixels[i];
        RGBA_buffer[i] = (0x00FFFFFF * pixel) | 0xFF000000; 
    } 
}

//--------------------------------------
void
Chip8::updateSystemTimers() {
    if (delay_timer > 0) {
        --delay_timer;
    }
    
    if (sound_timer > 0) {
       if(sound_timer != 0) {
            --sound_timer;
        }
    }
}
