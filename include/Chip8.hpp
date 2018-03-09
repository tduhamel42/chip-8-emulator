//
// File: Chip8.hpp
// Author: tanguyduhamel
// Date: Fri Mar  9 16:25:14 2018
//

#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "Screen.hpp"

# define MEMORY_SIZE 4096
# define REGISTER_NUMBER 16
# define KEY_NUMBER 16
# define STACK_SIZE 16

class Chip8
{

  // RAM
  uint8_t m_memory[MEMORY_SIZE]; // Chip ram

  // Registers
  uint8_t m_regs[REGISTER_NUMBER]; // Chip general purpose registers
  uint16_t m_i; // Address register
  uint16_t m_pc; // Program counter
  uint8_t m_delay_timer; // Timer for events
  uint8_t m_sound_timer; // Timer for sound effects

  // Stack
  uint16_t m_stack[STACK_SIZE];
  uint16_t m_sp; // Stack pointer

  // Input
  uint8_t m_keys[KEY_NUMBER];

  bool m_running;
  bool m_draw;

  // Opcodes
  uint16_t m_current_opcode;
  std::unordered_map<uint16_t, std::function<bool()>> m_opcodes;

  // The emulated screen
  Screen m_screen;

  void		initOpcodes();
  void		runCycle();
  void		dumpMemory();

  public:
    Chip8(std::string const &);
    Chip8(Chip8 const &) = delete;
    ~Chip8() = default;

    // Loads a rom into memory
    bool	loadRom(std::string const &);

    // Game loop
    int		run();
};
