// Header Files
//=============

#include "../Graphics.h"
#include <d3d9.h>
#include <d3dx9shader.h>
#include "../Mesh.h"
#include "../Effect.h"


namespace
{

}

// Helper Function Declarations
//=============================

namespace
{
	bool CreateDevice();
	bool CreateInterface();
}

// Interface
//==========

/*void init()
{

}*/

void eae6320::Graphics::Submit(std::vector<eae6320::GameObject*> objects)
{
	gameObjects = objects;
}
void eae6320::Graphics::SubmitDebugObjects(std::vector<eae6320::Graphics::DebugBox*> debugBoxObjects,
	std::vector<eae6320::Graphics::DebugLine*> debugLineObjects,
	std::vector<eae6320::Graphics::DebugSphere*> debugSphereObjects)
{
	debugBoxes = debugBoxObjects;
	debugLines = debugLineObjects;
	debugSpheres = debugSphereObjects;

}



void eae6320::Graphics::SubmitSpriteObjects(std::vector<GameSprite*>& objects)
{
	spriteObjects = objects;
}
void eae6320::Graphics::SubmitDebugMenu(DebugMenu& menu)
{
	debugMenu = menu;
}
bool eae6320::Graphics::Initialize(const HWND i_renderingWindow)
{
	s_renderingWindow = i_renderingWindow;
	// Initialize the interface to the Direct3D9 library
	if (!CreateInterface())
	{
		return false;
	}
	// Create an interface to a Direct3D device
	if (!CreateDevice())
	{
		goto OnError;
	}

	context.setDirect3d(s_direct3dDevice);
	HRESULT result1 = s_direct3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	HRESULT result2 = s_direct3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	HRESULT result3 = s_direct3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	
	for (unsigned int i = 0; i < spriteObjects.size(); i++)
	{
		GameSprite *currentObject;
		currentObject = spriteObjects[i];
		currentObject->Initialize(s_direct3dDevice, currentObject->file.c_str(), 
			currentObject->width, currentObject->height);
	}

	for (unsigned int i = 0; i < debugBoxes.size(); i++)
	{
		DebugBox *currentObject;
		currentObject = debugBoxes[i];
		currentObject->LoadDebugBox();
	}

	for (unsigned int i = 0; i < debugLines.size(); i++)
	{
		DebugLine *currentObject;
		currentObject = debugLines[i];
		currentObject->LoadDebugLine();
	}

	for (unsigned int i = 0; i < debugSpheres.size(); i++)
	{
		DebugSphere *currentObject;
		currentObject = debugSpheres[i];
		currentObject->LoadDebugSphere();
	}


	for (unsigned int i = 0; i < gameObjects.size(); i++)
	{
		GameObject *currentObject = gameObjects[i];
		//Initializing Effects...This will read from the binary file effects and make sure the effects path has the correct shader files
		currentObject->material->Init(context);
		if (!currentObject->mesh->LoadMesh(context))
			goto OnError;
		if (!currentObject->material->effect->LoadVertexShader(context))
		{
			goto OnError;
		}
		if (!currentObject->material->effect->LoadFragmentShader(context))
		{
			goto OnError;
		}

	}



	return true;

OnError:

	ShutDown();
	return false;
}



bool eae6320::Graphics::ShutDown()
{
	//effects.vertexShaderConstantTable->Release();
	bool wereThereErrors = false;

	if (s_direct3dInterface)
	{
		if (s_direct3dDevice)
		{

			for (unsigned int i = 0; i < gameObjects.size(); i++)
			{
				GameObject *currentObject = gameObjects[i];
				currentObject->mesh->ShutDown(context);

			}
			//effects.Shutdown();
			/*squareMesh.ShutDown(context);
			triangularMesh.ShutDown(context);
			triangularMesh2.ShutDown(context);*/


			s_direct3dDevice->Release();
			s_direct3dDevice = NULL;
		}

		s_direct3dInterface->Release();
		s_direct3dInterface = NULL;
	}
	s_renderingWindow = NULL;
	gameObjects.clear();
	return !wereThereErrors;
}

// Helper Function Definitions
//============================

namespace
{
	bool CreateDevice()
	{
		const UINT useDefaultDevice = D3DADAPTER_DEFAULT;
		const D3DDEVTYPE useHardwareRendering = D3DDEVTYPE_HAL;
		const DWORD useHardwareVertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		D3DPRESENT_PARAMETERS presentationParameters = { 0 };
		{
			{
				const unsigned int useRenderingWindowDimensions = 0;
				presentationParameters.BackBufferWidth = useRenderingWindowDimensions;
				presentationParameters.BackBufferHeight = useRenderingWindowDimensions;
			}
			presentationParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
			presentationParameters.BackBufferCount = 1;
			presentationParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
			presentationParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
			presentationParameters.hDeviceWindow = s_renderingWindow;
			presentationParameters.Windowed = TRUE;
			presentationParameters.EnableAutoDepthStencil = TRUE;
			presentationParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
			presentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
		}
		HRESULT result = s_direct3dInterface->CreateDevice(useDefaultDevice, useHardwareRendering,
			s_renderingWindow, useHardwareVertexProcessing, &presentationParameters, &s_direct3dDevice);
		if (SUCCEEDED(result))
		{
			return true;
		}
		else
		{
			eae6320::UserOutput::Print("Direct3D failed to create a Direct3D9 device");
			return false;
		}
	}

	bool CreateInterface()
	{
		// D3D_SDK_VERSION is #defined by the Direct3D header files,
		// and is just a way to make sure that everything is up-to-date
		s_direct3dInterface = Direct3DCreate9(D3D_SDK_VERSION);
		if (s_direct3dInterface)
		{
			return true;
		}
		else
		{
			eae6320::UserOutput::Print("DirectX failed to create a Direct3D9 interface");
			return false;
		}
	}
}
