/**
 * @file main.cpp
 *
 * カーネル本体のプログラムを書いたファイル．
 */

#include <cstdint>
#include <cstddef>
#include <cstdio>

#include <numeric>
#include <vector>
#include <deque>
#include <limits>

#include "frame_buffer_config.hpp"
#include "memory_map.hpp"
#include "graphics.hpp"
#include "mouse.hpp"
#include "font.hpp"
#include "console.hpp"
#include "pci.hpp"
#include "logger.hpp"
#include "usb/xhci/xhci.hpp"
#include "interrupt.hpp"
#include "asmfunc.h"
#include "segment.hpp"
#include "paging.hpp"
#include "memory_manager.hpp"
#include "window.hpp"
#include "layer.hpp"
#include "message.hpp"
#include "timer.hpp"
#include "acpi.hpp"
#include "keyboard.hpp"
#include "task.hpp"

int printk(const char* format, ...) {
  va_list ap;
  int result;
  char s[1024];

  va_start(ap, format);
  result = vsprintf(s, format, ap);
  va_end(ap);

  console->PutString(s);
  return result;
}

std::shared_ptr<Window> main_window;
unsigned int main_window_layer_id;
void InitializeMainWindow() {
  main_window = std::make_shared<Window>(
      160, 52, screen_config.pixel_format);
  DrawWindow(*main_window->Writer(), "Hello Window");

  main_window_layer_id = layer_manager->NewLayer()
    .SetWindow(main_window)
    .SetDraggable(true)
    .Move({300, 100})
    .ID();

  layer_manager->UpDown(main_window_layer_id, std::numeric_limits<int>::max());
}

std::shared_ptr<Window> text_window;
unsigned int text_window_layer_id;
void InitializeTextWindow() {
  const int win_w = 160;
  const int win_h = 52;

  text_window = std::make_shared<Window>(
      win_w, win_h, screen_config.pixel_format);
  DrawWindow(*text_window->Writer(), "Text Box Test");
  DrawTextbox(*text_window->Writer(), {4, 24}, {win_w - 8, win_h - 24 - 4});

  text_window_layer_id = layer_manager->NewLayer()
    .SetWindow(text_window)
    .SetDraggable(true)
    .Move({350, 200})
    .ID();

  layer_manager->UpDown(text_window_layer_id, std::numeric_limits<int>::max());
}

int text_window_index;

void DrawTextCursor(bool visible) {
  const auto color = visible ? ToColor(0) : ToColor(0xffffff);
  const auto pos = Vector2D<int>{8 + 8*text_window_index, 24 + 5};
  FillRectangle(*text_window->Writer(), pos, {7, 15}, color);
}

void InputTextWindow(char c) {
  if (c == 0) {
    return;
  }

  auto pos = []() { return Vector2D<int>{8 + 8*text_window_index, 24 + 6}; };

  const int max_chars = (text_window->Width() - 16) / 8 - 1;
  if (c == '\b' && text_window_index > 0) {
    DrawTextCursor(false);
    --text_window_index;
    FillRectangle(*text_window->Writer(), pos(), {8, 16}, ToColor(0xffffff));
    DrawTextCursor(true);
  } else if (c >= ' ' && text_window_index < max_chars) {
    DrawTextCursor(false);
    WriteAscii(*text_window->Writer(), pos(), c, ToColor(0));
    ++text_window_index;
    DrawTextCursor(true);
  }

  layer_manager->Draw(text_window_layer_id);
}

std::shared_ptr<Window> task_b_window;
unsigned int task_b_window_layer_id;
void InitializeTaskBWindow() {
  task_b_window = std::make_shared<Window>(
      160, 52, screen_config.pixel_format);
  DrawWindow(*task_b_window->Writer(), "TaskB Window");

  task_b_window_layer_id = layer_manager->NewLayer()
    .SetWindow(task_b_window)
    .SetDraggable(true)
    .Move({100, 100})
    .ID();

  layer_manager->UpDown(task_b_window_layer_id, std::numeric_limits<int>::max());
}

void TaskB(int task_id, int data) {
  printk("TaskB: task_id=%d, data=%d\n", task_id, data);
  char str[128];
  int count = 0;
  while (true) {
    ++count;
    sprintf(str, "%010d", count);
    FillRectangle(*task_b_window->Writer(), {24, 28}, {8 * 10, 16}, {0xc6, 0xc6, 0xc6});
    WriteString(*task_b_window->Writer(), {24, 28}, str, {0, 0, 0});
    layer_manager->Draw(task_b_window_layer_id);
  }
}

