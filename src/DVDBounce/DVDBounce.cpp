#include "SFML/Graphics.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Window/VideoMode.hpp"
#include <filesystem>
#include <iterator>
#include <random>

#include "Assets.h"

constexpr sf::Color ORANGE = sf::Color(/* red   = */ 0xff,
                                       /* green = */ 0x7f,
                                       /* blue  = */ 0x00);

// clang-format off
constexpr sf::Color ALL_COLORS[] = {
    sf::Color::Red,
    ORANGE,
    sf::Color::Yellow,
    sf::Color::Green,
    sf::Color::Blue,
    sf::Color::Cyan,
    sf::Color::Magenta,
    sf::Color::White
};
// clang-format on

constexpr size_t NUM_COLORS = std::size(ALL_COLORS);

int main() {
  std::default_random_engine Generator;
  std::uniform_int_distribution<size_t> RandomIndex(0, NUM_COLORS - 1);

  constexpr static sf::Vector2u SCREEN_SIZE{800, 600};
  sf::RenderWindow Window(sf::VideoMode(SCREEN_SIZE), "DVD");
  Window.setFramerateLimit(60);

  std::filesystem::path AssetRoot(DVDBOUNCE_ASSET_ROOT);

  sf::Texture DVDLogo;
  if (!DVDLogo.loadFromFile(AssetRoot / "dvd-logo-texture.png")) {
    Window.close();
    return -1;
  }

  sf::Sprite DVDSprite;
  DVDSprite.setTexture(DVDLogo);
  DVDSprite.setColor(sf::Color(0, 255, 0));
  const auto DIMENSIONS = DVDLogo.getSize();
  sf::Vector2f Velocity{2, 2};

  while (Window.isOpen()) {
    sf::Event Event;
    while (Window.pollEvent(Event)) {
      if (Event.type == sf::Event::Closed) {
        Window.close();
      } else if (Event.type == sf::Event::Resized) {
        Window.setSize(SCREEN_SIZE);
      }
    }

    Window.clear();
    const auto &TopLeft = DVDSprite.getPosition();
    bool Bounced = false;
    // Invert X-velocity if it touches the vertical bounds anywhere
    if (TopLeft.x < 0 || TopLeft.x + static_cast<float>(DIMENSIONS.x) >=
                             static_cast<float>(SCREEN_SIZE.x)) {
      Velocity.x *= -1;
      Bounced = true;
    }

    // Invert Y-velocity if it touches the horizontal bounds anywhere
    if (TopLeft.y < 0 || TopLeft.y + static_cast<float>(DIMENSIONS.y) >=
                             static_cast<float>(SCREEN_SIZE.y)) {
      Velocity.y *= -1;
      Bounced = true;
    }

    if (Bounced) {
      size_t Index = RandomIndex(Generator);
      assert(Index < NUM_COLORS && "Index out of bounds!");
      DVDSprite.setColor(ALL_COLORS[1]);
    }

    DVDSprite.setPosition(TopLeft + Velocity);

    Window.draw(DVDSprite);

    Window.display();
  }
}
