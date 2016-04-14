/*
	The main() function is where the program starts execution
*/

// Header Files
//=============

#include "cEffectBuilder.h"

// Entry Point
//============

int main( int i_argumentCount, char** i_arguments )

{
	//eae6320::OutputErrorMessage(i_arguments[1]);
	return eae6320::Build<eae6320::EffectBuilder>( i_arguments, i_argumentCount );
}
