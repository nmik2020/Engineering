#include "Materials.h"
#include <fstream>
#include <algorithm>	// For std::max
#include <sstream>
#include "../../External/OpenGlExtensions/OpenGlExtensions.h"

namespace eae6320
{

	void Materials::Init(GraphicsContext& context)
	{
		effect = new Effect();
		if (!ReadFromBinaryFile(path, context))
		{
			eae6320::UserOutput::Print("Effects.Lua not loaded properly");
		}
		for (size_t i = 0; i < texturePathList.size(); i++)
		bool success = LoadTexture(texturePathList[i].c_str(), context);

		effect->Init(context);

	}

	void Materials::setTexture(GraphicsContext& context)
	{
#if defined( EAE6320_PLATFORM_D3D )
		for (size_t i = 0; i < texturePathList.size(); i++)
		{
			
			//bool success = LoadTexture(texturePathList[i].c_str(), context);
			tUniformHandle uniformHandle = NULL;// effect->fragmentShaderConstantTable->GetConstantByName(NULL, textureSamplerList[i].c_str());
			UINT samplerID = 0;// effect->fragmentShaderConstantTable->GetSamplerIndex(uniformHandle);
			context.s_direct3dDevice->SetTexture(static_cast<DWORD>(samplerID), texture);
	
		}

#elif defined( EAE6320_PLATFORM_GL )
		for (size_t i = 0; i < textureSamplerList.size(); i++)
		{
			tUniformHandle uniformHandle = glGetUniformLocation(effect->s_programId,
				textureSamplerList[i].c_str());
			glActiveTexture(GL_TEXTURE0 + i );
			const GLenum errorCode1 = glGetError();

			std::string errormessage = "" ;
			bool success = LoadTexture(texturePathList[i].c_str(), texture, &errormessage);
			glBindTexture(GL_TEXTURE_2D, texture);
			const GLenum errorCode2 = glGetError();

			 glUniform1i(uniformHandle, i);
			 const GLenum errorCode3 = glGetError();

		}
#endif
	}
	bool Materials::ReadFromBinaryFile(const char * path, GraphicsContext& i_context)
	{

		std::ifstream inputFile(path, std::ifstream::in | std::ifstream::binary);
		if (inputFile)
		{
			inputFile.seekg(0, inputFile.end);
			int length = inputFile.tellg();
			inputFile.seekg(0, inputFile.beg);
			char * buffer = new char[length];
			inputFile.read(buffer, length);

			//Size of string gives the first string's size so that I don't have to calculate it here
			uint8_t sizeOfFirstString = *reinterpret_cast<uint8_t*>(buffer);

			//Added a null terminator at the end of each string or else I would get both strings inside vertices. +1 because I need the string after the size
			effect->path = reinterpret_cast<char*>(buffer + sizeof(uint8_t));

			lengthOfUniform = *reinterpret_cast<uint8_t*>(buffer + sizeOfFirstString + 2 * sizeof(uint8_t));
			uniformData = new sParameterToSet[lengthOfUniform];
			//+2 because there's a size at the beginning and and a null terminator at the end of the first string
			uniformData = reinterpret_cast<eae6320::sParameterToSet*>(buffer + 2 * sizeof(uint8_t) + sizeOfFirstString + sizeof(uint8_t));
			size_t b = lengthOfUniform*sizeof(sParameterToSet);
			size_t temp;
			//Might have to change this to use memcpy
			for (uint8_t i = 0; i < lengthOfUniform; i++)
			{
				std::string t1;
				if (i == 0) {
					temp = 2 * sizeof(uint8_t)
						+ sizeOfFirstString + sizeof(uint8_t)
						+ lengthOfUniform*sizeof(sParameterToSet);
					t1 = reinterpret_cast<char*>(buffer + temp);
				}
				else {
					temp = 2 * sizeof(uint8_t)
						+ sizeOfFirstString + sizeof(uint8_t)
						+ lengthOfUniform*sizeof(sParameterToSet) + handleList[i - 1].size() + (i) * 1;
					t1 = reinterpret_cast<char*>(buffer + temp);
				}

				handleList.push_back(t1);
			}
			size_t ty =  temp + handleList[lengthOfUniform - 1].length() + 1;
			numberOfTextures = *reinterpret_cast<uint8_t*>(buffer + ty);

			size_t temp2;

			for (uint8_t i = 0; i < numberOfTextures; i++)
			{
				std::string t1;
				if (i == 0) {
					// + 2 for null /0 after uniform and for length of textures
					temp2 = ty + sizeof(uint8_t);
					t1 = reinterpret_cast<char*>(buffer + temp2);
				}
				else {
					temp2  +=
						textureSamplerList[i - 1].length();

					t1 = reinterpret_cast<char*>(buffer + temp2 + sizeof(uint8_t));
				}
				textureSamplerList.push_back(t1);
			}

			temp2 += numberOfTextures * 1;
			for (uint8_t i = 0; i < numberOfTextures; i++)
			{
				std::string t1;
				if (i == 0)
					t1 = reinterpret_cast<char*>(buffer + temp2 + textureSamplerList[numberOfTextures - 1].length());
				else
					t1 = reinterpret_cast<char*>(buffer + temp2 + textureSamplerList[numberOfTextures - 1].length() + 
						texturePathList[i - 1].length() + i*1);

				texturePathList.push_back(t1);
			}


		

			/*std::string t1 = reinterpret_cast<char*>(buffer + 2 * sizeof(uint8_t) + sizeOfFirstString + sizeof(uint32_t) + lengthOfUniform*sizeof(sParameterToSet));
			std::string t2 = reinterpret_cast<char*>(buffer + 2 * sizeof(uint8_t) + sizeOfFirstString + sizeof(uint32_t) + lengthOfUniform*sizeof(sParameterToSet) + t1.size() + 1);
*/
			std::cout << "";
		}
		return true;
	}


#if defined( EAE6320_PLATFORM_D3D )
	bool Materials::LoadTexture(const char* const texturePath, GraphicsContext& i_context)
	{
		const unsigned int useDimensionsFromFile = D3DX_DEFAULT_NONPOW2;
		const unsigned int useMipMapsFromFile = D3DX_FROM_FILE;
		const DWORD staticTexture = 0;
		const D3DFORMAT useFormatFromFile = D3DFMT_FROM_FILE;
		const D3DPOOL letD3dManageMemory = D3DPOOL_MANAGED;
		const DWORD useDefaultFiltering = D3DX_DEFAULT;
		const D3DCOLOR noColorKey = 0;
		D3DXIMAGE_INFO* noSourceInfo = NULL;
		PALETTEENTRY* noColorPalette = NULL;
		bool result = D3DXCreateTextureFromFileEx(i_context.s_direct3dDevice, texturePath, useDimensionsFromFile, useDimensionsFromFile, useMipMapsFromFile,
			staticTexture, useFormatFromFile, letD3dManageMemory, useDefaultFiltering, useDefaultFiltering, noColorKey, noSourceInfo, noColorPalette,
			&texture);
		return result;
	}

#elif defined( EAE6320_PLATFORM_GL )

