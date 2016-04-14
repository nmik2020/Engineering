#ifndef MATERIALS_H
#define MATERIALS_H

#include "Includes.h"
#if defined( EAE6320_PLATFORM_D3D )
#include <d3d9.h>
#elif defined( EAE6320_PLATFORM_GL )
//#include <gl/GLU.h>
//#include <gl/GL.h>
#endif
#include <cstdint>
#include "../UserOutput/UserOutput.h"
#include <cassert>
#include <iostream>
#include "../Graphics/GraphicsContext.h"
#include "Effect.h"
#include "sParameterToSet.h"
namespace eae6320
{
	class Materials
	{

	public:
		sParameterToSet* uniformData;
		const char* path;
		Effect *effect;
		uint32_t lengthOfUniform;
		uint8_t numberOfTextures;

		std::vector<std::string> handleList;
		std::vector<std::string> textureSamplerList;
		std::vector<std::string> texturePathList;

#if defined( EAE6320_PLATFORM_D3D )
		IDirect3DTexture9* texture;

#elif defined( EAE6320_PLATFORM_GL )
		GLuint texture = 0;
#endif
	public:
		void Init(GraphicsContext& context);
		bool ReadFromBinaryFile(const char * path, GraphicsContext& i_context);
#if defined( EAE6320_PLATFORM_D3D )
		bool LoadTexture(const char* const i_path, GraphicsContext& i_context);

#elif defined( EAE6320_PLATFORM_GL )
		bool LoadTexture(const char* const i_path, GLuint& o_texture, std::string* o_errorMessage);
#endif
		void setTexture(GraphicsContext& context);
	};
}
#endif
