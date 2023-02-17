#include "Graphics.h"

Graphics::Graphics() {
	this->textFormat = NULL;
	this->dWriteFactory = NULL;
	this->factory = NULL;
	this->renderTarget = NULL;
	this->textLayout = NULL;
}

Graphics::~Graphics() {
	if (this->factory) this->factory->Release();
	if (this->renderTarget) this->renderTarget->Release();
	if (this->dWriteFactory) this->dWriteFactory->Release();
	if (this->textFormat) this->textFormat->Release();
}

HRESULT Graphics::InitWritingFactory() {
	HRESULT hr;
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory5),
		reinterpret_cast<IUnknown**>(&dWriteFactory));
	if(SUCCEEDED(hr)) {hr = this->dWriteFactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 21.0f, L"en-us", &textFormat);}
	if (SUCCEEDED(hr)) {this->textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);}
	return hr;
}

void Graphics::DrawTextF(std::wstring text, float x, float y, float width, float height, ID2D1Brush* color) {
	HRESULT hr;
	if (SUCCEEDED(this->InitWritingFactory())) {
		hr = this->dWriteFactory->CreateTextLayout(text.c_str(), (UINT)wcslen(text.c_str()), this->textFormat,
			width, height, &textLayout);
	}
	if (SUCCEEDED(hr)) {
		this->renderTarget->DrawTextLayout(D2D1::Point2F(x, y), textLayout, color, D2D1_DRAW_TEXT_OPTIONS_NONE);
	}
	this->textLayout->Release();
}

bool Graphics::InitGraphics(HWND hwnd) {
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
	if (hr != S_OK) return false;
	RECT rect;
	GetClientRect(hwnd, &rect);
	hr = factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rect.right, rect.bottom)), &renderTarget);
	if (hr != S_OK) return false;
	renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f)), &blackColor);
	renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &whiteColor);
	if (hr != S_OK) return false;
	return true;
}