#ifndef CHIP8_HPP
#define CHIP8_HPP

// std
#include <cstdint>
#include <array>
#include <string>

namespace chip8_consts {
    constexpr auto num_registers {16};
    constexpr auto num_keys      {16};
    constexpr auto gfx_width     {64};
    constexpr auto gfx_height    {32};
    constexpr auto num_pixels    {gfx_width * gfx_height};
    constexpr auto ram_size      {4096};
    constexpr auto stack_size    {16}; 
    constexpr auto rom_max_size  {ram_size - 0x200};
    constexpr auto rom_start     {0x200};
}

class Chip8 final {
public  /*RAII*/:
    Chip8(std::string rom_name);
    Chip8(Chip8 const &) = delete;
    Chip8(Chip8 &&) = delete;

public  /*functions*/:
    void tick();
    void renderTo(std::array<std::uint32_t, chip8_consts::num_pixels> &RGBA_buffer);
    void updateSystemTimers();

private /* data */:
    std::uint16_t opcode;
    std::uint16_t program_counter;
    std::uint16_t address_register;
    std::uint16_t stack_pointer;
    std::array<std::uint16_t, chip8_consts::stack_size> stack;
    std::array<std::uint8_t, chip8_consts::num_registers> registers; // 16 8bit registers V0 -> VF
    std::array<std::uint8_t, chip8_consts::ram_size> ram;
    std::array<std::uint8_t, chip8_consts::num_pixels> pixels;
    std::uint8_t delay_timer;
    std::uint8_t sound_timer;

public /* data */:
    std::array<std::uint8_t, chip8_consts::num_keys> keys;
    bool draw_flag;
    bool beep_flag;
};
#endif // CHIP8_HPP
