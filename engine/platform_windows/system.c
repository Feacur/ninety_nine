#include "engine/api/code.h"
#include "window_system.h"
#include "ogl_system.h"

#include <signal.h>
#include <Windows.h>

//
static void impl_signal_handler(int value);

//
// API
//

#include "engine/api/system.h"

bool engine_system_should_close;

void engine_system_init(void) {
	engine_system_register_window_class();
	engine_system_init_opengl();

	signal(SIGABRT, impl_signal_handler);
	signal(SIGFPE,  impl_signal_handler);
	signal(SIGILL,  impl_signal_handler);
	signal(SIGINT,  impl_signal_handler);
	signal(SIGSEGV, impl_signal_handler);
	signal(SIGTERM, impl_signal_handler);
}

void engine_system_deinit(void) {
	engine_system_unregister_window_class();
	engine_system_deinit_opengl();
}

void engine_system_poll_events(void) {
	MSG message;
	while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
		if (message.message == WM_QUIT) { engine_system_should_close = true; continue; }
		TranslateMessage(&message);
		DispatchMessageA(&message);
	}
}

//
// internal implementation
//

static void impl_signal_handler(int value) {
	// http://www.cplusplus.com/reference/csignal/signal/
	switch (value) {
		case SIGABRT: ENGINE_DEBUG_BREAK(); break; // Abnormal termination, such as is initiated by the abort function.
		case SIGFPE:  ENGINE_DEBUG_BREAK(); break; // Erroneous arithmetic operation, such as zero divide or an operation resulting in overflow (not necessarily with a floating-point operation).
		case SIGILL:  ENGINE_DEBUG_BREAK(); break; // Invalid function image, such as an illegal instruction. This is generally due to a corruption in the code or to an attempt to execute data.
		case SIGINT:  ENGINE_DEBUG_BREAK(); break; // Interactive attention signal. Generally generated by the application user.
		case SIGSEGV: ENGINE_DEBUG_BREAK(); break; // Invalid access to storage: When a program tries to read or write outside the memory it has allocated.
		case SIGTERM: ENGINE_DEBUG_BREAK(); break; // Termination request sent to program.
		default:      ENGINE_DEBUG_BREAK(); break; // ?
	}
	engine_system_should_close = true;
}

// https://docs.microsoft.com/en-us/cpp/c-runtime-library/argc-argv-wargv
// https://docs.microsoft.com/en-us/windows/win32/desktop-programming
// https://docs.microsoft.com/en-us/windows/win32/dlls/dllmain
// https://docs.microsoft.com/en-us/windows/win32/learnwin32/winmain--the-application-entry-point
// https://docs.microsoft.com/en-us/cpp/build/reference/subsystem-specify-subsystem

extern int main(int argc, char * argv[]);
int WINAPI WinMain(
	HINSTANCE hInstance,     // is something called a "handle to an instance" or "handle to a module." The operating system uses this value to identify the executable (EXE) when it is loaded in memory. The instance handle is needed for certain Windows functions—for example, to load icons or bitmaps.
	HINSTANCE hPrevInstance, // has no meaning. It was used in 16-bit Windows, but is now always zero.
	PSTR      pCmdLine,      // contains the command-line arguments as an ANSI string.
	int       nCmdShow       // is a flag that says whether the main application window will be minimized, maximized, or shown normally.
) {
	(void)hInstance; (void)hPrevInstance; (void)pCmdLine; (void)nCmdShow;
	return main(__argc, __argv);
}

//
// graphics cards supplements
//

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#endif

// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001UL;

// https://community.amd.com/thread/169965
// https://community.amd.com/thread/223376
// https://gpuopen.com/amdpowerxpressrequesthighperformance/
__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001UL;

#if defined(__clang__)
#pragma clang diagnostic pop
#endif
