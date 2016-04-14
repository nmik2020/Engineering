// Header Files
//=============

#include "../Graphics.h"

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <sstream>
#include "../../Windows/Functions.h"
#include "../Mesh.h"
#include "../Effect.h"
// Static Data Initialization
//===========================

namespace
{
	HWND s_renderingWindow = NULL;
	HGLRC s_openGlRenderingContext = NULL;
}

// Helper Function Declarations
//=============================

namespace
{
	bool CreateProgram();
	bool CreateRenderingContext();
}

// Interface
//==========
void eae6320::Graphics::Submit(std::vector<eae6320::GameObject*>& objects)
{
	gameObjects = objects;
}
bool eae6320::Graphics::Initialize(const HWND i_renderingWindow)
{


	s_renderingWindow = i_renderingWindow;

	// Create an OpenGL rendering context
	if (!CreateRenderingContext())
	{
		goto OnError;
	}

	// Load any required OpenGL extensions
	{
		std::string errorMessage;
		if (!OpenGlExtensions::Load(&errorMessage))
		{
			UserOutput::Print(errorMessage);
			goto OnError;
		}
	}
	bool success;

	//effects.Init(context);

	for (unsigned int i = 0; i < gameObjects.size(); i++)
	{
		GameObject *currentObject = gameObjects[i];
		currentObject->material->Init(context);

		//currentObject->material->effect->Init(context);

		success = currentObject->mesh->LoadMesh(context);
	}



	if (!success )
	{
		const char* errorCaption = "Missing file";
		std::string errorMessage("Windows could not find the Lua file");
		UserOutput::Print(errorMessage.c_str());
	}

	// Initialize the graphics objects
	
	if (!CreateProgram())
	{
		goto OnError;
	}

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);

	const GLenum errorCode = glGetError();
	if (errorCode != GL_NO_ERROR)
	{
		/*std::stringstream errorMessage;
		errorMessage << "OpenGL failed to create a program: " <<
		reinterpret_cast<const char*>(gluErrorString(errorCode));*/
		//eae6320::UserOutput::Print(errorMessage.str());
		//return false;
	}

	return true;

OnError:

	ShutDown();
	return false;
}


bool eae6320::Graphics::ShutDown()
{
	bool wereThereErrors = false;

	if (s_openGlRenderingContext != NULL)
	{
		/*if (s_programId != 0)
		{
			glDeleteProgram(s_programId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to delete the program: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				UserOutput::Print(errorMessage.str());
			}
			s_programId = 0;
		}*/
		//effects.Shutdown();

		if (wglMakeCurrent(context.s_deviceContext, NULL) != FALSE)
		{
			if (wglDeleteContext(s_openGlRenderingContext) == FALSE)
			{
				std::stringstream errorMessage;
				errorMessage << "Windows failed to delete the OpenGL rendering context: " << GetLastWindowsError();
				UserOutput::Print(errorMessage.str());
			}
		}
		else
		{
			std::stringstream errorMessage;
			errorMessage << "Windows failed to unset the current OpenGL rendering context: " << GetLastWindowsError();
			UserOutput::Print(errorMessage.str());
		}
		s_openGlRenderingContext = NULL;
	}

	if (context.s_deviceContext != NULL)
	{
		// The documentation says that this call isn't necessary when CS_OWNDC is used
		ReleaseDC(s_renderingWindow, context.s_deviceContext);
		context.s_deviceContext = NULL;
	}

	s_renderingWindow = NULL;

	return !wereThereErrors;
}

// Helper Function Declarations
//=============================

