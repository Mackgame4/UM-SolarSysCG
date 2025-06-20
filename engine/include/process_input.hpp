#pragma once

#include <bitset>
#include <cstdint>
#include <glm/glm.hpp>

struct Keyboard {
  enum Key : uint16_t {
    // clang-format off
        A, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        F1, F2, F3, F4, F5, F6, F7,
        F8, F9, F10, F11, F12,
        Left, Up, Right, Down,
        PageUp, PageDown,
        Home, End, Insert,
        LShift, RShift, LCtrl, RCtrl,
        Alt, Mod,
    // clang-format on
    // Not all inputs are implemented
    UnknownKey,
    _Count,
  };
};

struct Mouse {
  enum Button : uint8_t {
    Left,
    Middle,
    Right,
    // Not all inputs are implemented
    UnknownButton,
    _Count,
  };
};

class Input {
 public:
  // Keyboard event callbacks
  static void on_key_down(unsigned char native_key, int, int);
  static void on_key_up(unsigned char native_key, int, int);
  static void on_special_key_down(int native_key, int, int);
  static void on_special_key_up(int native_key, int, int);
  // Mouse event callbacks
  static void on_mouse_motion(int x, int y);
  static void on_mouse_button(int button, int state, int, int);
  // Updates input state (only mouse delta for now)
  static void process_input();

  // Keyboard

  static bool is_down(Keyboard::Key key);

  static bool is_up(Keyboard::Key key);

  // Mouse

  static bool is_down(Mouse::Button button);

  static bool is_up(Mouse::Button button);

  static glm::vec2 mouse_delta() { return s_mouse_delta; }

  static glm::vec2 mouse_position() { return s_mouse_position; }

 private:
  inline static std::bitset<Keyboard::Key::_Count> s_keyboard_state;
  inline static std::bitset<Mouse::Button::_Count> s_mouse_state;
  inline static glm::ivec2 s_mouse_position = glm::ivec2{};
  inline static glm::ivec2 s_mouse_delta = glm::ivec2{};

 private:
  static Keyboard::Key native_to_key(unsigned char);

  static Keyboard::Key special_native_to_key(int);

  static Mouse::Button native_to_button(int);
};