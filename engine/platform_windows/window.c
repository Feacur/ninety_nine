#include "engine/api/code.h"
#include "engine/api/types.h"
#include "engine/api/window.h"
#include "window_internal.h"

#include <Windows.h>

#define ENGINE_WINDOW_CLASS_NAME "engine_window_class_name"
#define ENGINE_WINDOW_POINTER "engine_window_pointer"

struct Engine_Window {
	HWND hwnd;
	bool skip_free_on_destroy;
};

struct Engine_Window * engine_window_create(void) {
	struct Engine_Window * window = ENGINE_MALLOC(sizeof(*window));

	window->hwnd = CreateWindowExA(
		WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR,
		ENGINE_WINDOW_CLASS_NAME, "",
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

//
//
//

/*
static void impl_toggle_borderless_fullsreen(HWND hwnd) {
	static WINDOWPLACEMENT normal_window_position;
	
	DWORD window_style = GetWindowLong(hwnd, GWL_STYLE);
	if ((window_style & WS_OVERLAPPEDWINDOW) == WS_OVERLAPPEDWINDOW) {
		if (!GetWindowPlacement(hwnd, &normal_window_position)) { return; }

		MONITORINFO monitor_info;
		memset(&monitor_info, 0, sizeof(monitor_info));
		monitor_info.cbSize = sizeof(MONITORINFO);
		if (!GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &monitor_info)) { return; }

		SetWindowLong(hwnd, GWL_STYLE, window_style & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(
			hwnd, HWND_TOP,
			monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
			monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
			monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED
		);
		return;
	}

	// Restore windowed mode
	SetWindowLong(hwnd, GWL_STYLE, window_style | WS_OVERLAPPEDWINDOW);
	SetWindowPlacement(hwnd, &normal_window_position);
	SetWindowPos(
		hwnd, 0,
		0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED
	);
}
*/

static void engine_window_clean(struct Engine_Window * window) {
	if (window->hwnd) {
		RemovePropA(window->hwnd, ENGINE_WINDOW_POINTER);
		window->hwnd = NULL;
	}
}

static LRESULT CALLBACK impl_window_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	struct Engine_Window * window = GetPropA(hwnd, ENGINE_WINDOW_POINTER);
	if (!window) { return DefWindowProc(hwnd, message, wParam, lParam); }

	switch (message) {
		case WM_CLOSE: {
			// if (window->callbacks.close) {
			// 	(*window->callbacks.close)(window);
			// }
			// else {
				window->skip_free_on_destroy = true;
				DestroyWindow(hwnd);
			// }
			return 0;
		} break;

		case WM_DESTROY: {
			engine_window_clean(window);
			if (!window->skip_free_on_destroy) {
				ENGINE_FREE(window);
			}
			return 0;
		} break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

void engine_window_internal_register_class(void) {
	WNDCLASSEXA window_class;
	memset(&window_class, 0, sizeof(window_class));
	window_class.cbSize        = sizeof(window_class);
	window_class.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc   = impl_window_procedure;
	window_class.hCursor       = LoadCursorA(0, IDC_ARROW);
	window_class.lpszClassName = ENGINE_WINDOW_CLASS_NAME;
	RegisterClassExA(&window_class);
}

void engine_window_internal_unregister_class(void) {
	UnregisterClassA(ENGINE_WINDOW_CLASS_NAME, GetModuleHandleA(NULL));
}