namespace
{
	bool CreateProgram()
	{
		// Create a program
		{
			for (unsigned int i = 0; i < gameObjects.size(); i++)
			{
				eae6320::GameObject  *currentObject = gameObjects[i];
				currentObject->material->effect->s_programId = glCreateProgram();
				//Initializing Effects...This will read from the binary file effects and make sure the effects path has the correct shader files
				//currentObject->effect->Init(context);
				// Load and attach the shaders
				if (!currentObject->material->effect->LoadVertexShader(context))
				{
					return false;
				}
				if (!currentObject->material->effect->LoadFragmentShader(context))
				{
					return false;
				}


				// Link the program
				{
					glLinkProgram(currentObject->material->effect->s_programId);
					GLenum errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						// Get link info
						// (this won't be used unless linking fails
						// but it can be useful to look at when debugging)
						std::string linkInfo;
						{
							GLint infoSize;
							glGetProgramiv(currentObject->material->effect->s_programId, GL_INFO_LOG_LENGTH, &infoSize);
							errorCode = glGetError();
							if (errorCode == GL_NO_ERROR)
							{
								eae6320::GraphicsContext::sLogInfo info(static_cast<size_t>(infoSize));
								GLsizei* dontReturnLength = NULL;
								glGetProgramInfoLog(currentObject->material->effect->s_programId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
								errorCode = glGetError();
								if (errorCode == GL_NO_ERROR)
								{
									linkInfo = info.memory;
								}
								else
								{
									std::stringstream errorMessage;
									errorMessage << "OpenGL failed to get link info of the program: " <<
										reinterpret_cast<const char*>(gluErrorString(errorCode));
									eae6320::UserOutput::Print(errorMessage.str());
									return false;
								}
							}
							else
							{
								std::stringstream errorMessage;
								errorMessage << "OpenGL failed to get the length of the program link info: " <<
									reinterpret_cast<const char*>(gluErrorString(errorCode));
								eae6320::UserOutput::Print(errorMessage.str());
								return false;
							}
						}
						// Check to see if there were link errors
						GLint didLinkingSucceed;
						{
							glGetProgramiv(currentObject->material->effect->s_programId, GL_LINK_STATUS, &didLinkingSucceed);
							errorCode = glGetError();
							if (errorCode == GL_NO_ERROR)
							{
								if (didLinkingSucceed == GL_FALSE)
								{
									std::stringstream errorMessage;
									errorMessage << "The program failed to link:\n" << linkInfo;
									eae6320::UserOutput::Print(errorMessage.str());
									return false;
								}
							}
							else
							{
								std::stringstream errorMessage;
								errorMessage << "OpenGL failed to find out if linking of the program succeeded: " <<
									reinterpret_cast<const char*>(gluErrorString(errorCode));
								eae6320::UserOutput::Print(errorMessage.str());
								return false;
							}
						}
					}
					else
					{
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to link the program: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						eae6320::UserOutput::Print(errorMessage.str());
						return false;
					}
				}


			}

			//effects.s_programId = glCreateProgram();
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				/*std::stringstream errorMessage;
				errorMessage << "OpenGL failed to create a program: " <<
				reinterpret_cast<const char*>(gluErrorString(errorCode));*/
				//eae6320::UserOutput::Print(errorMessage.str());
				//return false;
			}
			//return true;

		}

		//GLint location = glGetUniformLocation(effects.s_programId, "g_position_offset");
		//glUniform2fv(location, 1, 0);
		


		return true;
	}

	bool CreateRenderingContext()
	{
		// A "device context" can be thought of an abstraction that Windows uses
		// to represent the graphics adaptor used to display a given window
		context.s_deviceContext = GetDC(s_renderingWindow);
		if (context.s_deviceContext == NULL)
		{
			eae6320::UserOutput::Print("Windows failed to get the device context");
			return false;
		}
		// Windows requires that an OpenGL "render context" is made for the window we want to use to render into
		{
			// Set the pixel format of the rendering window
			{
				PIXELFORMATDESCRIPTOR desiredPixelFormat = { 0 };
				{
					desiredPixelFormat.nSize = sizeof(PIXELFORMATDESCRIPTOR);
					desiredPixelFormat.nVersion = 1;
					desiredPixelFormat.cDepthBits = 24;
					desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
					desiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
					desiredPixelFormat.cColorBits = 32;
					desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;
				}
				// Get the ID of the desired pixel format
				int pixelFormatId;
				{
					pixelFormatId = ChoosePixelFormat(context.s_deviceContext, &desiredPixelFormat);
					if (pixelFormatId == 0)
					{
						std::stringstream errorMessage;
						errorMessage << "Windows couldn't choose the closest pixel format: " << eae6320::GetLastWindowsError();
						eae6320::UserOutput::Print(errorMessage.str());
						return false;
					}
				}
				// Set it
				if (SetPixelFormat(context.s_deviceContext, pixelFormatId, &desiredPixelFormat) == FALSE)
				{
					std::stringstream errorMessage;
					errorMessage << "Windows couldn't set the desired pixel format: " << eae6320::GetLastWindowsError();
					eae6320::UserOutput::Print(errorMessage.str());
					return false;
				}
			}
			// Create the OpenGL rendering context
			s_openGlRenderingContext = wglCreateContext(context.s_deviceContext);
			if (s_openGlRenderingContext == NULL)
			{
				std::stringstream errorMessage;
				errorMessage << "Windows failed to create an OpenGL rendering context: " << eae6320::GetLastWindowsError();
				eae6320::UserOutput::Print(errorMessage.str());
				return false;
			}
			// Set it as the rendering context of this thread
			if (wglMakeCurrent(context.s_deviceContext, s_openGlRenderingContext) == FALSE)
			{
				std::stringstream errorMessage;
				errorMessage << "Windows failed to set the current OpenGL rendering context: " << eae6320::GetLastWindowsError();
				eae6320::UserOutput::Print(errorMessage.str());
				return false;
			}
		}

		return true;
	}

	
	
}
