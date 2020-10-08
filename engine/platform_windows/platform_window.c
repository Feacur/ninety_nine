#include "engine/api/code.h"
#include "engine/api/maths.h"
#include "engine/api/math_types.h"
#include "engine/api/key_codes.h"

#include "api/rendering_context.h"
#include "api/internal_window.h"

#include <Windows.h>
#include <hidusage.h>

//
#define ENGINE_WINDOW_POINTER "engine_window_pointer"

static HWND impl_window_raw_input_target = NULL;

static LRESULT CALLBACK impl_window_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//
// API
//

#include "engine/api/platform_window.h"

struct Engine_Window {
	HWND handle;
	struct Rendering_Context * rendering_context;

	svec2 size;
	u8 vsync;

	struct {
		svec2 delta;
		svec2 display_position;
		svec2 window_position;
		vec2  wheel;
		bool  keys[8];
		bool  prev[8];
	} mouse;

	struct {
		bool keys[256];
		bool prev[256];
	} keyboard;
};

struct Engine_Window * engine_window_create(void) {
	struct Engine_Window * window = ENGINE_MALLOC(sizeof(*window));
	memset(window, 0, sizeof(*window));

	window->handle = CreateWindowExA(
		WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR,
		ENGINE_WINDOW_CLASS_NAME, "ninety nine",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		HWND_DESKTOP, NULL, GetModuleHandle(NULL), NULL
	);
	SetPropA(window->handle, ENGINE_WINDOW_POINTER, window);

	RECT window_rect;
	GetClientRect(window->handle, &window_rect);
	window->size = (svec2){window_rect.right - window_rect.left, window_rect.bottom - window_rect.top};

	return window;
}

void engine_window_destroy(struct Engine_Window * window) {
	if (window->handle) { DestroyWindow(window->handle); return; }
	ENGINE_FREE(window);
}

bool engine_window_is_active(struct Engine_Window * window) {
	return window->handle;
}

void engine_window_init_context(struct Engine_Window * window) {
	window->rendering_context = engine_rendering_context_create(window);
}

void engine_window_deinit_context(struct Engine_Window * window) {
	engine_rendering_context_destroy(window->rendering_context);
}

static void engine_window_reset_input(struct Engine_Window * window) {
	memcpy(window->mouse.prev,    window->mouse.keys,    sizeof(window->mouse.keys));
	memcpy(window->keyboard.prev, window->keyboard.keys, sizeof(window->keyboard.keys));
	window->mouse.delta = (svec2){0, 0};
	window->mouse.wheel = (vec2){0, 0};
}

void engine_window_update(struct Engine_Window * window) {
	engine_window_reset_input(window);
}

svec2 engine_window_mouse_delta(struct Engine_Window * window) {
	return window->mouse.delta;
}

svec2 engine_window_mouse_display_position(struct Engine_Window * window) {
	return window->mouse.display_position;
}

svec2 engine_window_mouse_window_position(struct Engine_Window * window) {
	return window->mouse.window_position;
}

bool engine_window_mouse_button(struct Engine_Window * window, u8 key) {
	if (key >= sizeof(window->mouse.keys)) { return false; }
	return window->mouse.keys[key];
}

bool engine_window_mouse_transition(struct Engine_Window * window, u8 key, bool state) {
	if (key >= sizeof(window->mouse.keys)) { return false; }
	bool now  = window->mouse.keys[key];
	bool prev = window->mouse.prev[key];
	return (now != prev) && (now == state);
}

bool engine_window_key(struct Engine_Window * window, u8 key) {
	return window->keyboard.keys[key];
}

bool engine_window_key_transition(struct Engine_Window * window, u8 key, bool state) {
	bool now  = window->keyboard.keys[key];
	bool prev = window->keyboard.prev[key];
	return (now != prev) && (now == state);
}

void engine_window_toggle_raw_input(struct Engine_Window * window) {
	engine_window_reset_input(window);

	// https://docs.microsoft.com/windows-hardware/drivers/hid/hid-usages
	HWND hwnd_target; USHORT flags;
	if (!impl_window_raw_input_target) {
		hwnd_target = window->handle; flags = 0;
	}
	else if (impl_window_raw_input_target == window->handle) {
		hwnd_target = NULL; flags = RIDEV_REMOVE;
	}
	else { return; }

	RAWINPUTDEVICE const devices[] = {
		(RAWINPUTDEVICE){.usUsagePage = HID_USAGE_PAGE_GENERIC, .usUsage = HID_USAGE_GENERIC_KEYBOARD, .dwFlags = flags, .hwndTarget = hwnd_target},
		(RAWINPUTDEVICE){.usUsagePage = HID_USAGE_PAGE_GENERIC, .usUsage = HID_USAGE_GENERIC_MOUSE,    .dwFlags = flags, .hwndTarget = hwnd_target},
	};
	if (RegisterRawInputDevices(devices, sizeof(devices) / sizeof(devices[0]), sizeof(RAWINPUTDEVICE))) {
		impl_window_raw_input_target = hwnd_target;
	}
}

