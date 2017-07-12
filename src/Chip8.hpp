#include <cstdint>
#include <array>
#include <string>

class Chip8 final {
public  /*RAII*/:
    Chip8(std::string const &rom_name);
    Chip8(Chip8 const &) = delete;
    Chip8(Chip8 &&) = delete;

public  /*functions*/:
    void reset(std::string const &rom_name);
    void tick();
    std::array<std::uint8_t, 64 * 32>& getPixels(); 

private /*functions*/:
    void load_fontset_into_ram();
    void load_rom_into_ram(std::string const &rom_name);

public /* data */:
    bool draw_flag;
    std::array<std::uint8_t, 16> keys;

private /*data*/:
    // timers
    std::uint8_t delay_timer;
    std::uint8_t sound_timer;
    
    // stack
    std::uint16_t stack_pointer;
    std::array<std::uint16_t, 16> stack;

    // RAM and registers
    std::array<std::uint8_t, 16> registers; // 16 8bit registers V0 -> VF
    std::array<std::uint8_t, 4096> ram;

    // special purpose registers
    std::uint16_t address_register;
    std::uint16_t opcode;
    std::uint16_t program_counter;
    
    // graphics
    std::array<std::uint8_t, 64 * 32> pixels;
};
