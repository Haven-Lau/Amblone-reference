#define HOTKEY_DISPLAY_1      0
#define HOTKEY_DISPLAY_2      1
#define HOTKEY_STATIC_COLOUR  2
#define HOTKEY_SWEEP          3
#define HOTKEY_FAKE_TV        4

struct HotKeyMapping {
  int ID; // The ID, one of the HOTKEY_ defines
  bool IsSet; // Is a hotkey set?
  UINT Modifiers; // Modifiers (Ctrl, Alt etc.)
  UINT Key; // Hotkey key
};
