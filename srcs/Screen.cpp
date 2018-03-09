//
// File: Screen.cpp
// Author: tanguyduhamel
// Date: Fri Mar  9 16:53:15 2018
//

#include <iostream>
#include <strings.h>
#include "Screen.hpp"

Screen::Screen()
{
  // Creating window
  m_window = std::make_shared<sf::RenderWindow>(sf::VideoMode(SCREEN_WIDTH * PIXEL_SIZE, SCREEN_HEIGHT * PIXEL_SIZE),
      "Chip8");

  // Clearing screen
  clear();
}

void	Screen::clear()
{
  bzero(screen, SCREEN_WIDTH * SCREEN_HEIGHT);
}

void	Screen::drawDebug()
{
  std::cout << "DISPLAY DEBUG:" << std::endl;
  for (int y = 0; y < SCREEN_HEIGHT; ++y)
  {
    for (int x = 0; x < SCREEN_WIDTH; ++x)
    {
      if(screen[(y * 64) + x] == 0) 
	std::cout << "O";
      else 
	std::cout << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

void	Screen::draw()
{
  //drawDebug();
  m_window->clear(sf::Color::Black);
  for (int y = 0; y < SCREEN_HEIGHT; ++y)
  {
    for (int x = 0; x < SCREEN_WIDTH; ++x)
    {
      if(screen[(y * 64) + x] == 0) 
      {
	sf::RectangleShape rectangle(sf::Vector2f(PIXEL_SIZE, PIXEL_SIZE));
	rectangle.setPosition(x * PIXEL_SIZE, y * PIXEL_SIZE);
	m_window->draw(rectangle);
      }
    }
  }
  m_window->display();
}

void	Screen::keyPressed(sf::Event const &event)
{
  switch (event.key.code)
  {
    case sf::Keyboard::Key::Num1: m_on_key_pressed(0x1); break;
    case sf::Keyboard::Key::Num2: m_on_key_pressed(0x2); break;
    case sf::Keyboard::Key::Num3: m_on_key_pressed(0x3); break;
    case sf::Keyboard::Key::Num4: m_on_key_pressed(0xC); break;

    case sf::Keyboard::Key::Q: m_on_key_pressed(0x4); break;
    case sf::Keyboard::Key::W: m_on_key_pressed(0x5); break;
    case sf::Keyboard::Key::E: m_on_key_pressed(0x6); break;
    case sf::Keyboard::Key::R: m_on_key_pressed(0xD); break;

    case sf::Keyboard::Key::A: m_on_key_pressed(0x7); break;
    case sf::Keyboard::Key::S: m_on_key_pressed(0x8); break;
    case sf::Keyboard::Key::D: m_on_key_pressed(0x9); break;
    case sf::Keyboard::Key::F: m_on_key_pressed(0xE); break;

    case sf::Keyboard::Key::Z: m_on_key_pressed(0xA); break;
    case sf::Keyboard::Key::X: m_on_key_pressed(0x0); break;
    case sf::Keyboard::Key::C: m_on_key_pressed(0xB); break;
    case sf::Keyboard::Key::V: m_on_key_pressed(0xF); break;

    default:
      std::cout << "Unhandled key !" << std::endl;
  }
}

void	Screen::keyReleased(sf::Event const &event)
{
  switch (event.key.code)
  {
    case sf::Keyboard::Key::Num1: m_on_key_released(0x1); break;
    case sf::Keyboard::Key::Num2: m_on_key_released(0x2); break;
    case sf::Keyboard::Key::Num3: m_on_key_released(0x3); break;
    case sf::Keyboard::Key::Num4: m_on_key_released(0xC); break;

    case sf::Keyboard::Key::Q: m_on_key_released(0x4); break;
    case sf::Keyboard::Key::W: m_on_key_released(0x5); break;
    case sf::Keyboard::Key::E: m_on_key_released(0x6); break;
    case sf::Keyboard::Key::R: m_on_key_released(0xD); break;

    case sf::Keyboard::Key::A: m_on_key_released(0x7); break;
    case sf::Keyboard::Key::S: m_on_key_released(0x8); break;
    case sf::Keyboard::Key::D: m_on_key_released(0x9); break;
    case sf::Keyboard::Key::F: m_on_key_released(0xE); break;

    case sf::Keyboard::Key::Z: m_on_key_released(0xA); break;
    case sf::Keyboard::Key::X: m_on_key_released(0x0); break;
    case sf::Keyboard::Key::C: m_on_key_released(0xB); break;
    case sf::Keyboard::Key::V: m_on_key_released(0xF); break;
    
    default:
      std::cout << "Unhandled key !" << std::endl;
  }
}

bool	Screen::update()
{
  if (!m_window->isOpen())
    return (false);
  sf::Event event;
  while (m_window->pollEvent(event))
  {
    // "close requested" event: we close the window
    if (event.type == sf::Event::Closed)
      m_window->close();
    else if (event.type == sf::Event::KeyPressed)
      keyPressed(event);
    else if (event.type == sf::Event::KeyReleased)
      keyReleased(event);
  }
  return (true);
}

void	Screen::onKeyPressed(std::function<void(uint8_t)> const &func)
{
  m_on_key_pressed = func;
}

void	Screen::onKeyReleased(std::function<void(uint8_t)> const &func)
{
  m_on_key_released = func;
}
