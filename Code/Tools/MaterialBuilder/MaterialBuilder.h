

#ifndef EAE6320_MATERIALBUILDER_H
#define EAE6320_MATERIALBUILDER_H

// Header Files
//=============

#include "../BuilderHelper/cbBuilder.h"
#include "../../External/Lua/Includes.h"
#include <sstream>
#include "../../Engine/Windows/Functions.h"
#include "../../Engine/UserOutput/UserOutput.h"
#include <cassert>
#include <iostream>
#include "../../Engine/Graphics/sParameterToSet.h"
// Class Declaration
//==================

namespace eae6320
{


	class MaterialBuilder : public cbBuilder
	{

		std::string effectPath;
		std::vector<std::string> handleList;


	public:
		uint8_t uniformCount;
		uint8_t textureCount;

		sParameterToSet* uniformData;
		std::vector<std::string> texturePathList;
		std::vector<std::string> textureSamplerList;

		virtual bool Build( const std::vector<std::string>& i_arguments );
		bool LoadMaterial();
		bool LoadAsset(const char* i_path);
		bool LoadTableValues(lua_State& io_luaState);
		bool getHandleName(lua_State& io_luaState, int& value);
		bool getEffectPath(lua_State& io_luaState);
		bool getShaderType(lua_State& io_luaState, int& value);
		bool getUniforms(lua_State& io_luaState);
		bool getUniformValues(lua_State& io_luaState);
		bool LoadUniformTableValues(lua_State& io_luaState);
		void LoadValues(lua_State& io_luaState, int& value);
		bool getTextures(lua_State& io_luaState);
		bool LoadTextureTableValues(lua_State& io_luaState);
		bool getTextureSampler(lua_State& io_luaState, int& value);
		bool getTexturePath(lua_State& io_luaState, int& value);

		void writeToBinaryFile();
	};
}

#endif	
