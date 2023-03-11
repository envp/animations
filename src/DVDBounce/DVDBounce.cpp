#include "Assets.h"
#include "raylib.h"
#include "raymath.h"
#include <array>
#include <cassert>
#include <filesystem>

// clang-format off
constexpr Color ALL_COLORS[] = {
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    BLUE,
    MAGENTA,
    VIOLET,
    WHITE
};
// clang-format on

constexpr size_t NUM_COLORS = sizeof(ALL_COLORS) / sizeof(ALL_COLORS[0]);

static Color pick_random_color() {
  return ALL_COLORS[GetRandomValue(0, NUM_COLORS - 1)];
}

class Entity {
public:
  explicit Entity(const Rectangle &region)
      : m_top_left({region.x, region.y}), m_width(region.width),
        m_height(region.height) {}

  Vector2 &velocity() { return m_velocity; }

  const Vector2 &position() const { return m_top_left; }

  bool is_overlapping_x(const Rectangle &bounding_rect) const {
    return m_top_left.x < bounding_rect.x ||
           m_top_left.x + m_width >= bounding_rect.width;
  }

  bool is_overlapping_y(const Rectangle &bounding_rect) const {
    return m_top_left.y < bounding_rect.y ||
           m_top_left.y + m_height >= bounding_rect.height;
  }

  void tick() { m_top_left = Vector2Add(m_top_left, m_velocity); }

private:
  Vector2 m_top_left;
  float m_width;
  float m_height;
  Vector2 m_velocity = {0.0, 0.0};
};

int main() {
  Rectangle screen{0, 0, 800, 600};
  InitWindow(static_cast<int>(screen.width), static_cast<int>(screen.height),
             "DVD");
  SetTargetFPS(60);

  const std::filesystem::path ASSET_ROOT(DVDBOUNCE_ASSET_ROOT);
  auto asset_path = ASSET_ROOT / "dvd-logo-texture.png";
  // Check if texture loaded
  const Texture2D DVD_LOGO = LoadTexture(asset_path.c_str());

  if (DVD_LOGO.id <= 0) {
    return -1;
  }

  const auto TEXTURE_WIDTH = DVD_LOGO.width;
  const auto TEXTURE_HEIGHT = DVD_LOGO.height;
  const Vector2 &top_left = {
      static_cast<float>(GetRandomValue(0, static_cast<int>(screen.width) -
                                               TEXTURE_WIDTH - 1)),
      static_cast<float>(GetRandomValue(0, static_cast<int>(screen.height) -
                                               TEXTURE_HEIGHT - 1))};
  Entity logo_entity(/* region = */ {top_left.x, top_left.y,
                                     static_cast<float>(TEXTURE_WIDTH),
                                     static_cast<float>(TEXTURE_HEIGHT)});
  logo_entity.velocity() = {2, 2};
  Color current_color = ALL_COLORS[0];

  while (!WindowShouldClose()) {
    if (IsWindowResized()) {
      screen.width = static_cast<float>(GetScreenWidth());
      screen.height = static_cast<float>(GetScreenHeight());
    }
    BeginDrawing();
    {
      ClearBackground(BLACK);
      DrawTextureV(DVD_LOGO, logo_entity.position(), current_color);
      bool bounced = false;
      // Invert X-velocity if it touches the vertical bounds anywhere
      if (logo_entity.is_overlapping_x(screen)) {
        logo_entity.velocity().x *= -1;
        bounced = true;
      }
      // Invert Y-velocity if it touches the horizontal bounds anywhere
      if (logo_entity.is_overlapping_y(screen)) {
        logo_entity.velocity().y *= -1;
        bounced = true;
      }
      if (bounced) {
        current_color = pick_random_color();
      }
      logo_entity.tick();
    }
    EndDrawing();
  }
  CloseWindow();
}
