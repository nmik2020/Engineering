#include "GraphicsContext.h"

void eae6320::GraphicsContext::StartScene()
{
#if defined (EAE6320_PLATFORM_D3D)
	if (s_direct3dDevice) {
		HRESULT result = s_direct3dDevice->BeginScene();
	    assert(SUCCEEDED(result));
	}
#elif defined (EAE6320_PLATFORM_GL)
	
#endif
}

void eae6320::GraphicsContext::EndScene()
{
#if defined (EAE6320_PLATFORM_D3D)
	HRESULT result = s_direct3dDevice->EndScene();
	assert(SUCCEEDED(result));
#elif defined (EAE6320_PLATFORM_GL)

#endif
}

void eae6320::GraphicsContext::displayRenderedBuffer()
{
#if defined (EAE6320_PLATFORM_D3D)
	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it, the contents of the back buffer must be "presented"
	// (to the front buffer)
{
	const RECT* noSourceRectangle = NULL;
	const RECT* noDestinationRectangle = NULL;
	const HWND useDefaultWindow = NULL;
	const RGNDATA* noDirtyRegion = NULL;
	if (s_direct3dDevice) {

		HRESULT result = s_direct3dDevice->Present(noSourceRectangle, noDestinationRectangle, useDefaultWindow, noDirtyRegion);
		assert(SUCCEEDED(result));
	}
}
#elif defined (EAE6320_PLATFORM_GL)
	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it, the contents of the back buffer must be swapped with the "front buffer"
	// (which is what the user sees)
{
	BOOL result = SwapBuffers(s_deviceContext);
	assert(result != FALSE);
}
#endif
}

 


void eae6320::GraphicsContext::clear()
{
	// Every frame an entirely new image will be created.
	// Before drawing anything, then, the previous image will be erased
	// by "clearing" the image buffer (filling it with a solid color)
	{
#if defined( EAE6320_PLATFORM_D3D )

		const D3DRECT* subRectanglesToClear = NULL;
		const DWORD subRectangleCount = 0;
		const DWORD clearTheRenderTarget = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;
		D3DCOLOR clearColor;
		{
			// Black is usually used:
			clearColor = D3DCOLOR_XRGB(0, 0, 0);
		}
		const float noZBuffer = 1.0f;
		const DWORD noStencilBuffer = 0;
		if (s_direct3dDevice) {
			HRESULT result = s_direct3dDevice->Clear(subRectangleCount, subRectanglesToClear,
				clearTheRenderTarget, clearColor, noZBuffer, noStencilBuffer);
			assert(SUCCEEDED(result));
		}

#elif defined( EAE6320_PLATFORM_GL )

		// Every frame an entirely new image will be created.
		// Before drawing anything, then, the previous image will be erased
		// by "clearing" the image buffer (filling it with a solid color)
		{
			//If depth writing is disable
			glDepthMask(GL_TRUE);
			// Black is usually used
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			assert(glGetError() == GL_NO_ERROR);
			// In addition to the color, "depth" and "stencil" can also be cleared,
			// but for now we only care about color
			const GLbitfield clearColor = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
			glClear(clearColor);
			assert(glGetError() == GL_NO_ERROR);
		}
#endif

	}
}