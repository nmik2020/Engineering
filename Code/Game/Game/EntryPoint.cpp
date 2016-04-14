/*
	Unlike a standard C/C++ program,
	WinMain() is the function where Windows programs start execution
*/

// Header Files
//=============
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "WindowsProgram.h"
#include "Windows/WindowsIncludes.h"






int WINAPI WinMain(HINSTANCE i_thisInstanceOfTheProgram,HINSTANCE,char* i_commandLineArguments,int i_initialWindowDisplayState )
{	
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//runNetwork();
	bool isServer = false;
	if (i_commandLineArguments[1] == 'C' || i_commandLineArguments[1] == 'c')
		isServer = false;
	else
		isServer = true;

	OutputDebugStringA(i_commandLineArguments);

	const int exitCode = CreateMainWindowAndReturnExitCodeWhenItCloses( i_thisInstanceOfTheProgram, i_initialWindowDisplayState , isServer );
	_CrtDumpMemoryLeaks();

	return exitCode;
}
