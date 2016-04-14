
#ifndef GRAPHICSCONTEXT_H
#define GRAPHICSCONTEXT_H

#if defined( EAE6320_PLATFORM_D3D )
#include <d3d9.h>
#elif defined( EAE6320_PLATFORM_GL )
#include "../Windows/Includes.h"

#include <gl/GLU.h>
#include "../../External/OpenGlExtensions/OpenGlExtensions.h"
#endif

#include <cstdint>
#include "s_vertex.h"
#include <assert.h>
#include "../Math/cVector.h"
namespace eae6320 
{

	struct GraphicsContext
	{
		void StartScene();
		void EndScene();
		void clear();
		void displayRenderedBuffer();
#if defined( EAE6320_PLATFORM_D3D )

		IDirect3DDevice9* s_direct3dDevice;
		uint32_t* indexData;
		uint32_t numberOfIndices;
		uint32_t numberOfVertices;
		Math::cVector cameraStartPoint;
		Math::cVector cameraEndPoint;

		s_vertex *vertexData;
		

		void setDirect3d(IDirect3DDevice9* direct)
		{
			s_direct3dDevice = direct;
		}

		void changeWindingOrder()
		{

			uint32_t *tempindexData = new uint32_t[numberOfIndices];

			for (unsigned int i = 0; i < numberOfIndices; i++)
				tempindexData[i] = indexData[i];

			for (unsigned int i = 0; i < numberOfIndices; i += 3) {
				indexData[i] = tempindexData[i + 2];
				indexData[i + 1] = tempindexData[i + 1];
				indexData[i + 2] = tempindexData[i];

			}

		}
#elif defined( EAE6320_PLATFORM_GL)
	/*	GraphicsContext()
		{

		}

		~GraphicsContext()
		{

		}*/
		struct sLogInfo
		{
			GLchar* memory;
			//I got an error when the defualt constructor of sLogInfo wasn't defined.
			//I guess it is used to create memory as it is being used at multiple locations
			sLogInfo()
			{

			}
			sLogInfo(const size_t i_size)
			{
				memory = reinterpret_cast<GLchar*>(malloc(i_size));
			}

			~sLogInfo()
			{
				if (memory) free(memory);
			}
		};
	
		s_vertex *vertexData;
		uint32_t *indexData;
		uint32_t numberOfIndices;
		uint32_t numberOfVertices;
		HDC s_deviceContext = NULL;

		//sLogInfo info;
#endif

	};



}//End of Namespace

#endif