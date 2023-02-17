#include "GameController.h"

ID2D1Bitmap* GameController::LoadSprite(
    PCWSTR uri,
    UINT destinationWidth,
    UINT destinationHeight,
    ID2D1Bitmap* ppBitmap)
{
    this->factory = NULL;
    IWICBitmapDecoder* pDecoder = NULL;
    IWICBitmapFrameDecode* pSource = NULL;
    IWICStream* pStream = NULL;
    IWICFormatConverter* pConverter = NULL;
    IWICBitmapScaler* pScaler = NULL;

    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        _uuidof(IWICImagingFactory),
        (LPVOID*)&factory);

    if(SUCCEEDED(hr)){   
        hr = this->factory->CreateDecoderFromFilename(
        uri,
        NULL,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &pDecoder
        );
    }
    if (SUCCEEDED(hr))
    {
        hr = pDecoder->GetFrame(0, &pSource);
    }
    if (SUCCEEDED(hr))
    {
        hr = this->factory->CreateFormatConverter(&pConverter);
    }
    if (SUCCEEDED(hr))
    {
        hr = pConverter->Initialize(
            pSource,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            NULL,
            0.0f,
            WICBitmapPaletteTypeMedianCut
        );
    }

    if (SUCCEEDED(hr)) {
        hr = gfx->GetRenderTarget()->CreateBitmapFromWicBitmap(pConverter, NULL, &ppBitmap);
    }
    if (pDecoder) pDecoder->Release();
    if (pSource) pSource->Release();
    if (pStream) pStream->Release();
    if (pConverter) pConverter->Release();
    if (pScaler) pScaler->Release();
    return ppBitmap;
    ppBitmap = NULL;
}

void GameController::LoadNewPrompt() {
    index = 0;
    this->prompt.clear();
    std::random_device rd;
    std::mt19937 gen(rd());
    if (!lines.empty()) {
        std::uniform_int_distribution<> dis(0, lines.size() - 1);
        index = dis(gen);
        for (auto c : lines[index]) {
            if (isupper(c)) { this->prompt += L' '; }
            this->prompt += c;
        }
        lines.erase(lines.begin() + index);
    }
}

void GameController::Load(Graphics* graphics) {
    this->background = this->LoadSprite(L"bin\\GameBackground.png",
        (UINT)graphics->GetRenderTarget()->GetSize().width,
        (UINT)graphics->GetRenderTarget()->GetSize().height, this->background);
    this->yesButton = this->LoadSprite(L"bin\\yesButton.png", (UINT)120.0f,(UINT)60.0f, this->yesButton);
    this->noButton = this->LoadSprite(L"bin\\noButton.png", (UINT)120.0f, (UINT)60.0f, this->noButton);
    this->energyIcon = this->LoadSprite(L"bin\\energyIcon.png", (UINT)45, (UINT)45, this->energyIcon);
    this->resourcesIcon = this->LoadSprite(L"bin\\resources1.png", (UINT)90, (UINT)90, this->resourcesIcon);
    this->healthIcon = this->LoadSprite(L"bin\\healthIcon.png", (UINT)45, (UINT)45, this->healthIcon);
    yButtonPos = D2D1::RectF(15.0f, 530.0f, 15.0f+this->yesButton->GetSize().width, 530.0f+this->yesButton->GetSize().height);
    nButtonPos = D2D1::RectF(465.0f, 530.0f, 465.0f + this->noButton->GetSize().width, 530.0f + this->noButton->GetSize().height);
    SetRect(&yesRect, yButtonPos.left, yButtonPos.top, yButtonPos.right, yButtonPos.bottom);
    SetRect(&noRect, nButtonPos.left, nButtonPos.top, nButtonPos.right, nButtonPos.bottom);
    this->file.open("bin\\DataBase.csv");
    while (std::getline(this->file, this->buffer)) {
        std::wstringstream line(this->buffer);
        std::wstring str;
        int energyYes, energyNo, healthYes, healthNo, resourcesYes, resourcesNo;
        line >> str >> energyYes >> energyNo >> healthYes >> healthNo >> resourcesYes >> resourcesNo;
        lines.push_back(str);
        energy.push_back(std::make_pair(energyYes, energyNo));
        health.push_back(std::make_pair(healthYes, healthNo));
        resources.push_back(std::make_pair(resourcesYes, resourcesNo));
    }
    LoadNewPrompt();
}

void GameController::Unload() {
    if (this->background) this->background->Release();
    if (this->yesButton) this->yesButton->Release();
    if (this->noButton) this->noButton->Release();
    this->file.close();
}

void GameController::Update(HWND windowHandle) {
    if (resourcesInt <= 0 || energyInt <= 0 || healthInt <= 0) { this->prompt = gameEnd.second; gameEnded = true; }
    if ((resourcesInt > 0 || energyInt > 0 || healthInt > 0) && lines.empty()) { this->prompt = gameEnd.first; gameEnded = true; }
    POINT pt; 
    GetCursorPos(&pt);
    ScreenToClient(windowHandle, &pt);
    if (!bInit)
    {
        start = now = Clock::now();
        bInit = true;
    }
    now = Clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    if (duration.count() > 150)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 2);
        random = dis(gen);
        start = Clock::now();
        if (PtInRect(&yesRect, pt) && GetAsyncKeyState(VK_LBUTTON) && !gameEnded) {
            LoadNewPrompt(); healthInt += health[index].first * random;
            energyInt += energy[index].first * random; resourcesInt += resources[index].first * random;
        }
        else if (PtInRect(&noRect, pt) && GetAsyncKeyState(VK_LBUTTON) && !gameEnded) {
            random = rand() % 4;
            LoadNewPrompt(); healthInt += health[index].second * random;
            energyInt += energy[index].second * random; resourcesInt += resources[index].second * random;
        }
    }
}

void GameController::Render(Graphics* gfx) {
	gfx->GetRenderTarget()->BeginDraw();
    gfx->GetRenderTarget()->Clear(D2D1::ColorF(D2D1::ColorF::Black));
    gfx->GetRenderTarget()->DrawBitmap(background,
        D2D1::RectF(0, 0, background->GetSize().width, background->GetSize().height - 80));
    gfx->GetRenderTarget()->DrawBitmap(yesButton, yButtonPos);
    gfx->GetRenderTarget()->DrawBitmap(noButton, nButtonPos);
    gfx->DrawTextF(prompt, gfx->GetRenderTarget()->GetSize().width / 2 - 300, 0, 600, 20, gfx->whiteColor);
    gfx->DrawTextF(std::to_wstring(healthInt).c_str(), 80.0f, 565.0f, 245.0f, 595.0f, gfx->whiteColor);
    gfx->GetRenderTarget()->DrawBitmap(this->energyIcon, D2D1::RectF(180.5f, 520.0f, 222.5f, 565.0f));
    gfx->DrawTextF(std::to_wstring(resourcesInt).c_str(), 130.0f, 565.0f, 325.0f, 595.0f, gfx->whiteColor);
    gfx->GetRenderTarget()->DrawBitmap(this->resourcesIcon, D2D1::RectF(270.0f, 520.0f, 315.0f, 565.0f));
    gfx->DrawTextF(std::to_wstring(energyInt).c_str(), 180.0f, 565.0f, 405.0f, 595.0f, gfx->whiteColor);
    gfx->GetRenderTarget()->DrawBitmap(this->healthIcon, D2D1::RectF(358.5f, 520.0f, 407.5f, 565.0f));
    gfx->GetRenderTarget()->EndDraw();
}