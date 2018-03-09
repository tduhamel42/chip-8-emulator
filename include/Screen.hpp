//
// File: Screen.hpp
// Author: tanguyduhamel
// Date: Fri Mar  9 16:52:35 2018
//

#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>

# define SCREEN_WIDTH 64
# define SCREEN_HEIGHT 32
# define PIXEL_SIZE 16

class Screen
{

  std::shared_ptr<sf::RenderWindow> m_window;
  std::function<void(uint8_t)> m_on_key_pressed;
  std::function<void(uint8_t)> m_on_key_released;

  void		drawDebug();
  void		keyPressed(sf::Event const &);
  void		keyReleased(sf::Event const &);

  public:

    // The screen in memory
    uint8_t screen[SCREEN_WIDTH * SCREEN_HEIGHT];

    Screen();
    Screen(Screen const &) = delete;
    ~Screen() = default;

    void	clear();
    void	draw();
    bool	update();
    void	onKeyPressed(std::function<void(uint8_t)>);
    void	onKeyReleased(std::function<void(uint8_t)>);
};