void engine_window_toggle_borderless_fullsreen(struct Engine_Window * window) {
	static WINDOWPLACEMENT window_placement;
	HWND const hwnd = window->handle;
	
	LONG window_style = GetWindowLongA(hwnd, GWL_STYLE);
	if ((window_style & WS_OVERLAPPEDWINDOW) == WS_OVERLAPPEDWINDOW) {
		if (!GetWindowPlacement(hwnd, &window_placement)) { return; }

		MONITORINFO monitor_info = {.cbSize = sizeof(monitor_info)};
		if (!GetMonitorInfoA(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &monitor_info)) { return; }

		SetWindowLongA(hwnd, GWL_STYLE, window_style & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(
			hwnd, HWND_TOP,
			monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
			monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
			monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED
		);
		return;
	}

	SetWindowLongA(hwnd, GWL_STYLE, window_style | WS_OVERLAPPEDWINDOW);
	SetWindowPlacement(hwnd, &window_placement);
	SetWindowPos(
		hwnd, 0,
		0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED
	);
}

u16 engine_window_get_refresh_rate(struct Engine_Window * window, u16 default_value) {
	int value = GetDeviceCaps(GetDC(window->handle), VREFRESH);
	return value > 1 ? (u16)value : default_value;
}

//
// system API
//

#include "interoperations/system_window.h"

void engine_system__window_init(void) {
	RegisterClassExA(&(WNDCLASSEXA){
		.cbSize        = sizeof(WNDCLASSEXA),
		.lpszClassName = ENGINE_WINDOW_CLASS_NAME,
		.hInstance     = GetModuleHandleA(NULL),
		.lpfnWndProc   = impl_window_procedure,
		.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
		.hCursor       = LoadCursorA(0, IDC_ARROW),
	});
}

void engine_system__window_deinit(void) {
	UnregisterClassA(ENGINE_WINDOW_CLASS_NAME, GetModuleHandleA(NULL));
}

//
// context API
//

#include "interoperations/rendering_context_window.h"

HWND engine_rendering_context__window_get_handle(struct Engine_Window * window) {
	return window->handle;
}

void engine_rendering_context__window_detach(struct Engine_Window * window) {
	window->rendering_context = NULL;
}

//
// internal implementation
//

static void impl_keyboard_process_virtual_key(struct Engine_Window * window, USHORT key, bool is_down) {
	if ('A'        <= key && key <= 'Z')        { window->keyboard.keys[KC_A  + key - 'A']        = is_down; return; }
	if ('0'        <= key && key <= '9')        { window->keyboard.keys[KC_D0 + key - '0']        = is_down; return; }
	if (VK_NUMPAD0 <= key && key <= VK_NUMPAD9) { window->keyboard.keys[KC_N0 + key - VK_NUMPAD0] = is_down; return; }
	if (VK_F1      <= key && key <= VK_F24)     { window->keyboard.keys[KC_F1 + key - VK_F1]      = is_down; return; }
	//
	switch (key) {
		case VK_LEFT:  window->keyboard.keys[KC_Left]  = is_down; break;
		case VK_RIGHT: window->keyboard.keys[KC_Right] = is_down; break;
		case VK_DOWN:  window->keyboard.keys[KC_Down]  = is_down; break;
		case VK_UP:    window->keyboard.keys[KC_Up]    = is_down; break;
		//
		case VK_LBUTTON:  window->keyboard.keys[KC_M1] = is_down; break;
		case VK_RBUTTON:  window->keyboard.keys[KC_M2] = is_down; break;
		case VK_MBUTTON:  window->keyboard.keys[KC_M3] = is_down; break;
		case VK_XBUTTON1: window->keyboard.keys[KC_M4] = is_down; break;
		case VK_XBUTTON2: window->keyboard.keys[KC_M5] = is_down; break;
		//
		case VK_SHIFT:    window->keyboard.keys[KC_Shift]   = is_down; break;
		case VK_CONTROL:  window->keyboard.keys[KC_Control] = is_down; break;
		case VK_MENU:     window->keyboard.keys[KC_Alt]     = is_down; break;
		//
		case VK_ESCAPE:   window->keyboard.keys[KC_Esc]         = is_down; break;
		case VK_RETURN:   window->keyboard.keys[KC_Enter]       = is_down; break;
		case VK_TAB:      window->keyboard.keys[KC_Tab]         = is_down; break;
		case VK_SPACE:    window->keyboard.keys[KC_Space]       = is_down; break;
		case VK_BACK:     window->keyboard.keys[KC_Backspace]   = is_down; break;
		case VK_DELETE:   window->keyboard.keys[KC_Del]         = is_down; break;
		case VK_INSERT:   window->keyboard.keys[KC_Insert]      = is_down; break;
		case VK_HOME:     window->keyboard.keys[KC_Home]        = is_down; break;
		case VK_END:      window->keyboard.keys[KC_End]         = is_down; break;
		case VK_SNAPSHOT: window->keyboard.keys[KC_PrintScreen] = is_down; break;
		case VK_PRIOR:    window->keyboard.keys[KC_PageUp]      = is_down; break;
		case VK_NEXT:     window->keyboard.keys[KC_PageDown]    = is_down; break;
		//
		case VK_CAPITAL:   window->keyboard.keys[KC_CapsLock]   = is_down; break;
		case VK_NUMLOCK:   window->keyboard.keys[KC_NumLock]    = is_down; break;
		case VK_SCROLL:    window->keyboard.keys[KC_ScrollLock] = is_down; break;
		case VK_MULTIPLY:  window->keyboard.keys[KC_Mul]        = is_down; break;
		case VK_DIVIDE:    window->keyboard.keys[KC_Div]        = is_down; break;
		case VK_ADD:       window->keyboard.keys[KC_Add]        = is_down; break;
		case VK_SUBTRACT:  window->keyboard.keys[KC_Sub]        = is_down; break;
		case VK_DECIMAL:   window->keyboard.keys[KC_Decimal]    = is_down; break;
		case VK_SEPARATOR: window->keyboard.keys[KC_Separator]  = is_down; break;
		//
		case VK_OEM_MINUS:  window->keyboard.keys[KC_Minus]     = is_down; break;
		case VK_OEM_PLUS:   window->keyboard.keys[KC_Plus]      = is_down; break;
		case VK_OEM_COMMA:  window->keyboard.keys[KC_Comma]     = is_down; break;
		case VK_OEM_PERIOD: window->keyboard.keys[KC_Period]    = is_down; break;
		//
		case VK_OEM_1:      window->keyboard.keys[KC_Semicolon] = is_down; break;
		case VK_OEM_2:      window->keyboard.keys[KC_Slash]     = is_down; break;
		case VK_OEM_3:      window->keyboard.keys[KC_Tilde]     = is_down; break;
		case VK_OEM_4:      window->keyboard.keys[KC_SquareL]   = is_down; break;
		case VK_OEM_5:      window->keyboard.keys[KC_Backslash] = is_down; break;
		case VK_OEM_6:      window->keyboard.keys[KC_SquareR]   = is_down; break;
		case VK_OEM_7:      window->keyboard.keys[KC_Quote]     = is_down; break;
	}
}

static void raw_input_callback_mouse(struct Engine_Window * window, RAWMOUSE * data) {
	// https://docs.microsoft.com/windows/win32/api/winuser/ns-winuser-rawmouse
	bool const is_virtual_desktop = (data->usFlags & MOUSE_VIRTUAL_DESKTOP) == MOUSE_VIRTUAL_DESKTOP;
	int const height = GetSystemMetrics(is_virtual_desktop ? SM_CYVIRTUALSCREEN : SM_CYSCREEN);
	int const width  = GetSystemMetrics(is_virtual_desktop ? SM_CXVIRTUALSCREEN : SM_CXSCREEN);

	if ((data->usFlags & MOUSE_MOVE_ABSOLUTE) == MOUSE_MOVE_ABSOLUTE) {
		svec2 const previous_display_position = window->mouse.display_position;

		POINT position = (POINT){
			.x = mul_div_s32(data->lLastX, width,  UINT16_MAX),
			.y = mul_div_s32(data->lLastY, height, UINT16_MAX),
		};
		window->mouse.display_position = (svec2){position.x, position.y};
		window->mouse.display_position.y = height - (window->mouse.display_position.y + 1);
	
		ScreenToClient(window->handle, &position);
		window->mouse.window_position = (svec2){position.x, position.y};
		window->mouse.window_position.y = window->size.y - (window->mouse.window_position.y + 1);

		svec2 delta = (svec2){
			window->mouse.display_position.x - previous_display_position.x,
			window->mouse.display_position.y - previous_display_position.y,
		};
		window->mouse.delta.x += delta.x;
		window->mouse.delta.y += delta.y;
	}
	else if (data->lLastX != 0 || data->lLastY != 0) {
		POINT position;
		GetCursorPos(&position);
		window->mouse.display_position = (svec2){position.x, position.y};
		window->mouse.display_position.y = height - (window->mouse.display_position.y + 1);

		ScreenToClient(window->handle, &position);
		window->mouse.window_position = (svec2){position.x, position.y};
		window->mouse.window_position.y  = window->size.y - (window->mouse.window_position.y + 1);

		svec2 delta = (svec2){data->lLastX, -data->lLastY};
		window->mouse.delta.x += delta.x;
		window->mouse.delta.y += delta.y;
	}

	//
	if ((data->usButtonFlags & RI_MOUSE_HWHEEL) == RI_MOUSE_HWHEEL) {
		window->mouse.wheel.x += (r32)(short)data->usButtonData / WHEEL_DELTA;
	}

	if ((data->usButtonFlags & RI_MOUSE_WHEEL) == RI_MOUSE_WHEEL) {
		window->mouse.wheel.y += (r32)(short)data->usButtonData / WHEEL_DELTA;
	}

	//
	static u32 const keys_down[sizeof(window->mouse.keys)] = {
		RI_MOUSE_BUTTON_1_DOWN,
		RI_MOUSE_BUTTON_2_DOWN,
		RI_MOUSE_BUTTON_3_DOWN,
		RI_MOUSE_BUTTON_4_DOWN,
		RI_MOUSE_BUTTON_5_DOWN,
	};

	static u32 const keys_up[sizeof(window->mouse.keys)] = {
		RI_MOUSE_BUTTON_1_UP,
		RI_MOUSE_BUTTON_2_UP,
		RI_MOUSE_BUTTON_3_UP,
		RI_MOUSE_BUTTON_4_UP,
		RI_MOUSE_BUTTON_5_UP,
	};

	for (u8 i = 0; i < sizeof(window->mouse.keys); ++i) {
		bool is_down = (data->usButtonFlags & keys_down[i]) == keys_down[i];
		bool is_up   = (data->usButtonFlags & keys_up[i])   == keys_up[i];
		window->mouse.keys[i] = is_down && !is_up;
	}
}

static void raw_input_callback_keyboard(struct Engine_Window * window, RAWKEYBOARD * data) {
	// https://docs.microsoft.com/windows/win32/api/winuser/ns-winuser-rawkeyboard
	(void)window; (void)data;
	bool is_up = (data->Flags & RI_KEY_BREAK) == RI_KEY_BREAK;
	impl_keyboard_process_virtual_key(window, data->VKey, !is_up);
}

static void raw_input_callback_hid(struct Engine_Window * window, RAWHID * data) {
	// https://docs.microsoft.com/windows/win32/api/winuser/ns-winuser-rawhid
	(void)window; (void)data;
	printf("not implemented `raw_input_callback_hid`");
	ENGINE_DEBUG_BREAK();
}

static void impl_process_message_raw(struct Engine_Window * window, WPARAM wParam, LPARAM lParam) {
	// https://docs.microsoft.com/windows/win32/inputdev/raw-input
	(void)wParam; (void)lParam;
	if (impl_window_raw_input_target != window->handle) { return; }

	RAWINPUTHEADER header;
	UINT buffer_size = sizeof(header);
	if (GetRawInputData((HRAWINPUT)lParam, RID_HEADER, &header, &buffer_size, sizeof(RAWINPUTHEADER)) == (UINT)-1) { return; }

	buffer_size = header.dwSize;
	void * buffer = alloca(header.dwSize);
	if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &buffer_size, sizeof(RAWINPUTHEADER)) == (UINT)-1) { return; }

	RAWINPUT * input = (RAWINPUT *)buffer;
	switch (input->header.dwType) {
		case RIM_TYPEMOUSE:
			raw_input_callback_mouse(window, &input->data.mouse);
			break;
		case RIM_TYPEKEYBOARD:
			raw_input_callback_keyboard(window, &input->data.keyboard);
			break;
		case RIM_TYPEHID:
			raw_input_callback_hid(window, &input->data.hid);
			break;
	}
}

