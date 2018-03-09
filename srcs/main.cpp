//
// File: main.cpp
// Author: tanguyduhamel
// Date: Fri Mar  9 16:28:37 2018
//

#include <iostream>
#include "Chip8.hpp"

int	main(int argc, char **argv)
{
  if (argc != 2)
    throw std::runtime_error("Wron numbers of args !");
  Chip8 chip(argv[1]);
  return (chip.run());
}

