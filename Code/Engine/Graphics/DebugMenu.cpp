#include "DebugMenu.h"
#include "GameSprite.h"
//#ifdef DEBUG_MENU	// This wraps the whole file

// You may end up using these:
class Slider;
class CheckBox;
class Text;
class Button;
IDirect3DDevice9* s_direct3dDeviceMenu;

DebugMenu::DebugMenu()
{

	menuOffset = 0.0f;
	selectedMenu = 0;
	checkBoxChecked = false;
	sliderSelected = false;
	buttonSelected = false;
	sliderCurrentPosition = 0;
	sliderLastPosition = 0;
	isMenuActive = false;
}

DebugMenu::~DebugMenu()
{
	// ..
}

void Text::showText(float value, int redValue)
{
	 D3DXCreateFont(s_direct3dDeviceMenu, 25, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, FF_DONTCARE, "Arial", &font);

	SetRect(&fRectangle, 0, 0, 600, 300);
	textMessage = "FPS :  ";
	textMessage = textMessage + std::to_string(value);

	
	if (font)
	{
		font->DrawTextA(NULL, textMessage.c_str(), -1, &fRectangle, DT_LEFT, D3DCOLOR_XRGB(redValue, 0, 0));
	}

	if (font) {
		font->Release();
		font = NULL;
	}
}


void Slider::showSlider(int lastPosition, int newPosition, int redValue)
{
	D3DXCreateFont(s_direct3dDeviceMenu, 25, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, FF_DONTCARE, "Arial", &font);

	SetRect(&fRectangle, 0, 150, 600, 300);
	textMessage = "Slider :  ";
	if (newPosition >= 19)
		newPosition = 19;
	if (newPosition <= 0)
		newPosition = 0;

	if (lastPosition >= 19)
		lastPosition = 19;
	if (lastPosition <= 0)
		lastPosition = 0;
	sliderMsg.replace(sliderMsg.begin() + lastPosition, sliderMsg.begin() + lastPosition + 1, "-");
	sliderMsg.replace(sliderMsg.begin() + newPosition , sliderMsg.begin() + newPosition + 1, "*");
	textMessage = textMessage + sliderMsg;

	if (font)
	{
		font->DrawTextA(NULL, textMessage.c_str(), -1, &fRectangle, DT_LEFT, D3DCOLOR_XRGB(redValue, 0, 0));
	}

	if (font) {
		font->Release();
		font = NULL;
	}
}

void Button::showButton(int redValue)
{
	 D3DXCreateFont(s_direct3dDeviceMenu, 25, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, FF_DONTCARE, "Arial", &font);
	/*	if (!SUCCEEDED(hr))
	return false;*/
	//SetRect(&fRectangle, 0, 0, 600, 300);
	SetRect(&fRectangle, 0, 50, 600, 300);

	textMessage = "Reset Sphere ";
	GameSprite *currentSprite;
	currentSprite = new GameSprite(150, 50);

	currentSprite->file = "data/button.png";
	currentSprite->Initialize(s_direct3dDeviceMenu, currentSprite->file.c_str(),
		currentSprite->width, currentSprite->height);
	if (currentSprite && currentSprite->isInitialized())
		currentSprite->Draw(0.0f);
	if (font)
	{
		font->DrawTextA(NULL, textMessage.c_str(), -1, &fRectangle, DT_LEFT, D3DCOLOR_XRGB(redValue, 0, 0));
	}
	if (font) {
		font->Release();
		font = NULL;
	}
	delete currentSprite;
}

void CheckBox::showCheckBox(int redValue, bool enabled)
{
	D3DXCreateFont(s_direct3dDeviceMenu, 25, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, FF_DONTCARE, "Arial", &font);
	/*	if (!SUCCEEDED(hr))
	return false;*/
	//SetRect(&fRectangle, 0, 0, 600, 300);
	SetRect(&fRectangle, 0, 100, 600, 300);
	textMessage = "Enable Sphere : ";
	GameSprite *currentSprite;
	currentSprite = new GameSprite(180, 100);
	if(!enabled)
		currentSprite->file = "data/unchecked_checkbox.png";
	else
		currentSprite->file = "data/checked_checkbox.png";
	currentSprite->Initialize(s_direct3dDeviceMenu, currentSprite->file.c_str(),
		currentSprite->width, currentSprite->height);
	if (currentSprite && currentSprite->isInitialized())
		currentSprite->Draw(0.0f);
	if (font)
	{
		font->DrawTextA(NULL, textMessage.c_str(), -1, &fRectangle, DT_LEFT, D3DCOLOR_XRGB(redValue, 0, 0));
	}
	if (font) {
		font->Release();
		font = NULL;
	}
	delete currentSprite;

}



void DebugMenu::Activate(bool active, IDirect3DDevice9* direct3dDevice)
{

	s_direct3dDeviceMenu = direct3dDevice;
	isMenuActive = true;
	switch ( (int) selectedMenu)
	{
	case 0:
		textWidget.showText(menuOffset, 255);
		buttonWidget.showButton(0);
		checkBoxWidget.showCheckBox(0,checkBoxChecked);
		sliderWidget.showSlider(0,0,0);

		break;
	case 1:
		textWidget.showText(menuOffset, 0);
		buttonWidget.showButton(255);
		checkBoxWidget.showCheckBox(0, checkBoxChecked);
		sliderWidget.showSlider(0, 0,0);

		break;

	case 2:
		textWidget.showText(menuOffset, 0);
		buttonWidget.showButton(0);
		checkBoxWidget.showCheckBox(255, checkBoxChecked);
		sliderWidget.showSlider(0, 0,0);

		break;

	case 3:
		textWidget.showText(menuOffset, 0);
		buttonWidget.showButton(0);
		checkBoxWidget.showCheckBox(0, checkBoxChecked);
		sliderWidget.showSlider(sliderLastPosition, sliderCurrentPosition, 255);
		sliderLastPosition = sliderCurrentPosition;
		break;

	default:
		break;
	}
	

}


void DebugMenu::Update()
{
	// You may need to continue doing work here every frame, even when inactive.
	// Try to avoid that if possible.
	//if(!isMenuActive)dd
	/*textWidget.showText(menuOffset);
	buttonWidget.showButton();
	checkBoxWidget.showCheckBox();
	isMenuActive = true;*/

	
	
}


void DebugMenu::CreateSlider(const char *name, float* value, float min, float max)
{

	// ...
}


void DebugMenu::CreateCheckBox(const char *name, bool* value)
{
	// ...
}


void DebugMenu::CreateText(const char *name, char* value)
{
	 //textWidget = Text();
	// ...
}


void DebugMenu::CreateButton(const char *name, void(*callback)(void*), void *param)
{
	// ...
}


//#endif	// DEBUG_MENU