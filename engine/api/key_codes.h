#if !defined(ENGINE_KEY_CODES)
#define ENGINE_KEY_CODES

enum Key_Code {
	KC_A, KC_B, KC_C, KC_D, KC_E, KC_F, KC_G, KC_H, KC_I, KC_J, KC_K, KC_L, KC_M, KC_N, KC_O, KC_P, KC_Q, KC_R, KC_S, KC_T, KC_U, KC_V, KC_W, KC_X, KC_Y, KC_Z,
	//
	KC_D0, KC_D1, KC_D2, KC_D3, KC_D4, KC_D5, KC_D6, KC_D7, KC_D8, KC_D9,
	KC_N0, KC_N1, KC_N2, KC_N3, KC_N4, KC_N5, KC_N6, KC_N7, KC_N8, KC_N9,
	//
	KC_F0, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12,
	KC_F13, KC_F14, KC_F15, KC_F16, KC_F17, KC_F18, KC_F19, KC_F20, KC_F21, KC_F22, KC_F23, KC_F24,
	//
	KC_Left, KC_Right, KC_Down, KC_Up,
	KC_M1, KC_M2, KC_M3, KC_M4, KC_M5,
	//
	KC_Shift, KC_Control, KC_Alt,
	KC_Esc, KC_Enter, KC_Tab, KC_Space, KC_Backspace, KC_Del, KC_Insert, KC_Home, KC_End, KC_PrintScreen, KC_PageUp, KC_PageDown,
	KC_CapsLock, KC_NumLock, KC_ScrollLock, KC_Mul, KC_Div, KC_Add, KC_Sub, KC_Decimal, KC_Separator,
	KC_Minus, KC_Plus, KC_Comma, KC_Period,
	KC_Semicolon, KC_Tilde, KC_Slash, KC_SquareL, KC_Backslash, KC_SquareR, KC_Quote,
};

#endif // ENGINE_KEY_CODES
