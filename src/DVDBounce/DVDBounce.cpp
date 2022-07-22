#include "SFML/Graphics.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Window/VideoMode.hpp"
#include <filesystem>

#include "Assets.h"

int main() {
  const sf::Vector2u SCREEN_SIZE{800, 600};
  sf::RenderWindow Window(sf::VideoMode(SCREEN_SIZE), "DVD");
  Window.setFramerateLimit(60);

  std::filesystem::path AssetRoot(DVDBOUNCE_ASSET_ROOT);

  sf::Texture DVDLogo;
  if (!DVDLogo.loadFromFile(AssetRoot / "dvd-logo.png")) {
    Window.close();
    return -1;
  }

  sf::Sprite DVDSprite;
  DVDSprite.setTexture(DVDLogo);
  DVDSprite.setColor(sf::Color(0, 255, 0));
  const auto DIMENSIONS = DVDLogo.getSize();
  sf::Vector2f Velocity{1, 1};

  while (Window.isOpen()) {
    sf::Event Event;
    while (Window.pollEvent(Event)) {
      if (Event.type == sf::Event::Closed) {
        Window.close();
      }
    }

    Window.clear(sf::Color::White);
    const auto &TopLeft = DVDSprite.getPosition();

    if (TopLeft.x < 0 || TopLeft.x + static_cast<float>(DIMENSIONS.x) >=
                             static_cast<float>(SCREEN_SIZE.x)) {
      Velocity.x *= -1;
    }

    if (TopLeft.y < 0 || TopLeft.y + static_cast<float>(DIMENSIONS.y) >=
                             static_cast<float>(SCREEN_SIZE.y)) {
      Velocity.y *= -1;
    }

    DVDSprite.setPosition(TopLeft + Velocity);

    Window.draw(DVDSprite);

    Window.display();
  }
}
