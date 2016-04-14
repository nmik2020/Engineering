#include "Effect.h"
#include "../../Engine/Math/cVector.h"
#include "../Math/cMatrix_transformation.h"
#include "../Math/cQuaternion.h"
#include "../Math/Functions.h"
#include "../Math/cVector.h"
#include "../UserOutput/UserOutput.h"
#include "../Windows/Functions.h"
#include <sstream>
#include <fstream>

#define ALPHA_TRANSPARENCY 0
#define DEPTH_TESTING 1
#define DEPTH_WRITE 2
#define FACE_CULING 3
namespace eae6320
{

	void Effect::setAlphaTransparency(GraphicsContext& context)
	{
#if defined( EAE6320_PLATFORM_D3D )

		if (((renderStates >> ALPHA_TRANSPARENCY) & 1) == 1) {
			context.s_direct3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			context.s_direct3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			context.s_direct3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		}
		else {
			context.s_direct3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
#elif defined( EAE6320_PLATFORM_GL )
		if (((renderStates >> ALPHA_TRANSPARENCY) & 1) == 1) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else {
			glDisable(GL_BLEND);

		}
#endif
	}


	void Effect::setDepthTesting(GraphicsContext& context)
	{
#if defined( EAE6320_PLATFORM_D3D )

		if (((renderStates >> DEPTH_TESTING) & 1) == 1) {
			context.s_direct3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			context.s_direct3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		}
		else {
			context.s_direct3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		}
#elif defined( EAE6320_PLATFORM_GL )
		if (((renderStates >> DEPTH_TESTING) & 1) == 1) {
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
		}
		else {
			glDisable(GL_DEPTH_TEST);

		}
#endif
	}


	void Effect::setDepthWriting(GraphicsContext& context)
	{
#if defined( EAE6320_PLATFORM_D3D )

		if (((renderStates >> DEPTH_WRITE) & 1) == 1) {
			context.s_direct3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		}
		else {
			context.s_direct3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		}
#elif defined( EAE6320_PLATFORM_GL )
		if (((renderStates >> DEPTH_WRITE) & 1) == 1) {
			glDepthMask(GL_TRUE);
		}
		else {
			glDepthMask(GL_FALSE);
		}
#endif
	}


	void Effect::setFaceCuling(GraphicsContext& context)
	{
#if defined( EAE6320_PLATFORM_D3D )

		if (((renderStates >> FACE_CULING) & 1) == 1) {
			context.s_direct3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		}
		else {
			context.s_direct3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		}
#elif defined( EAE6320_PLATFORM_GL )
		if (((renderStates >> FACE_CULING) & 1) == 1) {
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);
		}
		else {
			glDisable(GL_CULL_FACE);
		}
#endif
	}



	void Effect::setEffects(GraphicsContext& context)
	{
		setAlphaTransparency(context);
		setDepthTesting(context);
		setDepthWriting(context);
		//setFaceCuling(context);
#if defined( EAE6320_PLATFORM_D3D )
		
		HRESULT result = context.s_direct3dDevice->SetVertexShader(s_vertexShader);
		assert(SUCCEEDED(result));
		result = context.s_direct3dDevice->SetPixelShader(s_fragmentShader);
		assert(SUCCEEDED(result));
#elif defined( EAE6320_PLATFORM_GL )
		{
			glUseProgram(s_programId);
			assert(glGetError() == GL_NO_ERROR);
		}
#endif
	}


	void Effect::setUniform(GraphicsContext& context, eae6320::sParameterToSet* parameter, uint32_t count, std::vector<std::string> handleList)
	{
#if defined( EAE6320_PLATFORM_D3D )
		for (size_t i = 0; i < count; i++)
		{
			tUniformHandle uniformHandle = fragmentShaderConstantTable->GetConstantByName(NULL, handleList[i].c_str());
			HRESULT result = fragmentShaderConstantTable->SetFloatArray
				(context.s_direct3dDevice, uniformHandle,
					parameter[i].values, parameter[i].valueCountToSet);
		}
		


#elif defined( EAE6320_PLATFORM_GL )
		for (size_t i = 0; i < count; i++)
		{
			tUniformHandle uniformHandle = glGetUniformLocation(s_programId,
				handleList[i].c_str());
			if (parameter[i].valueCountToSet == 1)
			{
				glUniform1fv(uniformHandle, 1, parameter[i].values);

			}else
			glUniform3fv(uniformHandle, 1, parameter[i].values);

		}
#endif
	}