	bool Materials::LoadTexture(const char* const i_path, GLuint& o_texture, std::string* o_errorMessage)
	{
		bool wereThereErrors = false;
		HANDLE fileHandle = INVALID_HANDLE_VALUE;
		void* fileContents = NULL;
		o_texture = 0;

		// Open the texture file
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
					std::string windowsErrorMessage(GetLastWindowsError());
					std::stringstream errorMessage;
					errorMessage << "Windows failed to open the texture file: " << windowsErrorMessage;
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
		}
		// Get the file's size
		size_t fileSize;
		{
			LARGE_INTEGER fileSize_integer;
			if (GetFileSizeEx(fileHandle, &fileSize_integer) != FALSE)
			{
				assert(fileSize_integer.QuadPart <= SIZE_MAX);
				fileSize = static_cast<size_t>(fileSize_integer.QuadPart);
			}
			else
			{
				wereThereErrors = true;
				if (o_errorMessage)
				{
					std::string windowsErrorMessage(GetLastWindowsError());
					std::stringstream errorMessage;
					errorMessage << "Windows failed to get the size of the texture file: " << windowsErrorMessage;
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
		}
		// Read the file's contents into temporary memory
		fileContents = malloc(fileSize);
		if (fileContents)
		{
			DWORD bytesReadCount;
			OVERLAPPED* readSynchronously = NULL;
			assert(fileSize < (uint64_t(1) << (sizeof(DWORD) * 8)));
			if (ReadFile(fileHandle, fileContents, static_cast<DWORD>(fileSize),
				&bytesReadCount, readSynchronously) == FALSE)
			{
				wereThereErrors = true;
				if (o_errorMessage)
				{
					std::string windowsErrorMessage(GetLastWindowsError());
					std::stringstream errorMessage;
					errorMessage << "Windows failed to read the contents of the texture file: " << windowsErrorMessage;
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
				errorMessage << "Failed to allocate " << fileSize << " bytes to read in the texture " << i_path;
				*o_errorMessage = errorMessage.str();
			}
			goto OnExit;
		}

		// Create a new texture and make it active
		{
			const GLsizei textureCount = 1;
			glGenTextures(textureCount, &o_texture);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				// This code only supports 2D textures;
				// if you want to support other types you will need to improve this code.
				glBindTexture(GL_TEXTURE_2D, o_texture);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to bind a new texture: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
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
					errorMessage << "OpenGL failed to get an unused texture ID: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
		}

		// Extract the data
		const uint8_t* currentPosition = reinterpret_cast<uint8_t*>(fileContents);
		// Verify that the file is a valid DDS
		{
			const size_t fourCcCount = 4;
			const uint8_t* const fourCc = currentPosition;
			const uint8_t fourCc_dds[fourCcCount] = { 'D', 'D', 'S', ' ' };
			// Each of the four characters can be compared in a single operation by casting to a uint32_t
			const bool isDds = *reinterpret_cast<const uint32_t*>(fourCc) == *reinterpret_cast<const uint32_t*>(fourCc_dds);
			if (isDds)
			{
				currentPosition += fourCcCount;
			}
			else
			{
				wereThereErrors = true;
				if (o_errorMessage)
				{
					char fourCcString[fourCcCount + 1] = { 0 };	// Add NULL terminator
					memcpy(fourCcString, currentPosition, fourCcCount);
					std::stringstream errorMessage;
					errorMessage << "The texture file \"" << i_path << "\" isn't a valid DDS. The Four CC is \"" << fourCcString << "\""
						" instead of \"DDS \"";
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
		}
		// Extract the header
		// (this struct can also be found in Dds.h in the DirectX header files
		// or here as of this comment: https://msdn.microsoft.com/en-us/library/windows/desktop/bb943982(v=vs.85).aspx )
		struct sDdsHeader
		{
			uint32_t structSize;
			uint32_t flags;
			uint32_t height;
			uint32_t width;
			uint32_t pitchOrLinearSize;
			uint32_t depth;
			uint32_t mipMapCount;
			uint32_t reserved1[11];
			struct
			{
				uint32_t structSize;
				uint32_t flags;
				uint8_t fourCc[4];
				uint32_t rgbBitCount;
				uint32_t bitMask_red;
				uint32_t bitMask_green;
				uint32_t bitMask_blue;
				uint32_t bitMask_alpha;
			} pixelFormat;
			uint32_t caps[4];
			uint32_t reserved2;
		};
		const sDdsHeader* ddsHeader = reinterpret_cast<const sDdsHeader*>(currentPosition);
		currentPosition += sizeof(sDdsHeader);
		// Convert the DDS format into an OpenGL format
		GLenum format;
		{
			// This code can only handle the two basic formats that the example TextureBuilder will create.
			// If a DDS in a different format is provided to TextureBuilder it will be passed through unchanged
			// and this code won't work.
			// Similarly, if you improve the example TextureBuilder to support more formats
			// you will also have to update this code to support them.
			const uint8_t fourCc_dxt1[] = { 'D', 'X', 'T', '1' };	// No alpha channel
			const uint8_t fourCc_dxt5[] = { 'D', 'X', 'T', '5' };	// Alpha channel
			const uint32_t fourCc_texture = *reinterpret_cast<const uint32_t*>(ddsHeader->pixelFormat.fourCc);
			if (fourCc_texture == *reinterpret_cast<const uint32_t*>(fourCc_dxt1))
			{
				format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			}
			else if (fourCc_texture == *reinterpret_cast<const uint32_t*>(fourCc_dxt5))
			{
				format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			}
			else
			{
				wereThereErrors = true;
				if (o_errorMessage)
				{
					char fourCcString[5] = { 0 };	// Add NULL terminator
					memcpy(fourCcString, ddsHeader->pixelFormat.fourCc, 4);
					std::stringstream errorMessage;
					errorMessage << "The texture file \"" << i_path << "\" has an unsupported format \"" << fourCcString << "\"";
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
		}
		// Iterate through each MIP map level and fill in the OpenGL texture
		{
			GLsizei currentWidth = ddsHeader->width;
			GLsizei currentHeight = ddsHeader->height;
			const GLsizei blockSize = format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ? 8 : 16;
			const GLint borderWidth = 0;
			for (uint32_t mipMapLevel = 0; mipMapLevel < ddsHeader->mipMapCount; ++mipMapLevel)
			{
				const GLsizei mipMapSize = ((currentWidth + 3) / 4) * ((currentHeight + 3) / 4) * blockSize;
				glCompressedTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(mipMapLevel), format, currentWidth, currentHeight,
					borderWidth, mipMapSize, currentPosition);
				const GLenum errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					currentPosition += static_cast<size_t>(mipMapSize);
					currentWidth = std::max(1, (currentWidth / 2));
					currentHeight = std::max(1, (currentHeight / 2));
				}
				else
				{
					wereThereErrors = true;
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "OpenGL rejected compressed texture data: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						*o_errorMessage = errorMessage.str();
					}
					goto OnExit;
				}
			}
		}

		assert(currentPosition == (reinterpret_cast<uint8_t*>(fileContents) + fileSize));

	OnExit:

		if (fileContents != NULL)
		{
			free(fileContents);
			fileContents = NULL;
		}
		if (fileHandle != INVALID_HANDLE_VALUE)
		{
			if (CloseHandle(fileHandle) == FALSE)
			{
				wereThereErrors = true;
				if (o_errorMessage)
				{
					std::string windowsErrorMessage(GetLastWindowsError());
					std::stringstream errorMessage;
					errorMessage << "\nWindows failed to close the texture file handle: " << windowsErrorMessage;
					*o_errorMessage += errorMessage.str();
				}
			}
			fileHandle = INVALID_HANDLE_VALUE;
		}
		if (wereThereErrors && (o_texture != 0))
		{
			const GLsizei textureCount = 1;
			glDeleteTextures(textureCount, &o_texture);
			assert(glGetError == GL_NO_ERROR);
			o_texture = 0;
		}

		return !wereThereErrors;
	}
#endif
}