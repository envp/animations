#include <raylib.h>
#include <raymath.h>

#include <algorithm>
#include <array>
#include <random>
#include <string>
#include <vector>

struct Star {
  Vector2 m_center;
  Color m_color;
  float m_inverse_scale;
};

template <typename Generator>
Vector2 get_random_position(const Rectangle &region, Generator &gen) {
  static std::uniform_real_distribution<float> x_rng(-0.5f * region.width,
                                                     0.5f * region.width);
  static std::uniform_real_distribution<float> y_rng(-0.5f * region.height,
                                                     0.5f * region.height);
  return {x_rng(gen), y_rng(gen)};
}

template <typename Generator> Color get_random_color(Generator &gen) {
  // clang-format off
  constexpr Color ALL_COLORS[] = {
      RED,
      ORANGE,
      YELLOW,
      GOLD,
      SKYBLUE,
      MAGENTA,
      VIOLET,
      BEIGE
  };
  // clang-format on

  constexpr size_t NUM_COLORS = sizeof(ALL_COLORS) / sizeof(ALL_COLORS[0]);
  return ALL_COLORS[GetRandomValue(0, NUM_COLORS - 1)];
}

bool is_visible(const Rectangle &screen, const Vector2 &point) {
  return point.x > 0 && point.x < screen.width && point.y > 0 &&
         point.y < screen.height;
}

bool draw(const Star &star, const Rectangle &screen) {
  float radius = 1.5f * std::min(3.0f, 1.f / star.m_inverse_scale);
  Vector2 offset = {screen.width / 2, screen.height / 2};
  Vector2 scaled_position =
      Vector2Scale(star.m_center, 1.f / star.m_inverse_scale);
  Vector2 screen_position = Vector2Add(scaled_position, offset);
  bool visible = is_visible(screen, screen_position);
  if (visible) {
    auto alpha = static_cast<unsigned char>(255 * (1.f - star.m_inverse_scale));
    Color outline_color = star.m_color;
    outline_color.a = alpha/2;
    Color fill_color = WHITE;
    fill_color.a = alpha;
    DrawCircleV(screen_position, radius, outline_color);
    DrawCircleV(screen_position, 0.75f * radius, fill_color);
  }
  return visible;
}

template <typename Generator>
void initialize_star(const Rectangle &screen, Generator &generator,
                     Star &star) {
  static std::uniform_real_distribution<float> inverse_scale_rng(1.0f / 3, 1.f);
  star.m_center = get_random_position(screen, generator);
  star.m_color = get_random_color(generator);
  star.m_inverse_scale = inverse_scale_rng(generator);
}

int main() {
  std::ranlux48_base generator;
  Rectangle screen{0, 0, 1200, 900};
  InitWindow(static_cast<int>(screen.width), static_cast<int>(screen.height),
             "starfield");
  SetTargetFPS(60);
  std::array<Star, 1024> stars;

  SetTraceLogLevel(LOG_INFO);

  for (auto &star : stars) {
    initialize_star(screen, generator, star);
  }
  float delta_inverse_scale = 1.0 / 300;

  while (!WindowShouldClose()) {
    BeginDrawing();
    {
      ClearBackground(BLACK);
      for (auto &star : stars) {
        bool drawn = draw(star, screen);
        if (!drawn) {
          initialize_star(screen, generator, star);
        } else {
          star.m_inverse_scale -= delta_inverse_scale;
        }
      }
    }
    DrawFPS(10, 10);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
