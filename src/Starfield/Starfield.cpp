#include "SFML/Graphics.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/VideoMode.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <functional>
#include <iostream>
#include <memory>
#include <random>

static constexpr auto WINDOW_WIDTH = 800;
static constexpr auto WINDOW_HEIGHT = 600;

static constexpr float HALF_WIDTH = 0.5 * static_cast<float>(WINDOW_WIDTH);
static constexpr float HALF_HEIGHT = 0.5 * static_cast<float>(WINDOW_HEIGHT);

constexpr size_t NUM_STARS = 512;
constexpr unsigned FRAME_RATE = 60;

class WorldState {
private:
  using FloatDist = std::uniform_real_distribution<float>;
  using UIntDist = std::uniform_int_distribution<size_t>;
  WorldState() = delete;

public:
  WorldState(std::pair<float, float> XBounds, std::pair<float, float> YBounds)
      : XDist(XBounds.first, XBounds.second),
        YDist(YBounds.first, YBounds.second) {}

  sf::Vector2f getRandom2D() {
    float X = XDist(Engine);
    float Y = YDist(Engine);
    return {X, Y};
  }

  float getRandomDepthRatio() {
    static FloatDist Dist(1, 3);
    return Dist(Engine);
  }

  sf::Color getRandomColor() {
    // Some values are repeated to
    constexpr sf::Color ALLOWED_STAR_COLORS[] = {
        // Reddish
        sf::Color(0xCF, 0x7F, 0x40),
        // Yellow
        sf::Color(0xF8, 0xEE, 0xA5),
        // Powder Blue
        sf::Color(0xB0, 0xE0, 0xE6),
        // Orange-ish
        sf::Color(0xFF, 0x95, 0x00),
        sf::Color::White,
    };
    static UIntDist ColorDist{0, std::size(ALLOWED_STAR_COLORS)};
    return ALLOWED_STAR_COLORS[ColorDist(Engine)];
  }

  void reset(std::pair<float, float> XBounds, std::pair<float, float> YBounds) {
    XDist = FloatDist(XBounds.first, XBounds.second);
    YDist = FloatDist(YBounds.first, YBounds.second);
  }

private:
  std::ranlux24 Engine;
  FloatDist XDist;
  FloatDist YDist;
};

static sf::CircleShape getDefaultStarShape() {
  sf::CircleShape DefaultStar(/* radius = */ 1.0f);
  DefaultStar.setFillColor(sf::Color::White);
  DefaultStar.setOutlineThickness(0.5f);
  return DefaultStar;
}

int main() {
  /// Create all the entities that will be in the scene
  sf::RenderWindow Window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}),
                          "starfield");
  Window.setFramerateLimit(FRAME_RATE);

  WorldState State({-1.0 * HALF_WIDTH * 3, 1.0 * HALF_WIDTH * 3},
                   {-1.0 * HALF_HEIGHT * 3, 1.0 * HALF_HEIGHT * 3});

  sf::Color OutlineColors[NUM_STARS];
  for (auto &Color : OutlineColors) {
    Color = State.getRandomColor();
  }

  float DepthRatio[NUM_STARS];
  for (auto &R : DepthRatio) {
    R = State.getRandomDepthRatio();
  }

  sf::Vector2f WorldCoordinates[NUM_STARS];
  sf::Vector2f ScreenPositions[NUM_STARS];
  for (auto &SP : WorldCoordinates) {
    SP = State.getRandom2D();
  }

  float Rate = 0.5;
  bool Paused = false;

  while (Window.isOpen()) {
    sf::Event Event;
    while (Window.pollEvent(Event)) {
      if (Event.type == sf::Event::Closed) {
        Window.close();
      } else if (Event.type == sf::Event::MouseButtonPressed) {
        Paused = true;
      } else if (Event.type == sf::Event::MouseButtonReleased) {
        Paused = false;
      } else if (Event.type == sf::Event::MouseWheelScrolled) {
        float Scroll = Event.mouseWheelScroll.delta;
        Rate -= Scroll / 10.0f;
        Rate = std::max(0.0f, Rate);
        Rate = std::min(5.0f, Rate);
      }
    }

    if (Paused) {
      continue;
    }

    // BEGIN FRAME
    Window.clear();
    auto View = Window.getView();
    auto Viewport = Window.getViewport(View);

    for (size_t Idx = 0; Idx != NUM_STARS; ++Idx) {
      sf::Vector2f WorldCoordinate = WorldCoordinates[Idx];
      DepthRatio[Idx] -= Rate / FRAME_RATE;
      ScreenPositions[Idx] = WorldCoordinate / DepthRatio[Idx] +
                             sf::Vector2f(HALF_WIDTH, HALF_HEIGHT);
      if (!Viewport.contains(sf::Vector2i{ScreenPositions[Idx]})) {
        DepthRatio[Idx] = State.getRandomDepthRatio();
        WorldCoordinates[Idx] = State.getRandom2D();
        OutlineColors[Idx] = State.getRandomColor();
      }
    }
    auto StarShape = getDefaultStarShape();
    for (size_t Idx = 0; Idx != NUM_STARS; ++Idx) {
      float Scale = DepthRatio[Idx];
      if (Scale < 0.5) {
        Scale = 0.5;
      }
      StarShape.setPosition(ScreenPositions[Idx]);
      StarShape.setOutlineColor(OutlineColors[Idx]);
      StarShape.setScale({1.0f / Scale, 1.0f / Scale});
      Window.draw(StarShape);
    }

    // END FRAME
    Window.display();
  }
  return 0;
}
