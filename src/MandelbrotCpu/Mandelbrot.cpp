#include <cassert>
#include <cmath>
#include <complex>
#include <cstdlib>

#include <algorithm>
#include <array>
#include <iterator>
#include <limits>
#include <optional>
#include <vector>

#include <raylib.h>

#ifndef MANDELBROT_PALETTE_INC
#error "Missing definition MANDELBROT_PALETTE_INC to use a color palette file."
#endif

#ifndef MANDELBROT_PALETTE_NUM_LEVELS
#error "Missing definition for MANDELBROT_PALETTE_NUM_LEVELS."
#endif

using u16 = std::uint16_t;
constexpr const u16 N_LEVELS = MANDELBROT_PALETTE_NUM_LEVELS;

inline Color map_color(u16 level) {
#define MAKE_COLOR(r, g, b, a) (Color{ (r), (g), (b), (a) })
  constexpr std::array<Color, N_LEVELS> COLOR_MAP = {
#include MANDELBROT_PALETTE_INC
  };
#undef MAKE_COLOR
  return COLOR_MAP[level];
}

static inline std::complex<float>
pixel_to_plane_coord(size_t row, size_t col, const Rectangle &screen) {
  // The scaled coordinates lie in:
  float im = 2 * static_cast<float>(row) / screen.height - 1;
  float re = 2 * static_cast<float>(col) / screen.width - 1;
  return std::complex<float>{ re - 0.4f, im } * 1.5f;
}

static inline u16 calc_mandelbrot_level(std::complex<float> coeff) {
  std::complex<float> z(0, 0);
  constexpr const size_t MAX_ITERATIONS = N_LEVELS - 1;
  u16 n_iterations = 0;

  while (std::norm(z) <= 4 && n_iterations < MAX_ITERATIONS) {
    z = z * z + coeff;
    ++n_iterations;
  }
  return n_iterations;
}

class DragRegion {
public:
  explicit DragRegion(Vector2 start)
      : m_start(start), m_end(std::move(start)) {}

  void set_end(Vector2 end) { m_end = std::move(end); }

  // Calculate the rectangular region created by dragging between from
  // first to last. Cannot be
  Rectangle as_rect() const {
    return Rectangle{
      std::min(m_start.x, m_end.x),
      std::min(m_start.y, m_end.y),
      std::abs(m_end.x - m_start.x),
      std::abs(m_end.y - m_start.y),
    };
  }

private:
  Vector2 m_start;
  Vector2 m_end;
};

/// A fixed size two-dimensional array that lives on the heap.
/// This is semantically like a vector<vector<T>>.
///
/// 1. only allocation happens at initialization
/// 2. It is stored in row-major order
///
/// The main advantage of this type is to be able to:
/// 1. Avoid metadata overheads of a vector-of-vectors
template <typename ElementType, size_t NumX, size_t NumY>
class FixedHeapArray2D {
  using StorageType = std::vector<ElementType>;

public:
  /// Returns a view of the storage in FixedHeapArray2D. The lifetime of this
  /// type cannot exceed its parent without an explicit copy.
  ///
  /// Required due to the constraint on `operator[]` being a binary operator.
  class FixedHeapArrayView {

  public:
    using const_iterator_type = typename StorageType::const_iterator;
    FixedHeapArrayView(StorageType *storage, size_t row)
        : m_storage(storage), m_start(row * NumX) {}

    // Non-copyable & non-movable type. These are typically intended to
    // allocated once and kept around as pooled storage.
    FixedHeapArrayView(const FixedHeapArrayView &) = delete;
    FixedHeapArrayView(FixedHeapArrayView &&) = delete;

    ElementType &operator[](size_t idx) {
      assert(idx < NumX && "Out of bounds access in FixedHeapArrayView");
      return (*m_storage)[m_start + idx];
    }

    const ElementType &operator[](size_t idx) const {
      assert(idx < NumX && "Out of bounds access in FixedHeapArrayView");
      return (*m_storage)[m_start + idx];
    }

  private:
    StorageType *m_storage;
    size_t m_start;
  };

  explicit FixedHeapArray2D(ElementType fill_value) noexcept
      : m_storage(NumX * NumY, ElementType(fill_value)) {}