	void Effect::setPositionOffset(GraphicsContext& context, Math::cMatrix_transformation& matrix, Math::cMatrix_transformation& localToWorldMatrix, Camera* cameraObject)
	{

		//Convert from World to View coordinates. The parameters passed are the rotation values for the camera and it's position so that what you see will be based on the camera's movement
		Math::cMatrix_transformation worldToViewMatrix = Math::cMatrix_transformation::CreateWorldToViewTransform(cameraObject->rotation, cameraObject->position);
		//Once that World to View part is done, we now need to convert the coordinates to the Screen
		Math::cMatrix_transformation viewToScreenMatrix = Math::cMatrix_transformation::CreateViewToScreenTransform(cameraObject->fieldOfView, cameraObject->aspectRatio, cameraObject->nearPlaneZ, cameraObject->farPlaneZ);

#if defined( EAE6320_PLATFORM_D3D )
	//	vertexHandle = vertexShaderConstantTable->GetConstantByName(NULL, "g_position_offset");


		//The following handles will parse the g_ values from the Shader
		transformHandle1 = vertexShaderConstantTable->GetConstantByName(NULL, "g_transform_localToWorld");
		transformHandle2 = vertexShaderConstantTable->GetConstantByName(NULL, "g_transform_worldToView");
		transformHandle3 = vertexShaderConstantTable->GetConstantByName(NULL, "g_transform_viewToScreen");

		//HRESULT result2 = vertexShaderConstantTable->SetFloatArray(context.s_direct3dDevice, vertexHandle, offset, 3);
		HRESULT result1 = vertexShaderConstantTable->SetMatrixTranspose(context.s_direct3dDevice, transformHandle1,
			reinterpret_cast<const D3DXMATRIX*>(&localToWorldMatrix));
		HRESULT result2 = vertexShaderConstantTable->SetMatrixTranspose(context.s_direct3dDevice, transformHandle2,
			reinterpret_cast<const D3DXMATRIX*>(&worldToViewMatrix));
		HRESULT result3 = vertexShaderConstantTable->SetMatrixTranspose(context.s_direct3dDevice, transformHandle3,
			reinterpret_cast<const D3DXMATRIX*>(&viewToScreenMatrix));

		assert(SUCCEEDED(result1 && result2 && result3));
		//assert(SUCCEEDED(result2));

#elif defined( EAE6320_PLATFORM_GL )
		//location = glGetUniformLocation(s_programId, "g_position_offset");

		transformHandle1 = glGetUniformLocation(s_programId, "g_transform_localToWorld");
		transformHandle2 = glGetUniformLocation(s_programId, "g_transform_worldToView");
		transformHandle3 = glGetUniformLocation(s_programId, "g_transform_viewToScreen");

		//glUniform2fv(location, 1, offset);
		const GLboolean dontTranspose = false; // Matrices are already in the correct format
		const GLsizei uniformCountToSet = 1;
		glUniformMatrix4fv(transformHandle1, uniformCountToSet, dontTranspose, reinterpret_cast<const GLfloat*>(&localToWorldMatrix));
		glUniformMatrix4fv(transformHandle2, uniformCountToSet, dontTranspose, reinterpret_cast<const GLfloat*>(&worldToViewMatrix));
		glUniformMatrix4fv(transformHandle3, uniformCountToSet, dontTranspose, reinterpret_cast<const GLfloat*>(&viewToScreenMatrix));

#endif

	}


