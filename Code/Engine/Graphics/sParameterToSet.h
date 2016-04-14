#ifndef S_PARAMETER_H
#define S_PARAMETER_H
#include <cstdint>
#include "Includes.h"
#if defined(EAE6320_PLATFORM_GL)
#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "../../External/OpenGlExtensions/OpenGlExtensions.h"
#endif
#include "../Windows/Functions.h"

typedef
#if defined( EAE6320_PLATFORM_D3D )
const char*
#elif defined( EAE6320_PLATFORM_GL )
GLint
#endif
tUniformHandle;
namespace eae6320 
{

		struct sParameterToSet
		{
			tUniformHandle  uniformHandle;
			Graphics::ShaderTypes shaderType;
			float values[3];
			uint8_t valueCountToSet;
		public:
			void setUniformValues(float xValue, float yValue, float zValue)
			{
				values[0] = xValue;
				values[1] = yValue;
				values[2] = zValue;
			}
		};

}

#endif
