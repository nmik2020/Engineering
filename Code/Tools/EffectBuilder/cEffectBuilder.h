

#ifndef EAE6320_EFFECTBUILDER_H
#define EAE6320_EFFECTBUILDER_H

// Header Files
//=============

#include "../BuilderHelper/cbBuilder.h"
#include "../../External/Lua/Includes.h"
#include <sstream>
#include "../../Engine/Windows/Functions.h"
#include "../../Engine/UserOutput/UserOutput.h"
#include <cassert>
#include <iostream>
// Class Declaration
//==================

namespace eae6320
{
	class EffectBuilder : public cbBuilder
	{

		//const char* fragmentShaderPath;
		std::string vertexShaderPath;
		std::string fragmentShaderPath;
	/*	bool isAlphaTransparencyEnabled;
		bool isDepthTestingEnabled;
		bool isDepthWritingEnabled;
		bool isFaceCulingEnabled;*/
		uint8_t renderStates = 0;
	public:

		virtual bool Build( const std::vector<std::string>& i_arguments );
		//bool LoadTableValues(lua_State& io_luaState);
		bool LoadEffect();
		bool LoadAsset(const char* i_path);
		bool LoadTableValues(lua_State& io_luaState);
		bool setAlphaTransparency(lua_State& io_luaState);
		bool setFragmentShaderPath(lua_State& io_luaState);
		bool setVertexShaderPath(lua_State& io_luaState);
		bool setDepthTesting(lua_State& io_luaState);
		bool setDepthWriting(lua_State& io_luaState);
		bool setFaceCuling(lua_State& io_luaState);

		void writeToBinaryFile();
	};
}

#endif	
