/**
 * @file layer.hpp
 *
 * 重ね合わせ処理を提供する
 */

#pragma once

#include <memory>
#include <map>
#include <vector>

#include "graphics.hpp"
#include "window.hpp"

/**
 * @brief Layerは1つの層を表す。
 *
 * 現状では1つのウィンドウしか保持しない設計だが,
 * 将来的には複数のウィンドウを持ち得る。
 */
class Layer {
  public:
    Layer(unsigned int id = 0);
    unsigned int ID() const;

    Layer& SetWindow(const std::shared_ptr<Window>& window);
    std::shared_ptr<Window> GetWindow() const;
    Vector2D<int> GetPosition() const;

    Layer& Move(Vector2D<int> pos);
    Layer& MoveRelative(Vector2D<int> pos_diff);

    void DrawTo(FrameBuffer& screen, const Rectangle<int>& area) const;

  private:
    unsigned int id_;
    Vector2D<int> pos_;
    std::shared_ptr<Window> window_;
};

class LayerManager {
  public:
    void SetWriter(FrameBuffer* screen);
    Layer& NewLayer();

    void Draw(const Rectangle<int>& area) const;
    /** @brief 指定したレイヤーに設定されるウィンドウの描画領域内を再描画する */
    void Draw(unsigned int id) const;

    void Move(unsigned int id, Vector2D<int> new_position);
    void MoveRelative(unsigned int id, Vector2D<int> pos_diff);

    void UpDown(unsigned int id, int new_height);
    void Hide(unsigned int id);

  private:
    FrameBuffer* screen_{nullptr};
    mutable FrameBuffer back_buffer_{};
    std::vector<std::unique_ptr<Layer>> layers_{};
    std::vector<Layer*> layer_stack_{};
    unsigned int latest_id_{0};

    Layer* FindLayer(unsigned int id);
};

extern LayerManager* layer_manager;