	void Effect::Init(GraphicsContext& context)
	{
		if (!Effect::ReadFromBinaryFile(path, context))
		{
			eae6320::UserOutput::Print("Effects.Lua not loaded properly");
		}
	}
	bool Effect::ReadFromBinaryFile(const char * path, GraphicsContext& context)
	{

		std::ifstream inputFile(path, std::ifstream::in | std::ifstream::binary);
		if (inputFile)
		{
			inputFile.seekg(0, inputFile.end);
			int length = inputFile.tellg();
			inputFile.seekg(0, inputFile.beg);
			char * buffer = new char[length];
			inputFile.read(buffer, length);

			//Get render States from the first byte
			renderStates = *reinterpret_cast<uint8_t*>(buffer);
			//Size of string gives the first string's size so that I don't have to calculate it here
			uint8_t sizeOfFirstString = *reinterpret_cast<uint8_t*>(buffer + sizeof(uint8_t));

			//Added a null terminator at the end of each string or else I would get both strings inside vertices. +1 because I need the string after the size
			 vertexShaderPath = reinterpret_cast<char*>(buffer + 2*sizeof(uint8_t));
			//+2 because there's a size at the beginning and and a null terminator at the end of the first string
			 fragmentShaderPath = reinterpret_cast<char*>(buffer + 3*sizeof(uint8_t) + sizeOfFirstString);
			 
		}
		return true;
	}

	char* Effect::ReadFromBinaryShaders(const char * path)
	{

		std::ifstream inputFile(path, std::ifstream::in | std::ifstream::binary);
		if (inputFile)
		{
			inputFile.seekg(0, inputFile.end);
			int length = inputFile.tellg();
			inputFile.seekg(0, inputFile.beg);
			char * buffer = new char[length];
			inputFile.read(buffer, length);
			return buffer;

		}
	}

#if defined( EAE6320_PLATFORM_D3D )

	bool Effect::LoadFragmentShader(GraphicsContext& context)
	{
		// Load the source code from file and compile it
		ID3DXBuffer* compiledShader;
		{

		
			//const char* sourceCodeFileName = "data/fragment.shader";

			//const D3DXMACRO* noMacros = NULL;
			const D3DXMACRO defines[] =
			{
				{ "EAE6320_PLATFORM_D3D", "1" },
				{ NULL, NULL }
			};
			ID3DXInclude* noIncludes = NULL;
			const char* entryPoint = "main";
			const char* profile = "ps_3_0";
			const DWORD noFlags = 0;
			ID3DXBuffer* errorMessages = NULL;
			ID3DXConstantTable** noConstants = NULL;

		}
		// Create the fragment shader object

		char *buffer = Effect::ReadFromBinaryShaders(fragmentShaderPath);
		D3DXGetShaderConstantTable(reinterpret_cast<const DWORD*>(buffer), &fragmentShaderConstantTable);
		/*if (!Effect::ReadFromBinaryShaders(fragmentShaderPath))
		{
			eae6320::UserOutput::Print("Fragment Shader not loaded properly");
		}*/
		bool wereThereErrors = false;
		{
			HRESULT result = context.s_direct3dDevice->CreatePixelShader(reinterpret_cast<DWORD*>(buffer),
				&s_fragmentShader);
			if (FAILED(result))
			{
				eae6320::UserOutput::Print("Direct3D failed to create the fragment shader");
				wereThereErrors = true;
			}
			//compiledShader->Release();
		}
		return !wereThereErrors;
	}

