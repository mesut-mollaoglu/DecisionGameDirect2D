#pragma once

#include "Graphics.h"

class GameController {
protected:
	Graphics* gfx;
public:
	RECT yesRect; RECT noRect; bool gameEnded = false;
	ID2D1Bitmap* background; ID2D1Bitmap* yesButton;
	ID2D1Bitmap* noButton; ID2D1Bitmap* healthIcon; ID2D1Bitmap* energyIcon;
	ID2D1Bitmap* resourcesIcon;	D2D1_RECT_F statsRect;
	std::vector<std::wstring> lines;
	std::vector<std::pair<int, int>> energy;
	std::vector<std::pair<int, int>> resources;
	std::vector<std::pair<int, int>> health;
	int index = 0, energyInt = 10, resourcesInt = 10, healthInt = 10, random = 0;
	std::pair<std::wstring, std::wstring> gameEnd = std::make_pair(
		L"Congrats! You did an amazing job!", 
		L"Despite all your sacrifices, you didn't manage to make it to the end...");
	std::wstring buffer;
	std::wfstream file;
	std::wstring prompt;
	D2D1_RECT_F yButtonPos;
	D2D1_RECT_F nButtonPos;
	IWICImagingFactory* factory;
	using Clock = std::chrono::steady_clock;
	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> now;
	std::chrono::milliseconds duration;
	bool bInit = false;
	void Init(Graphics* graphics) {
		gfx = graphics;
	}
	void LoadNewPrompt();
	ID2D1Bitmap* LoadSprite(
		PCWSTR uri,
		UINT destinationWidth,
		UINT destinationHeight,
		ID2D1Bitmap* ppBitmap);
	void Load(Graphics* graphics);
	void Unload();
	void Update(HWND windowHandle);
	void Render(Graphics* gfx);
};