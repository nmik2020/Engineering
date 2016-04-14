/*
This file contains the function declarations for graphics
*/

#ifndef EAE6320_USER_OUTPUT_H
#define EAE6320_USER_OUTPUT_H

// Header Files
//=============

#include <string>
#include "../Windows/Includes.h"
// Interface
//==========

namespace eae6320
{
	namespace UserOutput
	{
		//void Print(std::string output) { OutputDebugStringA(output.c_str()); }
		void Print(std::string output);
		//MessageBox()
	}
}

#endif	// EAE6320_GRAPHICS_H