	bool Effect::LoadVertexShader(GraphicsContext& context)
	{
		
		// Create the vertex shader object

		char *buffer = Effect::ReadFromBinaryShaders(vertexShaderPath);
		D3DXGetShaderConstantTable(reinterpret_cast<const DWORD*>(buffer), &vertexShaderConstantTable);

		bool wereThereErrors = false;
		{
			HRESULT result = context.s_direct3dDevice->CreateVertexShader(reinterpret_cast<DWORD*>(buffer),
				&s_vertexShader);
			if (FAILED(result))
			{
				eae6320::UserOutput::Print("Direct3D failed to create the vertex shader");
				wereThereErrors = true;
			}
			//compiledShader->Release();
		}
		return !wereThereErrors;
	}
#elif defined( EAE6320_PLATFORM_GL )

bool LoadAndAllocateShaderProgram(const char* i_path, void*& o_shader, size_t& o_size, std::string* o_errorMessage)
{
	bool wereThereErrors = false;

	// Load the shader source from disk
	o_shader = NULL;
	HANDLE fileHandle = INVALID_HANDLE_VALUE;
	{
		// Open the file
		{
			const DWORD desiredAccess = FILE_GENERIC_READ;
			const DWORD otherProgramsCanStillReadTheFile = FILE_SHARE_READ;
			SECURITY_ATTRIBUTES* useDefaultSecurity = NULL;
			const DWORD onlySucceedIfFileExists = OPEN_EXISTING;
			const DWORD useDefaultAttributes = FILE_ATTRIBUTE_NORMAL;
			const HANDLE dontUseTemplateFile = NULL;
			fileHandle = CreateFile(i_path, desiredAccess, otherProgramsCanStillReadTheFile,
				useDefaultSecurity, onlySucceedIfFileExists, useDefaultAttributes, dontUseTemplateFile);
			if (fileHandle == INVALID_HANDLE_VALUE)
			{
				wereThereErrors = true;
				if (o_errorMessage)
				{
					std::string windowsErrorMessage = eae6320::GetLastWindowsError();
					std::stringstream errorMessage;
					errorMessage << "Windows failed to open the shader file: " << windowsErrorMessage;
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
		}
		// Get the file's size
		{
			LARGE_INTEGER fileSize_integer;
			if (GetFileSizeEx(fileHandle, &fileSize_integer) != FALSE)
			{
				assert(fileSize_integer.QuadPart <= SIZE_MAX);
				o_size = static_cast<size_t>(fileSize_integer.QuadPart);
			}
			else
			{
				wereThereErrors = true;
				if (o_errorMessage)
				{
					std::string windowsErrorMessage = eae6320::GetLastWindowsError();
					std::stringstream errorMessage;
					errorMessage << "Windows failed to get the size of shader: " << windowsErrorMessage;
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
			// Add an extra byte for a NULL terminator
			o_size += 1;
		}
		// Read the file's contents into temporary memory
		o_shader = malloc(o_size);
		if (o_shader)
		{
			DWORD bytesReadCount;
			OVERLAPPED* readSynchronously = NULL;
			if (ReadFile(fileHandle, o_shader, o_size,
				&bytesReadCount, readSynchronously) == FALSE)
			{
				wereThereErrors = true;
				if (o_errorMessage)
				{
					std::string windowsErrorMessage = eae6320::GetLastWindowsError();
					std::stringstream errorMessage;
					errorMessage << "Windows failed to read the contents of shader: " << windowsErrorMessage;
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			if (o_errorMessage)
			{
				std::stringstream errorMessage;
				errorMessage << "Failed to allocate " << o_size << " bytes to read in the shader program " << i_path;
				*o_errorMessage = errorMessage.str();
			}
			goto OnExit;
		}
		// Add the NULL terminator
		reinterpret_cast<char*>(o_shader)[o_size - 1] = '\0';
	}

OnExit:

	if (wereThereErrors && o_shader)
	{
		free(o_shader);
		o_shader = NULL;
	}
	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		if (CloseHandle(fileHandle) == FALSE)
		{
			if (!wereThereErrors && o_errorMessage)
			{
				std::string windowsError = eae6320::GetLastWindowsError();
				std::stringstream errorMessage;
				errorMessage << "Windows failed to close the shader file handle: " << windowsError;
				*o_errorMessage = errorMessage.str();
			}
			wereThereErrors = true;
		}
		fileHandle = INVALID_HANDLE_VALUE;
	}

	return !wereThereErrors;
}

	bool Effect::LoadFragmentShader(GraphicsContext& context)
	{
		// Verify that compiling shaders at run-time is supported
		{
			GLboolean isShaderCompilingSupported;
			glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
			if (!isShaderCompilingSupported)
			{
				eae6320::UserOutput::Print("Compiling shaders at run-time isn't supported on this implementation (this should never happen)");
				return false;
			}
		}

		bool wereThereErrors = false;

		// Load the source code from file and set it into a shader
		GLuint fragmentShaderId = 0;
		void* shaderSource = NULL;
		{
			// Load the shader source code
			size_t fileSize;
			{
				//const char* sourceCodeFileName = "data/fragment.shader";
				std::string errorMessage;
				if (!LoadAndAllocateShaderProgram(fragmentShaderPath, shaderSource, fileSize, &errorMessage))
				{
					wereThereErrors = true;
					eae6320::UserOutput::Print(errorMessage);
					goto OnExit;
				}
			}
			// Generate a shader
			fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
			{
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to get an unused fragment shader ID: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					eae6320::UserOutput::Print(errorMessage.str());
					goto OnExit;
				}
				else if (fragmentShaderId == 0)
				{
					wereThereErrors = true;
					eae6320::UserOutput::Print("OpenGL failed to get an unused fragment shader ID");
					goto OnExit;
				}
			}
			// Set the source code into the shader
			{
				const GLsizei shaderSourceCount = 1;
				const GLchar* shaderSources[] =
				{
					reinterpret_cast<GLchar*>(shaderSource)
				};
				const GLint* sourcesAreNullTerminated = NULL;
				glShaderSource(fragmentShaderId, shaderSourceCount, shaderSources, sourcesAreNullTerminated);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to set the fragment shader source code: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					eae6320::UserOutput::Print(errorMessage.str());
					goto OnExit;
				}
			}
		}
		// Compile the shader source code
		{
			glCompileShader(fragmentShaderId);
			GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				// Get compilation info
				// (this won't be used unless compilation fails
				// but it can be useful to look at when debugging)
				std::string compilationInfo;
				{
					GLint infoSize;
					glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoSize);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						GraphicsContext::sLogInfo info(static_cast<size_t>(infoSize));

						GLsizei* dontReturnLength = NULL;
						glGetShaderInfoLog(fragmentShaderId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
						errorCode = glGetError();
						if (errorCode == GL_NO_ERROR)
						{
							compilationInfo = info.memory;
						}
						else
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "OpenGL failed to get compilation info of the fragment shader source code: " <<
								reinterpret_cast<const char*>(gluErrorString(errorCode));
							eae6320::UserOutput::Print(errorMessage.str());
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get the length of the fragment shader compilation info: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						eae6320::UserOutput::Print(errorMessage.str());
						goto OnExit;
					}
				}
				// Check to see if there were compilation errors
				GLint didCompilationSucceed;
				{
					glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &didCompilationSucceed);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						if (didCompilationSucceed == GL_FALSE)
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "The fragment shader failed to compile:\n" << compilationInfo;
							eae6320::UserOutput::Print(errorMessage.str());
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to find out if compilation of the fragment shader source code succeeded: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						eae6320::UserOutput::Print(errorMessage.str());
						goto OnExit;
					}
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to compile the fragment shader source code: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				eae6320::UserOutput::Print(errorMessage.str());
				goto OnExit;
			}
		}
		// Attach the shader to the program
		{
			glAttachShader(s_programId, fragmentShaderId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to attach the fragment shader to the program: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				eae6320::UserOutput::Print(errorMessage.str());
				goto OnExit;
			}
		}

	OnExit:

		if (fragmentShaderId != 0)
		{
			// Even if the shader was successfully compiled
			// once it has been attached to the program we can (and should) delete our reference to it
			// (any associated memory that OpenGL has allocated internally will be freed
			// once the program is deleted)
			glDeleteShader(fragmentShaderId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to delete the fragment shader ID: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				eae6320::UserOutput::Print(errorMessage.str());
			}
			fragmentShaderId = 0;
		}
		if (shaderSource != NULL)
		{
			free(shaderSource);
			shaderSource = NULL;
		}

		return !wereThereErrors;
	}

