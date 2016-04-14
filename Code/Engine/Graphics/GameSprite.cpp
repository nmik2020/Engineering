#include "GameSprite.h"

GameSprite::GameSprite()
{
	//Default sprite to position(0 , 0 , 0)
	position.x = 0;
	position.y = 0;
	position.z = 0;
	animationIndex = 0;
	rct.left = 0;
	rct.right = 0;
	rct.top = 0;
	rct.bottom = 0;
	color = D3DCOLOR_ARGB(255, 255, 255, 255);
	width = 0;
	height = 0;
	offset = 0;
	initialized = false;
}

GameSprite::GameSprite(float x, float y)
{
	//set the position to x,y and default z to 0
	position.x = x;
	position.y = y;
	position.z = 0;
	animationIndex = 0;
	rct.left = 0;
	rct.right = 0;
	rct.top = 0;
	rct.bottom = 0;
	width = 0;
	height = 0;
	offset = 0;
	color = D3DCOLOR_ARGB(255, 255, 255, 255);
	initialized = false;
}

bool GameSprite::Initialize(LPDIRECT3DDEVICE9 device, std::string file, int width, int height)
{
	if (!SUCCEEDED(D3DXCreateTextureFromFileEx(device, file.c_str(), width, height, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &tex)))
	{
		std::string s = "There was an issue creating the texture. MAke sure the requested image is available. Requested image :" + file;
		MessageBox(NULL, s.c_str(), NULL, NULL);
		return false;
	}
	//Attempt to create the sprite
	if (!SUCCEEDED(D3DXCreateSprite(device, &sprite)))
	{
		MessageBox(NULL, "There was an issue creating the sprite", NULL, NULL);
		return false;
	}

	initialized = true;
	return true;
}

bool GameSprite::isInitialized()
{
	return initialized;
}

void GameSprite::Update(float gameTime)
{

}


void GameSprite::Draw(float gameTime)
{
	if (sprite && tex)
	{

		sprite->Begin(D3DXSPRITE_ALPHABLEND);
		if(!(rct.left == 0 && rct.right == 0 && rct.top == 0 && rct.bottom == 0))
		sprite->Draw(tex, &rct,NULL, &position, color);
		else
			sprite->Draw(tex, NULL, NULL, &position, color);

		sprite->End();
	}
}

GameSprite::~GameSprite()
{
	if (sprite)
	{
		sprite->Release();
		sprite = 0;
	}

	if (tex)
	{
		tex->Release();
		tex = 0;
	}
}