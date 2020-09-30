#include "engine/api/code.h"
#include "engine/api/math_types.h"
#include "ogl_context.h"

#include <Windows.h>

//
#define ENGINE_WINDOW_CLASS_NAME "engine_window_class_name"
#define ENGINE_WINDOW_POINTER "engine_window_pointer"

static LRESULT CALLBACK impl_window_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
static void impl_toggle_borderless_fullsreen(HWND hwnd);

//
// API
//

#include "engine/api/platform_window.h"

struct Engine_Window {
	HWND hwnd;
	svec2 size;
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

void engine_window_toggle_borderless_fullsreen(struct Engine_Window * window) {
	impl_toggle_borderless_fullsreen(window->hwnd);
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

static void impl_process_message_raw(struct Engine_Window * window, WPARAM wParam, LPARAM lParam) {
	(void)window; (void)wParam; (void)lParam;
	// https://docs.microsoft.com/en-us/windows/win32/inputdev/raw-input
}

static void impl_process_message_keyboard(struct Engine_Window * window, WPARAM wParam, LPARAM lParam) {
	(void)window; (void)wParam; (void)lParam;
	// https://docs.microsoft.com/en-us/windows/win32/inputdev/keyboard-input
}

static void impl_process_message_mouse(struct Engine_Window * window, WPARAM wParam, LPARAM lParam, bool client_space, svec2 scale) {
	(void)window; (void)wParam; (void)lParam; (void)client_space; (void)scale;
	// https://docs.microsoft.com/en-us/windows/win32/inputdev/mouse-input
}

static void impl_toggle_borderless_fullsreen(HWND hwnd) {
	static WINDOWPLACEMENT window_placement;
	
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

static LRESULT CALLBACK impl_window_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	struct Engine_Window * window = GetPropA(hwnd, ENGINE_WINDOW_POINTER);
	if (!window) { return DefWindowProcA(hwnd, message, wParam, lParam); }

	switch (message) {
		case WM_INPUT: {
			impl_process_message_raw(window, wParam, lParam);
		} return 0;

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_KEYDOWN: {
			impl_process_message_keyboard(window, wParam, lParam);
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
			if (window->rendering_context) { engine_ogl_context_destroy(window->rendering_context); }
			if (window->hwnd == hwnd) { ENGINE_FREE(window); }
		} return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

//
#undef ENGINE_WINDOW_CLASS_NAME
#undef ENGINE_WINDOW_POINTER
