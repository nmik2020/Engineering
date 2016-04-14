#pragma once
#include <d3dx9.h>
#include <string>
//
//#ifdef _DEBUG
//#define DEBUG_MENU
//#endif
//
//#ifdef DEBUG_MENU
class Text
{
public:
	ID3DXFont *font;
	RECT fRectangle;
	std::string textMessage;
	Text()
	{
		font = NULL;
	}
	~Text()
	{

	}
	void Text::showText(float value, int redValue);
};

class Slider
{
public:
	ID3DXFont *font;
	RECT fRectangle;
	std::string textMessage;
	Slider()
	{
		font = NULL;
		sliderMsg = "*-------------------";
	}
	~Slider()
	{

	}
	std::string sliderMsg; // length = 20

	void Slider::showSlider(int lastPosition, int newPosition, int redValue);
};

class Button
{
public:
	ID3DXFont *font;
	RECT fRectangle;
	std::string textMessage;
	bool isSelected;

	Button()
	{
		font = NULL;
		isSelected = false;

	}
	~Button()
	{

	}
	void showButton(int redValue);
};

class CheckBox
{
public:
	ID3DXFont *font;
	RECT fRectangle;
	std::string textMessage;
	bool isSelected;
	CheckBox()
	{
		font = NULL;
		isSelected = false;
	}
	~CheckBox()
	{

	}
	void showCheckBox(int redValue, bool enabled);
};

class DebugMenu
{
public:

	DebugMenu();
	~DebugMenu();
	void Activate(bool active, IDirect3DDevice9* s_direct3dDevice);
	void Update();
	bool isMenuActive;
	static void CreateSlider(const char* name, float* value, float min, float max);
	static void CreateCheckBox(const char* name, bool* value);
	static void CreateText(const char* name, char* value);
	static void CreateButton(const char* name, void(*callback)(void*), void *param);
	float menuOffset;
	float selectedMenu;
	void set(float value)
	{
		menuOffset = value;
	}
	bool checkBoxChecked ;
	bool sliderSelected;
	bool buttonSelected;
	int sliderCurrentPosition;
	int sliderLastPosition;
	// Note that once a widget is created, there is no way to modify it or delete it later.
	// You could add a return value (like an int handle) if you'd like to do that.

private:
	// ... data to store created widgets
	Text textWidget;
	Button buttonWidget;
	CheckBox checkBoxWidget;
	Slider sliderWidget;
	ID3DXFont *font;


};

//#else

//class DebugMenu
//{
//public:
//	// All of these functions will compile out to nothing.
//	static inline void CreateSlider(const char *name, float* value, float min, float max) {}
//	static inline void CreateCheckBox(const char *name, bool* value) {}
//	static inline void CreateText(const char *name, char* value) {}
//	static inline void CreateButton(const char *name, void(*callback)(void*), void *param) {}
//
//};
//
//#endif	// DEBUG_MENU
