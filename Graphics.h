#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include <d2d1_1.h>
#include <random>
#include <dwrite_3.h>
#include <d2d1helper.h>
#include <tchar.h>
#include <wincodec.h>
#include <fstream>
#include <string>
#include <stdio.h>
#include <chrono>
#include <sstream>
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS
#undef max

class GameController;

class Graphics {
public:
	bool InitGraphics(HWND hwnd);
	Graphics();
	~Graphics();
 	ID2D1RenderTarget* GetRenderTarget() {
		return renderTarget;
	}
	ID2D1Factory1* GetFactory() {
		return factory;
	}	
	IDWriteFactory5* getWriteFactory() {
		return dWriteFactory;
	}
	HRESULT InitWritingFactory();
	ID2D1SolidColorBrush* blackColor;
	ID2D1SolidColorBrush* whiteColor;
	void DrawTextF(std::wstring text, float x, float y, float width, float height, ID2D1Brush* color);
private:
	ID2D1Factory1* factory;
	IDWriteFactory5* dWriteFactory;
	IDWriteTextFormat* textFormat;
	IDWriteTextLayout* textLayout;
	ID2D1HwndRenderTarget* renderTarget;
};