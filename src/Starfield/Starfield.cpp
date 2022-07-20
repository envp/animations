#include "SFML/Graphics.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/VideoMode.hpp"
#include <cstdint>
#include <functional>
#include <random>
#include <vector>

static constexpr auto WINDOW_WIDTH = 1200;
static constexpr auto WINDOW_HEIGHT = 600;

static void handleWindowEvents(sf::Window &Window) {
  sf::Event Event;
  while (Window.pollEvent(Event)) {
    if (Event.type == sf::Event::Closed) {
      Window.close();
    }
  }
}

int main() {
  /// Create all the entities that will be in the scene
  sf::Vector2u ScreenSize{WINDOW_WIDTH, WINDOW_HEIGHT};
  sf::RenderWindow Window(sf::VideoMode(ScreenSize), "starfield");
  // Higher values tend to poke holes in the battery
  Window.setFramerateLimit(60);

  while (Window.isOpen()) {
    handleWindowEvents(Window);

    Window.clear(sf::Color::Black);

    // end the current frame
    Window.display();
  }
  return 0;
}
