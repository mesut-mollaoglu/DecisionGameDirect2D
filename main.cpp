#include "Graphics.h"
#include "GameController.h"

GameController* controller;
Graphics* graphics;
HWND windowHandle = nullptr;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_DESTROY) { PostQuitMessage(0); controller->Unload(); return 0; }
	DefWindowProc(hwnd, message, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"ClassName";
	wc.lpszMenuName = nullptr;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&wc);
	RECT rect = { 0, 0, 600, 600};
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);
	windowHandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"ClassName", L"Astronaut Game", WS_OVERLAPPEDWINDOW, 100, 100, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);
	graphics = new Graphics();
	controller = new GameController();
	if (!graphics->InitGraphics(windowHandle)) {
		delete graphics;
		return -1;
	}
	controller->Init(graphics);
	ShowWindow(windowHandle, nCmdShow);
	controller->Load(graphics);
	MSG message = { 0 };
	message.message = WM_NULL;
	while (message.message != WM_QUIT) {
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else {
			controller->Render(graphics);
			controller->Update(windowHandle);
		}
	}
	return 0;
}