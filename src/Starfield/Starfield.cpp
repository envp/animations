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
#include <memory>
#include <random>
#include <utility>

static constexpr auto WINDOW_WIDTH = 800;
static constexpr auto WINDOW_HEIGHT = 600;

constexpr size_t NUM_STARS = 1024;

// Position and Velocity for a single star
struct PhaseCoordinate {
  sf::Vector2f Position;
  sf::Vector2f Velocity;
};

class WorldState {
private:
  using FloatDist = std::normal_distribution<float>;
  using UIntDist = std::uniform_int_distribution<size_t>;
  WorldState() = delete;

public:
  WorldState(std::pair<float, float> XBounds, std::pair<float, float> YBounds)
      : XDist(new FloatDist((XBounds.second + XBounds.first) / 2,
                            (XBounds.second - XBounds.first) / 4)),
        YDist(new FloatDist((YBounds.second + YBounds.first) / 2,
                            (YBounds.second - YBounds.first) / 4)) {}

  std::pair<float, float> getRandomXY() {
    float X = (*XDist)(Engine);
    float Y = (*YDist)(Engine);
    return {X, Y};
  }

  sf::Color getRandomColor() {
    // Some values are repeated to
    constexpr sf::Color ALLOWED_STAR_COLORS[] = {
        // Reddish
        sf::Color(0xCF, 0x51, 0x40),
        // Powder Blue
        sf::Color(0xB0, 0xE0, 0xE6),
        // Orange-ish
        sf::Color(0xFF, 0x95, 0x00),
        sf::Color::White,
    };
    static UIntDist ColorDist{0, std::size(ALLOWED_STAR_COLORS)};
    return ALLOWED_STAR_COLORS[ColorDist(Engine)];
  }

  void reset() {
    XDist->reset();
    YDist->reset();
  }

private:
  std::default_random_engine Engine;
  std::unique_ptr<FloatDist> XDist;
  std::unique_ptr<FloatDist> YDist;
};

int main() {
  /// Create all the entities that will be in the scene
  sf::Vector2u ScreenSize{WINDOW_WIDTH, WINDOW_HEIGHT};
  sf::RenderWindow Window(sf::VideoMode(ScreenSize), "starfield");
  Window.setFramerateLimit(60);

  // Tiny factor which we scale with by repeaated multiplication.
  // Because it is so small, the scaling *looks* approximately linear over the
  constexpr float SCALE_FACTOR = 1.006;
  WorldState State({0, static_cast<float>(WINDOW_WIDTH) - 1},
                   {0, static_cast<float>(WINDOW_HEIGHT) - 1});

  sf::CircleShape StarShapes[NUM_STARS];
  std::fill_n(std::begin(StarShapes), NUM_STARS,
              sf::CircleShape(/* radius = */ 1.0f));
  float StarScale[NUM_STARS];
  std::fill_n(std::begin(StarScale), NUM_STARS, 1.0f);

  for (auto &Star : StarShapes) {
    Star.setFillColor(State.getRandomColor());
  }
  PhaseCoordinate Coordinates[NUM_STARS];

  sf::Vector2f Origin = Window.getView().getCenter();
  for (auto &Coordinate : Coordinates) {
    const auto &P = State.getRandomXY();
    sf::Vector2f Position(P.first, P.second);
    sf::Vector2f Velocity = (Position - Origin).normalized().cwiseMul({2, 2});
    Coordinate = {.Position = Position, .Velocity = Velocity};
  }

  while (Window.isOpen()) {
    sf::Event Event;
    while (Window.pollEvent(Event)) {
      if (Event.type == sf::Event::Closed) {
        Window.close();
      }
    }

    // BEGIN FRAME
    Window.clear();
    const auto &CurrentView = Window.getView();
    const auto &Viewport = Window.getViewport(CurrentView);
    Origin = CurrentView.getCenter();

    for (size_t Idx = 0; Idx != NUM_STARS; ++Idx) {
      auto &Star = StarShapes[Idx];
      auto &Coord = Coordinates[Idx];
      Coord.Position += Coord.Velocity;
      // Scaling the velocity with the frame creates a faux-parallax effect
      // because "neighbouring" stars cross each other
      Coord.Velocity *= SCALE_FACTOR;
      StarScale[Idx] *= SCALE_FACTOR;
      auto ScreenPosition = Window.mapCoordsToPixel(Coord.Position);

      if (Viewport.contains(ScreenPosition)) {
        Star.setPosition(Coord.Position);
        Star.setScale({StarScale[Idx], StarScale[Idx]});
        Window.draw(Star);
      } else {
        auto NextPos = State.getRandomXY();
        auto NextColor = State.getRandomColor();
        Star.setFillColor(NextColor);
        Coord.Position.x = NextPos.first;
        Coord.Position.y = NextPos.second;
        Coord.Velocity =
            (Coord.Position - Origin).normalized().cwiseMul({2, 2});
        StarScale[Idx] = 1.0;
      }
    }

    // END FRAME
    Window.display();
  }
  return 0;
}
