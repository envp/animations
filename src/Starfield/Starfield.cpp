#include "SFML/Graphics.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/VideoMode.hpp"

static constexpr auto WINDOW_WIDTH = 800;
static constexpr auto WINDOW_HEIGHT = 600;

int main() {
  /// Create all the entities that will be in the scene
  sf::Vector2u ScreenSize{WINDOW_WIDTH, WINDOW_HEIGHT};
  sf::RenderWindow Window(sf::VideoMode(ScreenSize), "starfield");
  // Higher values tend to poke holes in the battery
  Window.setFramerateLimit(60);

  while (Window.isOpen()) {
    sf::Event Event;
    while (Window.pollEvent(Event)) {
      if (Event.type == sf::Event::Closed) {
        Window.close();
      }
    }

    Window.clear();

    // end the current frame
    Window.display();
  }
  return 0;
}
