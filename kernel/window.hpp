/**
 * @file window.hpp
 *
 * 表示領域を表すWindowクラスを提供する。
 */

#pragma once

#include <vector>
#include <optional>
#include <string>
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
    virtual ~Window() = default;
    Window(const Window& rhs) = delete;
    Window& operator=(const Window& rhs) = delete;

    void DrawTo(FrameBuffer& dst, Vector2D<int> pos, const Rectangle<int>& area);
    void SetTransparentColor(std::optional<PixelColor> c);
    WindowWriter* Writer();

    const PixelColor& At(Vector2D<int> pos) const;
    void Write(Vector2D<int> pos, PixelColor c);

    int Width() const;
    int Height() const;
    Vector2D<int> Size() const;

    /** @brief このウィンドウの平面描画領域内で、矩形領域を異動する。
     *
     * @param dst_pos  移動先の原点
     * @param src      移動元矩形の原点と大きさ
     */
    void Move(Vector2D<int> dst_pos, const Rectangle<int>& src);

    virtual void Activate() {}
    virtual void Deactivate() {}

  private:
    int width_, height_;
    std::vector<std::vector<PixelColor>> data_{};
    WindowWriter writer_{*this};
    std::optional<PixelColor> transparent_color_{std::nullopt};

    FrameBuffer shadow_buffer_{};
};

class ToplevelWindow : public Window {
  public:
    static constexpr Vector2D<int> kTopLeftMargin{4, 24};
    static constexpr Vector2D<int> kBottomRightMargin{4, 4};

    class InnerAreaWriter : public PixelWriter {
      public:
        InnerAreaWriter(ToplevelWindow& window) : window_{window} {}
        virtual void Write(Vector2D<int> pos, const PixelColor& c) override {
          window_.Write(pos + kTopLeftMargin, c);
        }
        virtual int Width() const override {
          return window_.Width() - kTopLeftMargin.x - kBottomRightMargin.x; }
        virtual int Height() const override {
          return window_.Height() - kTopLeftMargin.y - kBottomRightMargin.y; }
      private:
        ToplevelWindow& window_;
    };

    ToplevelWindow(int width, int height, PixelFormat shadow_format,
                  const std::string& title);

    virtual void Activate() override;
    virtual void Deactivate() override;

    InnerAreaWriter* InnerWriter() { return &inner_writer_; }
    Vector2D<int> InnerSize() const;

  private:
    std::string title_;
    InnerAreaWriter inner_writer_{*this};
};

void DrawWindow(PixelWriter& writer, const char* title);
void DrawTextbox(PixelWriter& writer, Vector2D<int> pos, Vector2D<int> size);
void DrawWindowTitle(PixelWriter& writer, const char* title, bool activate);