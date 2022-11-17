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
  Vector2 velocity{2, 2};
  Vector2 top_left = {
      static_cast<float>(GetRandomValue(0, static_cast<int>(screen.width) -
                                               TEXTURE_WIDTH - 1)),
      static_cast<float>(GetRandomValue(0, static_cast<int>(screen.height) -
                                               TEXTURE_HEIGHT - 1))};
  Color current_color = ALL_COLORS[0];

  while (!WindowShouldClose()) {
    if (IsWindowResized()) {
      screen.width = static_cast<float>(GetScreenWidth());
      screen.height = static_cast<float>(GetScreenHeight());
    }
    BeginDrawing();
    {
      ClearBackground(BLACK);
      DrawTextureV(DVD_LOGO, top_left, current_color);
      bool bounced = false;
      // Invert X-velocity if it touches the vertical bounds anywhere
      if (top_left.x < 0 ||
          top_left.x + static_cast<float>(TEXTURE_WIDTH) >= screen.width) {
        velocity.x *= -1;
        bounced = true;
      }

      // Invert Y-velocity if it touches the horizontal bounds anywhere
      if (top_left.y < 0 ||
          top_left.y + static_cast<float>(TEXTURE_HEIGHT) >= screen.height) {
        velocity.y *= -1;
        bounced = true;
      }

      if (bounced) {
        current_color = ALL_COLORS[GetRandomValue(0, NUM_COLORS - 1)];
      }

      top_left = Vector2Add(top_left, velocity);
    }
    EndDrawing();
  }
  CloseWindow();
}