std::deque<Message>* main_queue;

alignas(16) uint8_t kernel_main_stack[1024 * 1024];

extern "C" void KernelMainNewStack(
    const FrameBufferConfig& frame_buffer_config_ref,
    const MemoryMap& memory_map_ref,
    const acpi::RSDP& acpi_table) {
  MemoryMap memory_map{memory_map_ref};

  InitializeGraphics(frame_buffer_config_ref);
  InitializeConsole();

  printk("Welcome to MikanOS!\n");
  SetLogLevel(kWarn);

  InitializeSegmentation();
  InitializePaging();
  InitializeMemoryManager(memory_map);
  ::main_queue = new std::deque<Message>(32);
  InitializeInterrupt(main_queue);

  InitializePCI();
  usb::xhci::Initialize();

  InitializeLayer();
  InitializeMainWindow();
  InitializeTextWindow();
  InitializeTaskBWindow();
  InitializeMouse();
  layer_manager->Draw({{0, 0}, ScreenSize()});

  acpi::Initialize(acpi_table);
  InitializeLAPICTimer(*main_queue);

  InitializeKeyboard(*main_queue);

  const int kTextboxCursorTimer = 1;
  const int kTimer05Sec = static_cast<int>(kTimerFreq * 0.5);
  __asm__("cli");
  timer_manager->AddTimer(Timer{kTimer05Sec, kTextboxCursorTimer});
  __asm__("sti");
  bool textbox_cursor_visible = false;

  std::vector<uint64_t> task_b_stack(1024);
  uint64_t task_b_stack_addr = reinterpret_cast<uint64_t>(&task_b_stack[0]);
  uint64_t* task_b_stack_aligned =
    reinterpret_cast<uint64_t*>(task_b_stack_addr & ~0xflu);

  task_b_stack_aligned[1023] = 0; // not-used
  task_b_stack_aligned[1022] = reinterpret_cast<uint64_t>(StartTask);
  task_b_stack_aligned[1021] = 0; // rax
  task_b_stack_aligned[1020] = 0; // rbx
  task_b_stack_aligned[1019] = 0; // rcx
  task_b_stack_aligned[1018] = reinterpret_cast<uint64_t>(TaskB); // rdx
  task_b_stack_aligned[1017] = 1; // rdi
  task_b_stack_aligned[1016] = 43; // rsi
  task_b_stack_aligned[1015] = 0; // rbp
  task_b_stack_aligned[1014] = 0; // r8
  task_b_stack_aligned[1013] = 0; // r9
  task_b_stack_aligned[1012] = 0; // r10
  task_b_stack_aligned[1011] = 0; // r11
  task_b_stack_aligned[1010] = 0; // r12
  task_b_stack_aligned[1009] = 0; // r13
  task_b_stack_aligned[1008] = 0; // r14
  task_b_stack_aligned[1007] = 0; // r15

  task_b_rsp = reinterpret_cast<uint64_t>(&task_b_stack_aligned[1007]);

  InitializeTask();

  char str[128];

  while (true) {
    __asm__("cli");
    const auto tick = timer_manager->CurrentTick();
    __asm__("sti");

    sprintf(str, "%010lu", tick);
    FillRectangle(*main_window->Writer(), {24, 28}, {8 * 10, 16}, {0xc6, 0xc6, 0xc6});
    WriteString(*main_window->Writer(), {24, 28}, str, {0, 0, 0});
    layer_manager->Draw(main_window_layer_id);

    __asm__("cli");
    if (main_queue->size() == 0) {
      __asm__("sti\n\thlt");
      continue;
    }

    Message msg = main_queue->front();
    main_queue->pop_front();
    __asm__("sti");

    switch (msg.type) {
    case Message::kInterruptXHCI:
      usb::xhci::ProcessEvents();
      break;
    case Message::kTimerTimeout:
      if (msg.arg.timer.value == kTextboxCursorTimer) {
        __asm__("cli");
        timer_manager->AddTimer(
            Timer{msg.arg.timer.timeout + kTimer05Sec, kTextboxCursorTimer});
        __asm__("sti");
        textbox_cursor_visible = !textbox_cursor_visible;
        DrawTextCursor(textbox_cursor_visible);
        layer_manager->Draw(text_window_layer_id);
      }
      break;
    case Message::kKeyPush:
      InputTextWindow(msg.arg.keyboard.ascii);
      break;
    default:
      Log(kError, "Unknown message type: %d\n", msg.type);
    }
  }
}

extern "C" void __cxa_pure_virtual() {
  while (1) __asm__("hlt");
}
