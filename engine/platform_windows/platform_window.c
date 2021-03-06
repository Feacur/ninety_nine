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
#define ENGINE_MOUSE_KEYS 8
#define ENGINE_KEYBOARD_KEYS 256

static HWND impl_window_raw_input_target = NULL;

static void impl_keyboard_remap_ascii(struct Engine_Window * window);
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
		bool  keys[ENGINE_MOUSE_KEYS];
		bool  prev[ENGINE_MOUSE_KEYS];
	} mouse;

	struct {
		bool keys[ENGINE_KEYBOARD_KEYS];
		bool prev[ENGINE_KEYBOARD_KEYS];
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
	window->size = SVEC2(window_rect.right - window_rect.left, window_rect.bottom - window_rect.top);

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

void engine_window_update(struct Engine_Window * window) {
	impl_keyboard_remap_ascii(window);

	memcpy(window->mouse.prev,    window->mouse.keys,    sizeof(window->mouse.keys));
	memcpy(window->keyboard.prev, window->keyboard.keys, sizeof(window->keyboard.keys));
	window->mouse.delta = SVEC2(0, 0);
	window->mouse.wheel = VEC2(0, 0);

	engine_rendering_context_update(window->rendering_context);
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
	if (key >= ENGINE_MOUSE_KEYS) { return false; }
	return window->mouse.keys[key];
}

bool engine_window_mouse_transition(struct Engine_Window * window, u8 key, bool state) {
	if (key >= ENGINE_MOUSE_KEYS) { return false; }
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
	memset(window->mouse.keys,    0, sizeof(window->mouse.keys));
	memset(window->keyboard.keys, 0, sizeof(window->keyboard.keys));
	window->mouse.delta = SVEC2(0, 0);
	window->mouse.wheel = VEC2(0, 0);

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

#include "interoperations/system__window.h"

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

#include "interoperations/rendering_context__window.h"

HWND engine_rendering_context__window_get_handle(struct Engine_Window * window) {
	return window->handle;
}

void engine_rendering_context__window_detach(struct Engine_Window * window) {
	window->rendering_context = NULL;
}

//
// internal implementation
//

static void impl_keyboard_remap_ascii(struct Engine_Window * window) {
	bool * keys = window->keyboard.keys;
	//
	memcpy(keys + 'A', keys + 'a', (size_t)(1 + 'z' - 'a'));
	//
	char const symbols_src[] = "0123456789"    "\'"    ",-./;=["    "\\"    "]`";
	char const symbols_dst[] = ")!@#$%^&*("    "\""    "<_>?:+{"    "|"     "}~";
	u8 const symbols_number = sizeof(symbols_src) / sizeof(symbols_src[0]);
	for (u8 i = 0; i < symbols_number; ++i) {
		keys[(u8)symbols_dst[i]] = keys[(u8)symbols_src[i]];
	}
}

static void impl_keyboard_process_virtual_key(struct Engine_Window * window, USHORT key, bool is_down) {
	if ('A'        <= key && key <= 'Z')        { window->keyboard.keys['a'     + key - 'A']        = is_down; return; }
	if ('0'        <= key && key <= '9')        { window->keyboard.keys['0'     + key - '0']        = is_down; return; }
	if (VK_NUMPAD0 <= key && key <= VK_NUMPAD9) { window->keyboard.keys[KC_Num0 + key - VK_NUMPAD0] = is_down; return; }
	if (VK_F1      <= key && key <= VK_F24)     { window->keyboard.keys[KC_F1   + key - VK_F1]      = is_down; return; }
	//
	switch (key) {
		// common keyboard, ASCII control characters
		case VK_BACK:   window->keyboard.keys['\b'] = is_down; break;
		case VK_TAB:    window->keyboard.keys['\t'] = is_down; break;
		case VK_RETURN: window->keyboard.keys['\r'] = is_down; break;
		case VK_ESCAPE: window->keyboard.keys[0x1B] = is_down; break;
		// common keyboard, ASCII printable characters
		case VK_SPACE:      window->keyboard.keys[' ']  = is_down; break;
		case VK_OEM_7:      window->keyboard.keys['\''] = is_down; break;
		case VK_OEM_COMMA:  window->keyboard.keys[',']  = is_down; break;
		case VK_OEM_MINUS:  window->keyboard.keys['-']  = is_down; break;
		case VK_OEM_PERIOD: window->keyboard.keys['.']  = is_down; break;
		case VK_OEM_2:      window->keyboard.keys['/']  = is_down; break;
		case VK_OEM_1:      window->keyboard.keys[';']  = is_down; break;
		case VK_OEM_PLUS:   window->keyboard.keys['=']  = is_down; break;
		case VK_OEM_4:      window->keyboard.keys['[']  = is_down; break;
		case VK_OEM_5:      window->keyboard.keys['\\'] = is_down; break;
		case VK_OEM_6:      window->keyboard.keys[']']  = is_down; break;
		case VK_OEM_3:      window->keyboard.keys['`']  = is_down; break;
		case VK_DELETE:     window->keyboard.keys[0x7f] = is_down; break;
		// common keyboard, non-ASCII
		case VK_CAPITAL:  window->keyboard.keys[KC_CapsLock]    = is_down; break;
		case VK_SHIFT:    window->keyboard.keys[KC_Shift]       = is_down; break;
		case VK_CONTROL:  window->keyboard.keys[KC_Control]     = is_down; break;
		case VK_MENU:     window->keyboard.keys[KC_Alt]         = is_down; break;
		case VK_LEFT:     window->keyboard.keys[KC_ArrowLeft]   = is_down; break;
		case VK_RIGHT:    window->keyboard.keys[KC_ArrowRight]  = is_down; break;
		case VK_DOWN:     window->keyboard.keys[KC_ArrowDown]   = is_down; break;
		case VK_UP:       window->keyboard.keys[KC_ArrowUp]     = is_down; break;
		case VK_INSERT:   window->keyboard.keys[KC_Insert]      = is_down; break;
		case VK_SNAPSHOT: window->keyboard.keys[KC_PrintScreen] = is_down; break;
		case VK_PRIOR:    window->keyboard.keys[KC_PageUp]      = is_down; break;
		case VK_NEXT:     window->keyboard.keys[KC_PageDown]    = is_down; break;
		case VK_HOME:     window->keyboard.keys[KC_Home]        = is_down; break;
		case VK_END:      window->keyboard.keys[KC_End]         = is_down; break;
		// numeric keypad
		case VK_NUMLOCK:  window->keyboard.keys[KC_NumLock] = is_down; break;
		case VK_ADD:      window->keyboard.keys[KC_NumAdd]  = is_down; break;
		case VK_SUBTRACT: window->keyboard.keys[KC_NumSub]  = is_down; break;
		case VK_MULTIPLY: window->keyboard.keys[KC_NumMul]  = is_down; break;
		case VK_DIVIDE:   window->keyboard.keys[KC_NumDiv]  = is_down; break;
		case VK_DECIMAL:  window->keyboard.keys[KC_NumDot]  = is_down; break;
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
		window->mouse.display_position = SVEC2(position.x, position.y);
		window->mouse.display_position.y = height - (window->mouse.display_position.y + 1);
	
		ScreenToClient(window->handle, &position);
		window->mouse.window_position = SVEC2(position.x, position.y);
		window->mouse.window_position.y = window->size.y - (window->mouse.window_position.y + 1);

		svec2 delta = svec2_sub(window->mouse.display_position, previous_display_position);
		window->mouse.delta.x += delta.x;
		window->mouse.delta.y += delta.y;
	}
	else if (data->lLastX != 0 || data->lLastY != 0) {
		POINT position;
		GetCursorPos(&position);
		window->mouse.display_position = SVEC2(position.x, position.y);
		window->mouse.display_position.y = height - (window->mouse.display_position.y + 1);

		ScreenToClient(window->handle, &position);
		window->mouse.window_position = SVEC2(position.x, position.y);
		window->mouse.window_position.y  = window->size.y - (window->mouse.window_position.y + 1);

		svec2 delta = SVEC2(data->lLastX, -data->lLastY);
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
	static u32 const keys_down[ENGINE_MOUSE_KEYS] = {
		RI_MOUSE_BUTTON_1_DOWN,
		RI_MOUSE_BUTTON_2_DOWN,
		RI_MOUSE_BUTTON_3_DOWN,
		RI_MOUSE_BUTTON_4_DOWN,
		RI_MOUSE_BUTTON_5_DOWN,
	};

	static u32 const keys_up[ENGINE_MOUSE_KEYS] = {
		RI_MOUSE_BUTTON_1_UP,
		RI_MOUSE_BUTTON_2_UP,
		RI_MOUSE_BUTTON_3_UP,
		RI_MOUSE_BUTTON_4_UP,
		RI_MOUSE_BUTTON_5_UP,
	};

	for (u8 i = 0; i < ENGINE_MOUSE_KEYS; ++i) {
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

	window->mouse.display_position = SVEC2(screen.x, screen.y);
	window->mouse.display_position.y = height - (window->mouse.display_position.y + 1);

	window->mouse.window_position = SVEC2(client.x, client.y);
	window->mouse.window_position.y = window->size.y - (window->mouse.window_position.y + 1);

	svec2 delta = svec2_sub(window->mouse.display_position, previous_display_position);
	window->mouse.delta.x += delta.x;
	window->mouse.delta.y += delta.y;

	//
	window->mouse.wheel.x += wheel_mask.x * (GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
	window->mouse.wheel.y += wheel_mask.y * (GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);

	//
	static u32 const keys_down[ENGINE_MOUSE_KEYS] = {
		MK_LBUTTON,
		MK_MBUTTON,
		MK_RBUTTON,
		MK_XBUTTON1,
		MK_XBUTTON2,
	};

	WPARAM virtual_keys = wParam;
	for (u8 i = 0; i < ENGINE_MOUSE_KEYS; ++i) {
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
			impl_process_message_mouse(window, wParam, lParam, true, VEC2(0, 0));
		} return 0;

		case WM_MOUSEWHEEL: {
			impl_process_message_mouse(window, wParam, lParam, false, VEC2(0, 1));
		} return 0;
		
		case WM_MOUSEHWHEEL: {
			impl_process_message_mouse(window, wParam, lParam, false, VEC2(1, 0));
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
			window->size = SVEC2(LOWORD(lParam), HIWORD(lParam));
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
#undef ENGINE_MOUSE_KEYS
#undef ENGINE_KEYBOARD_KEYS
