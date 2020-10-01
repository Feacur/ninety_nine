#include "engine/api/code.h"
#include "engine/api/math_types.h"
#include "ogl_context.h"

#include <Windows.h>
#include <hidusage.h>

//
#define ENGINE_WINDOW_CLASS_NAME "engine_window_class_name"
#define ENGINE_WINDOW_POINTER "engine_window_pointer"

static HWND impl_window_raw_input_target = NULL;

static LRESULT CALLBACK impl_window_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//
// API
//

#include "engine/api/platform_window.h"

struct Engine_Window {
	HWND hwnd;
	svec2 size;
	struct {
		svec2 delta;
		svec2 display_position;
		svec2 window_position;
	} mouse;
	struct Rendering_Context_OGL * rendering_context;
};

struct Engine_Window * engine_window_create(void) {
	struct Engine_Window * window = ENGINE_MALLOC(sizeof(*window));

	window->hwnd = CreateWindowExA(
		WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR,
		ENGINE_WINDOW_CLASS_NAME, "ninety nine",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		// int X, Y, nWidth, nHeight
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		HWND_DESKTOP, NULL, GetModuleHandle(NULL), NULL
	);
	SetPropA(window->hwnd, ENGINE_WINDOW_POINTER, window);

	RECT window_rect;
	GetClientRect(window->hwnd, &window_rect);
	window->size = (svec2){window_rect.right - window_rect.left, window_rect.bottom - window_rect.top};

	return window;
}

void engine_window_destroy(struct Engine_Window * window) {
	if (window->hwnd) { DestroyWindow(window->hwnd); return; }
	ENGINE_FREE(window);
}

bool engine_window_is_active(struct Engine_Window * window) {
	return window->hwnd;
}

void engine_window_init_context(struct Engine_Window * window) {
	window->rendering_context = engine_ogl_context_create(window);
}

