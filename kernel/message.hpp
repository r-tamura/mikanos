#pragma once

struct Message {
  enum Type {
    kInterruptXHCI,
    kInterruptAPICTimer,
    kTimerTimeout,
  } type;

  union {
    struct {
      unsigned long timeout;
      int value;
    } timer;
  } arg;
};