  // Non-copyable & non-movable type. These are typically intended to allocated
  // once and kept around as pooled storage.
  FixedHeapArray2D(const FixedHeapArray2D &) = delete;
  FixedHeapArray2D(FixedHeapArray2D &&) = delete;

  FixedHeapArrayView operator[](size_t row_idx) {
    assert(row_idx < NumY && "Out of bounds X access");
    return FixedHeapArrayView(&m_storage, row_idx);
  }
  const FixedHeapArrayView &operator[](size_t row_idx) const {
    assert(row_idx < NumY && "Out of bounds X access");
    return FixedHeapArrayView(&m_storage, row_idx);
  }

  ElementType *data() { return m_storage.data(); }

private:
  static const constexpr size_t NUM_ELEMS = NumX * NumY;
  StorageType m_storage;
};

int main() {
  constexpr const size_t SCREEN_WIDTH = 900;
  constexpr const size_t SCREEN_HEIGHT = 900;
  constexpr Rectangle SCREEN{ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
  InitWindow(static_cast<int>(SCREEN.width), static_cast<int>(SCREEN.height),
             "starfield");
  SetExitKey(KEY_Q);
  SetGesturesEnabled(GESTURE_DRAG);
  SetTargetFPS(60);

  FixedHeapArray2D<u16, SCREEN_WIDTH, SCREEN_HEIGHT> iterations(0);
  FixedHeapArray2D<Color, SCREEN_WIDTH, SCREEN_HEIGHT> pixel_buf(BLACK);

  for (size_t row = 0; row < SCREEN_HEIGHT; ++row) {
    for (size_t col = 0; col < SCREEN_WIDTH; ++col) {
      auto coeff = pixel_to_plane_coord(row, col, SCREEN);
      auto value = calc_mandelbrot_level(coeff);
      iterations[row][col] = value;
      pixel_buf[row][col] = map_color(value);
    }
  }

  Image img = { pixel_buf.data(), SCREEN_WIDTH, SCREEN_HEIGHT, 1,
                PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };
  Texture2D texture = LoadTextureFromImage(img);
  std::optional<DragRegion> drag_region;
  std::optional<DragRegion> zoom_region;
  bool show_debug_info = false;
  while (!WindowShouldClose()) {
    // FIXME: There is a minimum delay / movement required to detect a drag
    // gesture, so the drag region's start doesn't make 1-to-1 with where the
    // mouse button was pressed.
    if (IsGestureDetected(GESTURE_DRAG)) {
      if (!drag_region) {
        drag_region = DragRegion(GetMousePosition());
      } else {
        drag_region->set_end(GetMousePosition());
      }
    } else {
      zoom_region = drag_region;
      drag_region.reset();
    }
    if (IsKeyPressed(KEY_D)) {
      show_debug_info = !show_debug_info;
    }
    BeginDrawing();
    {
      DrawTexture(texture, 0, 0, WHITE);
      DrawLine(0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH / 2, RED);
      DrawLine(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, RED);
      if (drag_region) {
        DrawRectangleLinesEx(drag_region->as_rect(), 0.5f, RAYWHITE);
      }
      if (zoom_region) {
        auto rect = zoom_region->as_rect();
      }
      if (show_debug_info) {
        int row = GetMouseY();
        int col = GetMouseX();
        int font_size = 16;
        Vector2 dbginfo_topleft;
        // Try to draw above & to the right the mouse, but if that's not
        // possible draw to the left at the same level.
        dbginfo_topleft.y =
            static_cast<float>(row < font_size ? row : row - font_size);
        dbginfo_topleft.x =
            static_cast<float>(col < font_size ? col : col - font_size * 2);
        if (row < font_size) {
        } else {
          row -= font_size;
        }
        auto value = iterations[row][col];
        DrawText(std::to_string(value).c_str(), col, row, font_size, RAYWHITE);
      }
    }
    DrawFPS(0, 10);
    DrawText("q: Exit"
             "\n"
             "d: Toggle Debug Info",
             0, 40, 20, WHITE);
    EndDrawing();
  }
  UnloadTexture(texture);
  CloseWindow();
}
