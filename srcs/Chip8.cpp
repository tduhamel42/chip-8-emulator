//
// File: Chip8.cpp
// Author: tanguyduhamel
// Date: Fri Mar  9 16:26:19 2018
//

#include <iostream>
#include <fstream>
#include <strings.h>
#include <ctime>
#include <iomanip>
#include "Chip8.hpp"

// The fontset
constexpr uint8_t fontset[80] =
{ 
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

Chip8::Chip8(std::string const &rom_file)
{
  // Registers initialization
  m_pc = 0x200;
  m_i = 0;
  m_sp = 0;

  // Clearing stuffs
  bzero(m_memory, MEMORY_SIZE);
  bzero(m_regs, REGISTER_NUMBER);
  bzero(m_keys, KEY_NUMBER);
  bzero(m_stack, STACK_SIZE * sizeof(uint16_t));

  // Load fontset into memory
  for (int i = 0; i < 80; ++i)
    m_memory[i] = fontset[i];

  // Other things
  m_delay_timer = 0;
  m_sound_timer = 0;
  m_draw = true;
  m_running = true;

  // Opcodes initialization
  initOpcodes();

  // Init event
  m_screen.onKeyPressed([&](int key)
      {
      m_keys[key] = 1;
      });
  m_screen.onKeyReleased([&](int key)
      {
      m_keys[key] = 0;
      });

  loadRom(rom_file);
}

bool	Chip8::loadRom(std::string const &rom_file)
{
  std::ifstream file;

  std::cout << "Reading rom " << rom_file << "..." << std::endl;
  file.open(rom_file, std::ios::in | std::ios::binary | std::ios::ate);
  if (!file.is_open())
    throw std::runtime_error("Could not open rom !");
  std::streampos size = file.tellg();
  file.seekg(0, std::ios::beg);
  char *mem = new char[size];
  file.read(mem, size);
  file.close();
  for (int i = 0; i < size; ++i)
    m_memory[i + 512] = mem[i];
  delete[] mem;
  std::cout << "Rom loaded !" << std::endl;
  return (true);
}

void	Chip8::dumpMemory()
{
  const char *p = reinterpret_cast<const char *>(m_memory);
  for (unsigned int i = 0; i < MEMORY_SIZE; i++) {
    std::cout << "0x" << std::hex << std::setw(8)
      << std::setfill('0') << int(p[i]) << " ";
    if (i % 16 == 0)
      std::cout << std::endl;
  }
  std::cout << std::endl;
}

void	Chip8::initOpcodes()
{
  m_opcodes[0x0000] = [&]() -> bool
  {
    switch (m_current_opcode & 0x000F)
    {
      // Clear screen
      case 0x0000:
	m_screen.clear();
	m_pc += 2;
	break;
	// Returns from subroutine
      case 0x000E:
	--m_sp;
	m_pc = m_stack[m_sp];
	m_pc += 2;
	break;
    }
    return (true);
  };
  m_opcodes[0x1000] = [&]() -> bool
  {
    // Jumps to address
    m_pc = m_current_opcode & 0x0FFF;
    return (true);
  };
  m_opcodes[0x2000] = [&]() -> bool
  {
    // Calls subroutine
    m_stack[m_sp++] = m_pc; 
    m_pc = m_current_opcode & 0x0FFF;
    return (true);
  };
  m_opcodes[0x3000] = [&]() -> bool
  {
    // Skips next instruction if register is equal to val
    if (m_regs[(m_current_opcode & 0x0F00) >> 8] == (m_current_opcode & 0x00FF))
      m_pc += 4;
    else
      m_pc += 2;
    return (true);
  };
  m_opcodes[0x4000] = [&]() -> bool
  {
    // Skips next instruction if register is not equal to val
    if (m_regs[(m_current_opcode & 0x0F00) >> 8] != (m_current_opcode & 0x00FF))
      m_pc += 4;
    else
      m_pc += 2;
    return (true);
  };
  m_opcodes[0x4000] = [&]() -> bool
  {
    // Skips next instruction if register is equal to register
    if (m_regs[(m_current_opcode & 0x0F00) >> 8] == m_regs[(m_current_opcode & 0x00F0) >> 8])
      m_pc += 4;
    else
      m_pc += 2;
    return (true);
  };
  m_opcodes[0x6000] = [&]() -> bool
  {
    // Sets register's value
    m_regs[(m_current_opcode & 0x0F00) >> 8] = (m_current_opcode & 0x00FF);
    m_pc += 2;
    return (true);
  };
  m_opcodes[0x7000] = [&]() -> bool
  {
    // Adds value to register
    m_regs[(m_current_opcode & 0x0F00) >> 8] += (m_current_opcode & 0x00FF);
    m_pc += 2;
    return (true);
  };
  m_opcodes[0x8000] = [&]() -> bool
  {
    switch(m_current_opcode & 0x000F)
    {
      case 0x0000: // 0x8XY0: Sets VX to the value of VY
	m_regs[(m_current_opcode & 0x0F00) >> 8] =
	  m_regs[(m_current_opcode & 0x00F0) >> 4];
	m_pc += 2;
	break;
      case 0x0001: // 0x8XY1: Sets VX to "VX OR VY"
	m_regs[(m_current_opcode & 0x0F00) >> 8] |=
	  m_regs[(m_current_opcode & 0x00F0) >> 4];
	m_pc += 2;
	break;
      case 0x0002: // 0x8XY2: Sets VX to "VX AND VY"
	m_regs[(m_current_opcode & 0x0F00) >> 8] &=
	  m_regs[(m_current_opcode & 0x00F0) >> 4];
	m_pc += 2;
	break;
      case 0x0003: // 0x8XY3: Sets VX to "VX XOR VY"
	m_regs[(m_current_opcode & 0x0F00) >> 8] ^=
	  m_regs[(m_current_opcode & 0x00F0) >> 4];
	m_pc += 2;
	break;
      case 0x0004: // 0x8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't					
	if(m_regs[(m_current_opcode & 0x00F0) >> 4] >
	    (0xFF - m_regs[(m_current_opcode & 0x0F00) >> 8])) 
	  m_regs[0xF] = 1;
	else 
	  m_regs[0xF] = 0;					
	m_regs[(m_current_opcode & 0x0F00) >> 8] +=
	  m_regs[(m_current_opcode & 0x00F0) >> 4];
	m_pc += 2;					
	break;
      case 0x0005: // 0x8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
	if(m_regs[(m_current_opcode & 0x00F0) >> 4] >
	    m_regs[(m_current_opcode & 0x0F00) >> 8]) 
	  m_regs[0xF] = 0; // there is a borrow
	else 
	  m_regs[0xF] = 1;					
	m_regs[(m_current_opcode & 0x0F00) >> 8] -=
	  m_regs[(m_current_opcode & 0x00F0) >> 4];
	m_pc += 2;
	break;
      case 0x0006: // 0x8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
	m_regs[0xF] = m_regs[(m_current_opcode & 0x0F00) >> 8] & 0x1;
	m_regs[(m_current_opcode & 0x0F00) >> 8] >>= 1;
	m_pc += 2;
	break;
      case 0x0007: // 0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
	if(m_regs[(m_current_opcode & 0x0F00) >> 8] >
	    m_regs[(m_current_opcode & 0x00F0) >> 4])	// VY-VX
	  m_regs[0xF] = 0; // there is a borrow
	else
	  m_regs[0xF] = 1;
	m_regs[(m_current_opcode & 0x0F00) >> 8] =
	  m_regs[(m_current_opcode & 0x00F0) >> 4] -
	  m_regs[(m_current_opcode & 0x0F00) >> 8];
	m_pc += 2;
	break;
      case 0x000E: // 0x8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
	m_regs[0xF] = m_regs[(m_current_opcode & 0x0F00) >> 8] >> 7;
	m_regs[(m_current_opcode & 0x0F00) >> 8] <<= 1;
	m_pc += 2;
	break;
    }
    return (true);
  };
  m_opcodes[0x9000] = [&]() -> bool
  {
    if (m_regs[(m_current_opcode & 0x0F00) >> 8] !=
	m_regs[(m_current_opcode & 0x00F0) >> 4])
      m_pc += 4;
    else
      m_pc += 2;
    return (true);
  };
  m_opcodes[0xA000] = [&]() -> bool
  {
    m_i = m_current_opcode & 0x0FFF;
    m_pc += 2;
    return (true);
  };
  m_opcodes[0xB000] = [&]() -> bool
  {
    m_pc = (m_current_opcode & 0x0FFF) + m_regs[0];
    return (true);
  };
  m_opcodes[0xC000] = [&]() -> bool
  {
    std::srand(std::time(0));
    m_regs[(m_current_opcode & 0x0F00) >> 8] = (std::rand() % 0xFF) &
      (m_current_opcode & 0x00FF);
    m_pc += 2;
    return (true);
  };
  m_opcodes[0XD000] = [&]() -> bool
  {
    uint16_t x = m_regs[(m_current_opcode & 0x0F00) >> 8];
    uint16_t y = m_regs[(m_current_opcode & 0x00F0) >> 4];
    uint16_t height = (m_current_opcode & 0x000F);

    unsigned short pixel;

    m_regs[0xF] = 0;
    for (int yline = 0; yline < height; yline++)
    {
      pixel = m_memory[m_i + yline];
      for(int xline = 0; xline < 8; xline++)
      {
	if((pixel & (0x80 >> xline)) != 0)
	{
	  if (m_screen.screen[(x + xline + ((y + yline) * 64))] == 1)
	  {
	    m_regs[0xF] = 1;                                    
	  }
	  m_screen.screen[x + xline + ((y + yline) * 64)] ^= 1;
	}
      }
    }
    m_draw = true;
    m_pc += 2;
    return (true);
  };
  m_opcodes[0xE000] = [&]() -> bool
  {
    switch(m_current_opcode & 0x00FF)
    {
      case 0x009E: 
	if(m_keys[m_regs[(m_current_opcode & 0x0F00) >> 8]] != 0)
	  m_pc += 4;
	else
	  m_pc += 2;
	break;
      case 0x00A1:
	if(m_keys[m_regs[(m_current_opcode & 0x0F00) >> 8]] == 0)
	  m_pc += 4;
	else
	  m_pc += 2;
	break;
    }
    return (true);
  };
  m_opcodes[0xF000] = [&]() -> bool
  {
    switch(m_current_opcode & 0x00FF)
    {
      case 0x0007:
	m_regs[(m_current_opcode & 0x0F00) >> 8] = m_delay_timer;
	m_pc += 2;
	break;
      case 0x000A: 
	{
	  bool keyPress = false;

	  for(int i = 0; i < 16; ++i)
	  {
	    if(m_keys[i] != 0)
	    {
	      m_regs[(m_current_opcode & 0x0F00) >> 8] = i;
	      keyPress = true;
	    }
	  }
	  if(!keyPress)						
	    return (false);
	  m_pc += 2;					
	}
	break;
      case 0x0015:
	m_delay_timer = m_regs[(m_current_opcode & 0x0F00) >> 8];
	m_pc += 2;
	break;
      case 0x0018:
	m_sound_timer = m_regs[(m_current_opcode & 0x0F00) >> 8];
	m_pc += 2;
	break;
      case 0x001E:
	if(m_i + m_regs[(m_current_opcode & 0x0F00) >> 8] > 0xFFF)
	  m_regs[0xF] = 1;
	else
	  m_regs[0xF] = 0;
	m_i += m_regs[(m_current_opcode & 0x0F00) >> 8];
	m_pc += 2;
	break;
      case 0x0029:
	m_i = m_regs[(m_current_opcode & 0x0F00) >> 8] * 0x5;
	m_pc += 2;
	break;
      case 0x0033:
	m_memory[m_i] = m_regs[(m_current_opcode & 0x0F00) >> 8] / 100;
	m_memory[m_i + 1] = (m_regs[(m_current_opcode & 0x0F00) >> 8] /
	    10) % 10;
	m_memory[m_i + 2] = (m_regs[(m_current_opcode & 0x0F00) >> 8] %
	    100) % 10;					
	m_pc += 2;
	break;
      case 0x0055:
	for (int i = 0; i <= ((m_current_opcode & 0x0F00) >> 8); ++i)
	  m_memory[m_i + i] = m_regs[i];	

	m_i += ((m_current_opcode & 0x0F00) >> 8) + 1;
	m_pc += 2;
	break;
      case 0x0065:
	for (int i = 0; i <= ((m_current_opcode & 0x0F00) >> 8); ++i)
	  m_regs[i] = m_memory[m_i + i];			

	m_i += ((m_current_opcode & 0x0F00) >> 8) + 1;
	m_pc += 2;
	break;
    }
    return (true);
  };
}

void	Chip8::runCycle()
{
  // Gets opcode from memory(rom part)
  m_current_opcode = m_memory[m_pc] << 8 | m_memory[m_pc + 1];
  auto it = m_opcodes.find(m_current_opcode & 0xF000);
  if (it == m_opcodes.end())
    throw std::runtime_error("Unknown opcode: " +
	std::to_string(m_current_opcode));
  if (!m_opcodes[m_current_opcode & 0xF000]())
    return ;
  if (m_delay_timer > 0)
    --m_delay_timer;
  if (m_sound_timer > 0)
  {
    if (m_sound_timer == 1)
      std::cout << "BEEP" << std::endl;
    --m_sound_timer;
  }
}

int	Chip8::run()
{
  /*std::cout << "Dumping memory" << std::endl;
  dumpMemory();*/
  std::cout << "Running game..." << std::endl;
  while (m_running)
  {
    if (!m_screen.update())
      m_running = false;
    runCycle();
    if (m_draw)
      m_screen.draw();
    m_draw = false;
  }
  return (0);
}
