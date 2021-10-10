/**
 * @file window.hpp
 *
 * 表示領域を表すWindowクラスを提供する。
 */

#pragma once

#include <vector>
#include <optional>
#include "graphics.hpp"
#include "frame_buffer.hpp"

/** @brief Windowクラスはグラフィックの表示領域を表す。
 *
 * タイトルやメニューがあるウィンドウだけでなく、マウスカーソルなども対象とする。
 */
class Window {
  public:
    class WindowWriter : public PixelWriter {
      public:
        WindowWriter(Window& window) : window_{window} {}
        virtual void Write(Vector2D<int> pos, const PixelColor& c) override {
          window_.Write(pos, c);
        }
        virtual int Width() const override { return window_.Width(); }
        virtual int Height() const override { return window_.Height(); }
      private:
        Window& window_;
    };
    Window(int width, int height, PixelFormat shadow_format);
    ~Window() = default;
    Window(const Window& rhs) = delete;
    Window& operator=(const Window& rhs) = delete;

    void DrawTo(FrameBuffer& dst, Vector2D<int> position);
    void SetTransparentColor(std::optional<PixelColor> c);
    WindowWriter* Writer();

    const PixelColor& At(Vector2D<int> pos) const;
    void Write(Vector2D<int> pos, PixelColor c);

    int Width() const;
    int Height() const;

    /** @brief このウィンドウの平面描画領域内で、矩形領域を異動する。
     *
     * @param dst_pos  移動先の原点
     * @param src      移動元矩形の原点と大きさ
     */
    void Move(Vector2D<int> dst_pos, const Rectangle<int>& src);

  private:
    int width_, height_;
    std::vector<std::vector<PixelColor>> data_{};
    WindowWriter writer_{*this};
    std::optional<PixelColor> transparent_color_{std::nullopt};

    FrameBuffer shadow_buffer_{};
};

void DrawWindow(PixelWriter& writer, const char* title);