void engine_window_toggle_raw_input(struct Engine_Window * window) {
	// https://docs.microsoft.com/windows-hardware/drivers/hid/hid-usages
	HWND hwnd_target; USHORT flags;
	if (!impl_window_raw_input_target) {
		hwnd_target = window->hwnd; flags = 0;
	}
	else if (impl_window_raw_input_target == window->hwnd) {
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
	HWND const hwnd = window->hwnd;
	
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

//
// system API
//

#include "window_system.h"

void engine_system_register_window_class(void) {
	RegisterClassExA(&(WNDCLASSEXA){
		.cbSize        = sizeof(WNDCLASSEXA),
		.lpszClassName = ENGINE_WINDOW_CLASS_NAME,
		.hInstance     = GetModuleHandleA(NULL),
		.lpfnWndProc   = impl_window_procedure,
		.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
		.hCursor       = LoadCursorA(0, IDC_ARROW),
	});
}

void engine_system_unregister_window_class(void) {
	UnregisterClassA(ENGINE_WINDOW_CLASS_NAME, GetModuleHandleA(NULL));
}

//
// context API
//

#include "window_context.h"

HDC engine_window_get_hdc(struct Engine_Window * window) {
	return GetDC(window->hwnd);
}

//
// internal implementation
//

static s32 mul_div(s32 value, s32 numerator, s32 denominator) {
	s32 a = value / denominator;
	s32 b = value % denominator;
	return a * numerator + b * numerator / denominator;
}

static void raw_input_callback_mouse(struct Engine_Window * window, RAWMOUSE * data) {
	// https://docs.microsoft.com/windows/win32/api/winuser/ns-winuser-rawmouse
	bool const is_virtual_desktop = (data->usFlags & MOUSE_VIRTUAL_DESKTOP) == MOUSE_VIRTUAL_DESKTOP;
	int const height = GetSystemMetrics(is_virtual_desktop ? SM_CYVIRTUALSCREEN : SM_CYSCREEN);
	int const width  = GetSystemMetrics(is_virtual_desktop ? SM_CXVIRTUALSCREEN : SM_CXSCREEN);

	if ((data->usFlags & MOUSE_MOVE_ABSOLUTE) == MOUSE_MOVE_ABSOLUTE) {
		svec2 const previous_display_position = window->mouse.display_position;

		POINT position = (POINT){
			.x = mul_div(data->lLastX, width,  UINT16_MAX),
			.y = mul_div(data->lLastY, height, UINT16_MAX),
		};
		window->mouse.display_position = (svec2){position.x, position.y};
	
		ScreenToClient(window->hwnd, &position);
		window->mouse.window_position = (svec2){position.x, position.y};

		window->mouse.delta = (svec2){
			window->mouse.display_position.x - previous_display_position.x,
			window->mouse.display_position.y - previous_display_position.y,
		};
	}
	else {
		POINT position;
		GetCursorPos(&position);
		window->mouse.display_position = (svec2){position.x, position.y};

		ScreenToClient(window->hwnd, &position);
		window->mouse.window_position = (svec2){position.x, position.y};

		window->mouse.delta = (svec2){data->lLastX, data->lLastY};
	}

	window->mouse.display_position.y = height - (window->mouse.display_position.y + 1);
	window->mouse.window_position.y  = window->size.y - (window->mouse.window_position.y + 1);
	window->mouse.delta.y            = -window->mouse.delta.y;
}

static void raw_input_callback_keyboard(struct Engine_Window * window, RAWKEYBOARD * data) {
	// https://docs.microsoft.com/windows/win32/api/winuser/ns-winuser-rawkeyboard
	(void)window; (void)data;
	
}

static void raw_input_callback_hid(struct Engine_Window * window, RAWHID * data) {
	// https://docs.microsoft.com/windows/win32/api/winuser/ns-winuser-rawhid
	(void)window; (void)data;
	
}

static void impl_process_message_raw(struct Engine_Window * window, WPARAM wParam, LPARAM lParam) {
	// https://docs.microsoft.com/windows/win32/inputdev/raw-input
	(void)wParam; (void)lParam;
	if (impl_window_raw_input_target != window->hwnd) { return; }

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

static void impl_process_message_mouse(struct Engine_Window * window, WPARAM wParam, LPARAM lParam, bool client_space, svec2 scale) {
	// https://docs.microsoft.com/windows/win32/inputdev/mouse-input
	(void)wParam; (void)lParam; (void)client_space; (void)scale;
	if (impl_window_raw_input_target == window->hwnd) { return; }
}

static void impl_process_message_keyboard(struct Engine_Window * window, WPARAM wParam, LPARAM lParam) {
	// https://docs.microsoft.com/windows/win32/inputdev/keyboard-input
	(void)wParam; (void)lParam;
	if (impl_window_raw_input_target == window->hwnd) { return; }
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
			impl_process_message_mouse(window, wParam, lParam, true, (svec2){0, 0});
		} return 0;

		case WM_MOUSEWHEEL: {
			impl_process_message_mouse(window, wParam, lParam, false, (svec2){0, 1});
		} return 0;
		
		case WM_MOUSEHWHEEL: {
			impl_process_message_mouse(window, wParam, lParam, false, (svec2){1, 0});
		} return 0;

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_KEYDOWN: {
			impl_process_message_keyboard(window, wParam, lParam);
		} return 0;

		case WM_KILLFOCUS: {
			// keyboard_reset(window);
			// mouse_reset(window);
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
			window->hwnd = NULL;
			DestroyWindow(hwnd);
		} return 0;

		case WM_DESTROY: {
			RemovePropA(hwnd, ENGINE_WINDOW_POINTER);
			if (impl_window_raw_input_target == hwnd) { engine_window_toggle_raw_input(window); }
			if (window->rendering_context) { engine_ogl_context_destroy(window->rendering_context); }
			if (window->hwnd == hwnd) { ENGINE_FREE(window); }
		} return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

//
#undef ENGINE_WINDOW_CLASS_NAME
#undef ENGINE_WINDOW_POINTER
