#ifndef EFFECT_H
#define EFFECT_H
#include "sParameterToSet.h"

#if defined( EAE6320_PLATFORM_D3D )
#include <d3d9.h>
#include <d3dx9shader.h>
#elif defined( EAE6320_PLATFORM_GL )

#include <Windows.h>
#include <gl/GLU.h>

#include "../../External/OpenGlExtensions/OpenGlExtensions.h"
#endif

#include "../Graphics/GraphicsContext.h"
#include<cassert>
#include "../Graphics/Camera.h"
namespace eae6320
{

	class Effect
	{
	public:
		typedef
#if defined( EAE6320_PLATFORM_D3D )
			const char*
#elif defined( EAE6320_PLATFORM_GL )
			GLint
#endif
			tUniformHandle;
	private:
		const char* fragmentShaderPath ;
		const char* vertexShaderPath;

		tUniformHandle transformHandle1, transformHandle2, transformHandle3;
#if defined( EAE6320_PLATFORM_D3D )
	public:

		IDirect3DVertexShader9* s_vertexShader = NULL;
		IDirect3DPixelShader9* s_fragmentShader = NULL;
		ID3DXConstantTable *vertexShaderConstantTable = NULL ;
		ID3DXConstantTable* fragmentShaderConstantTable = NULL;

#elif defined( EAE6320_PLATFORM_GL )
	public:
		GLuint s_programId = 0;

#endif
	public:
		const char* path;
		uint8_t renderStates;
		void Init(GraphicsContext& context);
		void setAlphaTransparency(GraphicsContext& context);
		void setDepthTesting(GraphicsContext& context);
		void setDepthWriting(GraphicsContext& context);
		void setFaceCuling(GraphicsContext& context);

		bool ReadFromBinaryFile(const char * path, GraphicsContext& context);
		bool LoadVertexShader(GraphicsContext& context);
		bool LoadFragmentShader(GraphicsContext& context);
		char* ReadFromBinaryShaders(const char * path);
		void setEffects(GraphicsContext& context);
		void setPositionOffset(GraphicsContext& context, Math::cMatrix_transformation& matrix, 
			Math::cMatrix_transformation& localToWorldMatrix, Camera* cameraObject);
		void setUniform(GraphicsContext& context, eae6320::sParameterToSet* parameter, uint32_t count, std::vector<std::string> handleList);
			void Shutdown()
		{
#if defined( EAE6320_PLATFORM_D3D )
			if (s_vertexShader)
			{
				s_vertexShader->Release();
				s_vertexShader = NULL;
			}
			if (s_fragmentShader)
			{
				s_fragmentShader->Release();
				s_fragmentShader = NULL;
			}
#elif defined(EAE6320_PLATFORM_GL)
			if (s_programId != 0)
			{
				glDeleteProgram(s_programId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					/*std::stringstream errorMessage;
					errorMessage << "OpenGL failed to delete the program: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));*/
				//	UserOutput::Print(errorMessage.str());
				}
				s_programId = 0;
			}
#endif

		}

	};

}

#endif

