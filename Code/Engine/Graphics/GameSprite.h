#ifndef GAMESPRITE_H
#define GAMESPRITE_H


#include <d3dx9.h>
#include <string>

class GameSprite
{
public:
	GameSprite();
	GameSprite(float x, float y);
	~GameSprite();
	float aspect;
	int width;
	int height;
	int offset;
	RECT rct;
	//Sprite Functions
	bool Initialize(LPDIRECT3DDEVICE9 device, std::string file, int width, int height);
	bool isInitialized();
	std::string file;
	virtual void Update(float gameTime);
	virtual void Draw(float gameTime);
	int animationIndex;
private:
	LPDIRECT3DTEXTURE9 tex;
	LPD3DXSPRITE sprite;
	D3DXVECTOR3 position;
	D3DCOLOR color;
	bool initialized;
};

#endif