	bool Effect::LoadVertexShader(GraphicsContext& context)
	{
		// Verify that compiling shaders at run-time is supported
		{
			GLboolean isShaderCompilingSupported;
			glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
			if (!isShaderCompilingSupported)
			{
				eae6320::UserOutput::Print("Compiling shaders at run-time isn't supported on this implementation (this should never happen)");
				return false;
			}
		}

		bool wereThereErrors = false;

		// Load the source code from file and set it into a shader
		GLuint vertexShaderId = 0;
		void* shaderSource = NULL;
		{
			// Load the shader source code
			size_t fileSize;
			{
				//const char* sourceCodeFileName = "data/vertex.shader";
				std::string errorMessage;
				if (!LoadAndAllocateShaderProgram(vertexShaderPath, shaderSource, fileSize, &errorMessage))
				{
					wereThereErrors = true;
					eae6320::UserOutput::Print(errorMessage);
					goto OnExit;
				}
			}
			// Generate a shader
			vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
			{
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to get an unused vertex shader ID: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					eae6320::UserOutput::Print(errorMessage.str());
					goto OnExit;
				}
				else if (vertexShaderId == 0)
				{
					wereThereErrors = true;
					eae6320::UserOutput::Print("OpenGL failed to get an unused vertex shader ID");
					goto OnExit;
				}
			}
			// Set the source code into the shader
			{
				const GLsizei shaderSourceCount = 1;
				const GLchar* shaderSources[] =
				{
					reinterpret_cast<GLchar*>(shaderSource)
				};
				const GLint* sourcesAreNullTerminated = NULL;
				glShaderSource(vertexShaderId, shaderSourceCount, shaderSources, sourcesAreNullTerminated);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to set the vertex shader source code: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					eae6320::UserOutput::Print(errorMessage.str());
					goto OnExit;
				}
			}
		}
		// Compile the shader source code
		{
			glCompileShader(vertexShaderId);
			GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				// Get compilation info
				// (this won't be used unless compilation fails
				// but it can be useful to look at when debugging)
				std::string compilationInfo;
				{
					GLint infoSize;
					glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoSize);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						GraphicsContext::sLogInfo info(static_cast<size_t>(infoSize));
						GLsizei* dontReturnLength = NULL;
						glGetShaderInfoLog(vertexShaderId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
						errorCode = glGetError();
						if (errorCode == GL_NO_ERROR)
						{
							compilationInfo = info.memory;
						}
						else
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "OpenGL failed to get compilation info of the vertex shader source code: " <<
								reinterpret_cast<const char*>(gluErrorString(errorCode));
							eae6320::UserOutput::Print(errorMessage.str());
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get the length of the vertex shader compilation info: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						eae6320::UserOutput::Print(errorMessage.str());
						goto OnExit;
					}
				}
				// Check to see if there were compilation errors
				GLint didCompilationSucceed;
				{
					glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &didCompilationSucceed);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						if (didCompilationSucceed == GL_FALSE)
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "The vertex shader failed to compile:\n" << compilationInfo;
							eae6320::UserOutput::Print(errorMessage.str());
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to find out if compilation of the vertex shader source code succeeded: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						eae6320::UserOutput::Print(errorMessage.str());
						goto OnExit;
					}
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to compile the vertex shader source code: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				eae6320::UserOutput::Print(errorMessage.str());
				goto OnExit;
			}
		}
		// Attach the shader to the program
		{
			glAttachShader(s_programId, vertexShaderId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to attach the vertex shader to the program: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				eae6320::UserOutput::Print(errorMessage.str());
				goto OnExit;
			}
		}

	OnExit:

		if (vertexShaderId != 0)
		{
			// Even if the shader was successfully compiled
			// once it has been attached to the program we can (and should) delete our reference to it
			// (any associated memory that OpenGL has allocated internally will be freed
			// once the program is deleted)
			glDeleteShader(vertexShaderId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to delete the vertex shader ID: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				eae6320::UserOutput::Print(errorMessage.str());
			}
			vertexShaderId = 0;
		}
		if (shaderSource != NULL)
		{
			free(shaderSource);
			shaderSource = NULL;
		}

		return !wereThereErrors;
	}
#endif
}