static void impl_process_message_mouse(struct Engine_Window * window, WPARAM wParam, LPARAM lParam, bool client_space, vec2 wheel_mask) {
	// https://docs.microsoft.com/windows/win32/inputdev/mouse-input
	if (impl_window_raw_input_target == window->handle) { return; }

	int const height = GetSystemMetrics(SM_CYSCREEN);

	POINTS points = MAKEPOINTS(lParam);
	POINT client, screen;
	if (client_space) {
		client = (POINT){.x = points.x, .y = points.y};
		screen = client;
		ClientToScreen(window->handle, &screen);
	}
	else {
		screen = (POINT){.x = points.x, .y = points.y};
		client = screen;
		ScreenToClient(window->handle, &client);
	}

	svec2 const previous_display_position = window->mouse.display_position;

	window->mouse.display_position = (svec2){screen.x, screen.y};
	window->mouse.display_position.y = height - (window->mouse.display_position.y + 1);

	window->mouse.window_position = (svec2){client.x, client.y};
	window->mouse.window_position.y = window->size.y - (window->mouse.window_position.y + 1);

	svec2 delta = (svec2){
		window->mouse.display_position.x - previous_display_position.x,
		window->mouse.display_position.y - previous_display_position.y,
	};
	window->mouse.delta.x += delta.x;
	window->mouse.delta.y += delta.y;

	//
	window->mouse.wheel.x += wheel_mask.x * (GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
	window->mouse.wheel.y += wheel_mask.y * (GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);

	//
	static u32 const keys_down[sizeof(window->mouse.keys)] = {
		MK_LBUTTON,
		MK_MBUTTON,
		MK_RBUTTON,
		MK_XBUTTON1,
		MK_XBUTTON2,
	};

	WPARAM virtual_keys = wParam;
	for (u8 i = 0; i < sizeof(window->mouse.keys); ++i) {
		bool is_down = (virtual_keys & keys_down[i]) == keys_down[i];
		window->mouse.keys[i] = is_down;
	}
}

static void impl_process_message_keyboard(struct Engine_Window * window, WPARAM wParam, LPARAM lParam) {
	// https://docs.microsoft.com/windows/win32/inputdev/keyboard-input
	(void)wParam; (void)lParam;
	if (impl_window_raw_input_target == window->handle) { return; }

	WORD flag = HIWORD(lParam);
	bool is_up = (flag & KF_UP) == KF_UP;
	impl_keyboard_process_virtual_key(window, (USHORT)wParam, !is_up);
}

static LRESULT CALLBACK impl_window_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	struct Engine_Window * window = GetPropA(hwnd, ENGINE_WINDOW_POINTER);
	if (!window) { return DefWindowProcA(hwnd, message, wParam, lParam); }

	switch (message) {
		case WM_INPUT: {
			impl_process_message_raw(window, wParam, lParam);
		} return 0;

		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP: {
			impl_process_message_mouse(window, wParam, lParam, true, (vec2){0, 0});
		} return 0;

		case WM_MOUSEWHEEL: {
			impl_process_message_mouse(window, wParam, lParam, false, (vec2){0, 1});
		} return 0;
		
		case WM_MOUSEHWHEEL: {
			impl_process_message_mouse(window, wParam, lParam, false, (vec2){1, 0});
		} return 0;

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_KEYDOWN: {
			impl_process_message_keyboard(window, wParam, lParam);
		} return 0;

		case WM_KILLFOCUS: {
			memset(window->mouse.keys,    0, sizeof(window->mouse.keys));
			memset(window->keyboard.keys, 0, sizeof(window->keyboard.keys));
		} return 0;

		case WM_SIZE: {
			window->size = (svec2){LOWORD(lParam), HIWORD(lParam)};
			switch (wParam) {
				case SIZE_MINIMIZED: break;
				case SIZE_MAXIMIZED: break;
				case SIZE_RESTORED:  break;
			}
		} return 0;

		case WM_CLOSE: {
			// if (window->callbacks.close) {
			// 	(*window->callbacks.close)(window);
			// }
			window->handle = NULL;
			DestroyWindow(hwnd);
		} return 0;

		case WM_DESTROY: {
			RemovePropA(hwnd, ENGINE_WINDOW_POINTER);
			if (impl_window_raw_input_target == hwnd) { engine_window_toggle_raw_input(window); }
			if (window->rendering_context) { engine_rendering_context_destroy(window->rendering_context); }
			if (window->handle == hwnd) { ENGINE_FREE(window); }
		} return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

//
#undef ENGINE_WINDOW_POINTER
