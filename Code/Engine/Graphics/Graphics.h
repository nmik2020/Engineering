/*
This file contains the function declarations for graphics
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Header Files
//=============

#include "../Windows/Includes.h"

#if defined( EAE6320_PLATFORM_GL )
#include <gl/GL.h>
#include <gl/GLU.h>
#include "../../External/OpenGlExtensions/OpenGlExtensions.h"
#endif

#include "../UserOutput/UserOutput.h"
#include <vector>
#include <cassert>
#include <cstdint>
#include <sstream>
#include "../GameObject/GameObject.h"
#include "DebugShapes.h"
#include "GameSprite.h"
#include "DebugMenu.h"

// Interface
//==========

// Static Data Initialization
//===========================

#if defined( EAE6320_PLATFORM_D3D )

extern HWND s_renderingWindow ;
extern IDirect3D9* s_direct3dInterface;
extern IDirect3DDevice9* s_direct3dDevice;

#elif defined( EAE6320_PLATFORM_GL )

#endif
extern std::vector<eae6320::GameObject*> gameObjects;
extern std::vector<eae6320::Graphics::DebugBox*> debugBoxes;
extern std::vector<eae6320::Graphics::DebugLine*> debugLines;
extern std::vector<eae6320::Graphics::DebugSphere*> debugSpheres;
extern std::vector<GameSprite*> spriteObjects;
extern DebugMenu debugMenu;


extern eae6320::GraphicsContext context;
//extern eae6320::Effect effects;


namespace eae6320
{
	void  handleCollisions(float i_frameTime);
	static bool onTheGround = false;
	static float timeLeftInFrame;
	namespace Graphics
	{
		bool Initialize(const HWND i_renderingWindow);
		void Render();
		void updateWorld();

		 void RunDebugMenu();
		void Submit(std::vector<eae6320::GameObject*> objects);
		void SubmitDebugObjects(std::vector<eae6320::Graphics::DebugBox*> debugBoxObjects,
			std::vector<eae6320::Graphics::DebugLine*> debugLineObjects,
			std::vector<eae6320::Graphics::DebugSphere*> debugSphereObjects);
		void SubmitSpriteObjects(std::vector<GameSprite*>& objects);
		void SubmitDebugMenu(DebugMenu& menu);
		bool ShutDown();
		static bool isDebugMenuActive = false ;
		void  traceRayVertical();
		void  traceRayHorizontal(float& timeCollided);

		int IntersectSegmentTriangle(eae6320::Math::cVector p, eae6320::Math::cVector q, eae6320::Math::cVector a, eae6320::Math::cVector b, eae6320::Math::cVector c,
			float &u, float &v, float &w, float &t, eae6320::Math::cVector &normal);
	}
}

#endif	// EAE6320_GRAPHICS_H
