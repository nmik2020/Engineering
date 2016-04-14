
#include "UserOutput.h"

namespace eae6320
{
	namespace UserOutput
	{

		void Print(std::string output)
		{
			OutputDebugStringA(output.c_str());
			MessageBox(NULL, output.c_str(), NULL, MB_OK | MB_ICONERROR);

		}
	}
}