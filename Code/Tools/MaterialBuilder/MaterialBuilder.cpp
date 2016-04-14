// Header Files
//=============

#include "MaterialBuilder.h"

#include <fstream>

#define ALPHA_TRANSPARENCY 0
#define DEPTH_TESTING 1
#define DEPTH_WRITE 2
#define FACE_CULING 3

// Interface
//==========

// Build
//------

bool eae6320::MaterialBuilder::Build( const std::vector<std::string>& )
{
	bool wereThereErrors = false;

	// Copy the source to the target
	{
		if(LoadMaterial())
		{
			writeToBinaryFile();
			return true;

		}
		wereThereErrors = true;
	}
	
	return !wereThereErrors;
}

bool  eae6320::MaterialBuilder::LoadMaterial()
{

	if (! eae6320::MaterialBuilder::LoadAsset(m_path_source))
	{
		return false;
	}
	return true;
}

bool  eae6320::MaterialBuilder::LoadTableValues(lua_State& io_luaState )
{
	

	bool success =  getEffectPath(io_luaState) && getUniforms(io_luaState) && getTextures(io_luaState)
		;

	return success;

}

bool eae6320::MaterialBuilder::getUniforms(lua_State& io_luaState)
{
	bool wereThereErrors = false;
	const char* key1 = "Uniform";
	lua_pushstring(&io_luaState, key1);
	lua_gettable(&io_luaState, -2);

	if (lua_istable(&io_luaState, -1))
	{
		if (!LoadUniformTableValues(io_luaState))
		{
			wereThereErrors = true;
			goto OnExit;
		}
	}
	else
	{
		wereThereErrors = true;
		std::cerr << "The value at \"" << key1 << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
		goto OnExit;
	}

OnExit:

	// Pop the textures table
	lua_pop(&io_luaState, 1);

	return !wereThereErrors;

}


bool eae6320::MaterialBuilder::getTextures(lua_State& io_luaState)
{
	bool wereThereErrors = false;
	const char*  key2 = "Texture";
	lua_pushstring(&io_luaState, key2);
	lua_gettable(&io_luaState, -2);

	if (lua_istable(&io_luaState, -1))
	{
		if (!LoadTextureTableValues(io_luaState))
		{
			wereThereErrors = true;
			goto OnExit;
		}
	}
	else
	{
		wereThereErrors = true;
		std::cerr << "The value at \"" << key2 << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
		goto OnExit;
	}

OnExit:

	// Pop the textures table
	lua_pop(&io_luaState, 1);

	return !wereThereErrors;

}

bool eae6320::MaterialBuilder::getEffectPath(lua_State& io_luaState)
{
	{
		const char* key3 = "effectPath";
		lua_pushstring(&io_luaState, key3);

		
		{
			const int currentIndexOfTable = -2;
			lua_gettable(&io_luaState, currentIndexOfTable);
		}

		if (lua_isnil(&io_luaState, -1))
		{
			std::cerr << "No value for \"" << key3 << "\" was found in the asset table"
				"\n";
			lua_pop(&io_luaState, 1);
			return false;
		}


		if (lua_type(&io_luaState, -1) != LUA_TSTRING)
		{
			std::cerr << "The value for \"" << key3 << "\" must be a string "
				"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
			lua_pop(&io_luaState, 1);
			return false;
		}

		{
			effectPath = lua_tostring(&io_luaState, -1);
			std::cout << "The value for \"" << key3 << "\" is \"" << effectPath << "\"\n";
			lua_pop(&io_luaState, 1);
			return true;
		}
	}

}

bool eae6320::MaterialBuilder::getHandleName(lua_State& io_luaState, int& value)
{
	{

		const char* key4 = "handleName";
		lua_pushstring(&io_luaState, key4);

		{
			const int currentIndexOfTable = -2;
			lua_gettable(&io_luaState, currentIndexOfTable);
		}


		if (lua_isnil(&io_luaState, -1))
		{
			std::cerr << "No value for \"" << key4 << "\" was found in the asset table"
				"\n";
			lua_pop(&io_luaState, 1);
			return false;
		}


		if (lua_type(&io_luaState, -1) != LUA_TSTRING)
		{
			std::cerr << "The value for \"" << key4 << "\" must be a string "
				"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
			lua_pop(&io_luaState, 1);
			return false;
		}

		{
			/*uniformData[value].uniformHandle = lua_tostring(&io_luaState, -1);*/
			handleList.push_back(lua_tostring(&io_luaState, -1));
			//std::cout << "The value for \"" << key << "\" is \"" << uniformData[value].uniformHandle << "\"\n";
			lua_pop(&io_luaState, 1);
			return true;
		}
	}
}

bool eae6320::MaterialBuilder::getShaderType(lua_State& io_luaState, int& value)
{

	{

		const char* key5 = "shaderType";
		lua_pushstring(&io_luaState, key5);

		{
			const int currentIndexOfTable = -2;
			lua_gettable(&io_luaState, currentIndexOfTable);
		}


		if (lua_isnil(&io_luaState, -1))
		{
			std::cerr << "No value for \"" << key5 << "\" was found in the asset table"
				"\n";
			lua_pop(&io_luaState, 1);
			return false;
		}


		if (!lua_isnumber(&io_luaState, -1))
		{
			std::cerr << "The value for \"" << key5 << "\" must be a string "
				"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
			lua_pop(&io_luaState, 1);
			return false;
		}

		{
			int i = lua_tointeger(&io_luaState, -1);
			switch (i)
			{
			case 0:
				uniformData[value].shaderType = Graphics::ShaderTypes::Vertex ;
				break;
			case 1:
				uniformData[value].shaderType = Graphics::ShaderTypes::Fragment;

				break;
			}
			//uniformData[value].shaderType = lua_tonumber(&io_luaState, -1);
			std::cout << "The value for \"" << key5 << "\" is \"" << uniformData[value].shaderType << "\"\n";
			lua_pop(&io_luaState, 1);
			return true;
		}
	}
}

bool  eae6320::MaterialBuilder::LoadUniformTableValues(lua_State& io_luaState)
{
	std::cout << "Iterating through every Unifrom path:"
		"\n";
	uniformCount = luaL_len(&io_luaState, -1);
	uniformData = new eae6320::sParameterToSet[uniformCount];
	uniformData[0].uniformHandle = 0;

	int value = 0;
	int a = 0, b = 0, c = 0;
	for (uint8_t i = 1; i <= uniformCount; i++)
	{
		//Goes inside each table in Vertices
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		getHandleName(io_luaState, value);
		getShaderType(io_luaState, value);
		LoadValues(io_luaState, value);
		//uniformData[i].uniformHandle = 0;
		++value;
		lua_pop(&io_luaState, 1);
	}
	return true;
}


bool  eae6320::MaterialBuilder::LoadTextureTableValues(lua_State& io_luaState)
{
	std::cout << "Iterating through every Texture path:"
		"\n";
	textureCount = luaL_len(&io_luaState, -1);
	//uniformData = new eae6320::sParameterToSet[uniformCount];
	//uniformData[0].uniformHandle = 0;

	int value = 0;
	int a = 0, b = 0, c = 0;
	for (uint8_t i = 1; i <= textureCount; i++)
	{
		//Goes inside each table in Vertices
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		getTextureSampler(io_luaState, value);
		getTexturePath(io_luaState, value);
		++value;
		lua_pop(&io_luaState, 1);
	}
	return true;
}

bool eae6320::MaterialBuilder::getTextureSampler(lua_State& io_luaState, int& value)
{
	{

		const char* key6 = "sampler";
		lua_pushstring(&io_luaState, key6);

		{
			const int currentIndexOfTable = -2;
			lua_gettable(&io_luaState, currentIndexOfTable);
		}


		if (lua_isnil(&io_luaState, -1))
		{
			std::cerr << "No value for \"" << key6 << "\" was found in the asset table"
				"\n";
			lua_pop(&io_luaState, 1);
			return false;
		}


		if (lua_type(&io_luaState, -1) != LUA_TSTRING)
		{
			std::cerr << "The value for \"" << key6 << "\" must be a string "
				"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
			lua_pop(&io_luaState, 1);
			return false;
		}

		{
			/*uniformData[value].uniformHandle = lua_tostring(&io_luaState, -1);*/
			textureSamplerList.push_back(lua_tostring(&io_luaState, -1));
			//std::cout << "The value for \"" << key << "\" is \"" << uniformData[value].uniformHandle << "\"\n";
			lua_pop(&io_luaState, 1);
			return true;
		}
	}
}

bool eae6320::MaterialBuilder::getTexturePath(lua_State& io_luaState, int& value)
{
	{

		const char* key7 = "texturePath";
		lua_pushstring(&io_luaState, key7);

		{
			const int currentIndexOfTable = -2;
			lua_gettable(&io_luaState, currentIndexOfTable);
		}


		if (lua_isnil(&io_luaState, -1))
		{
			std::cerr << "No value for \"" << key7 << "\" was found in the asset table"
				"\n";
			lua_pop(&io_luaState, 1);
			return false;
		}


		if (lua_type(&io_luaState, -1) != LUA_TSTRING)
		{
			std::cerr << "The value for \"" << key7 << "\" must be a string "
				"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
			lua_pop(&io_luaState, 1);
			return false;
		}

		{
			/*uniformData[value].uniformHandle = lua_tostring(&io_luaState, -1);*/
			texturePathList.push_back(lua_tostring(&io_luaState, -1));
			//std::cout << "The value for \"" << key << "\" is \"" << uniformData[value].uniformHandle << "\"\n";
			lua_pop(&io_luaState, 1);
			return true;
		}
	}
}





void eae6320::MaterialBuilder::LoadValues(lua_State& io_luaState, int& value)
{
	float a = 0, b = 0, c = 0, d = 0;
	int index;
	if (lua_istable(&io_luaState, -1))
	{

		{//Each Position Table
			const char*  key8 = "values";
			lua_pushstring(&io_luaState, key8);
			lua_gettable(&io_luaState, -2);
			if (lua_istable(&io_luaState, -1))
			{
				index = 1;
				lua_pushnil(&io_luaState);

				while (lua_next(&io_luaState, -2))
				{
					if (index == 1)
						a = (float)lua_tonumber(&io_luaState, -1);
					else if (index == 2)
						b = (float)lua_tonumber(&io_luaState, -1);
					else if (index == 3)
						c = (float)lua_tonumber(&io_luaState, -1);
				/*	else
						d = (float)lua_tonumber(&io_luaState, -1);*/
					lua_pop(&io_luaState, 1);
					index++;
				}
			}


		}
	}
	lua_pop(&io_luaState, 1);
	uniformData[value].valueCountToSet = index - 1;
	uniformData[value].setUniformValues(a, b, c);
}

bool eae6320::MaterialBuilder::getUniformValues(lua_State& io_luaState)
{

	bool wereThereErrors = false;
	const char* key9 = "values";
	lua_pushstring(&io_luaState, key9);
	lua_gettable(&io_luaState, -2);

	if (lua_istable(&io_luaState, -1))
	{
		if (!LoadUniformTableValues(io_luaState))
		{
			wereThereErrors = true;
			goto OnExit;
		}
	}
	else
	{
		wereThereErrors = true;
		std::cerr << "The value at \"" << key9 << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
		goto OnExit;
	}

OnExit:

	// Pop the textures table
	lua_pop(&io_luaState, 1);

	return !wereThereErrors;
}






bool  eae6320::MaterialBuilder::LoadAsset(const char* i_path )
{
	bool wereThereErrors = false;

	// Create a new Lua state
	lua_State* luaState = NULL;
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			wereThereErrors = true;
			std::cerr << "Failed to create a new Lua state"
				"\n";
			goto OnExit;
		}
	}

	// Load the asset file as a "chunk",
	// meaning there will be a callable function at the top of the stack
	{
		const int luaResult = luaL_loadfile(luaState, i_path);
		if (luaResult != LUA_OK)
		{
			wereThereErrors = true;
			eae6320::UserOutput::Print(lua_tostring(luaState, -1));
			OutputErrorMessage(lua_tostring(luaState, -1));
			lua_pop(luaState, 1);
			goto OnExit;
		}
	}
	// Execute the "chunk", which should load the asset
	// into a table at the top of the stack
	{
		const int argumentCount = 0;
		const int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
		const int noMessageHandler = 0;
		const int luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
		if (luaResult == LUA_OK)
		{
			// A well-behaved asset file will only return a single value
			const int returnedValueCount = lua_gettop(luaState);
			if (returnedValueCount == 1)
			{
				// A correct asset file _must_ return a table
				if (!lua_istable(luaState, -1))
				{
					wereThereErrors = true;
					std::cerr << "Asset files must return a table (instead of a " <<
						luaL_typename(luaState, -1) << ")\n";
					// Pop the returned non-table value
					lua_pop(luaState, 1);
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				std::cerr << "Asset files must return a single table (instead of " <<
					returnedValueCount << " values)"
					"\n";
				// Pop every value that was returned
				lua_pop(luaState, returnedValueCount);
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			eae6320::UserOutput::Print(lua_tostring(luaState, -1));
			OutputErrorMessage(lua_tostring(luaState, -1));

			lua_pop(luaState, 1);
			goto OnExit;
		}
	}

	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1
	if (!LoadTableValues(*luaState))
	{
		wereThereErrors = true;
	}

	// Pop the table
	lua_pop(luaState, 1);

OnExit:
//
//	if (luaState)
//	{
//		// If I haven't made any mistakes
//		// there shouldn't be anything on the stack,
//		// regardless of any errors encountered while loading the file:
//		assert(lua_gettop(luaState) == 0);
//
//		lua_close(luaState);
//		luaState = NULL;
//	}

	return !wereThereErrors;
}

void  eae6320::MaterialBuilder::writeToBinaryFile()
{
	uint8_t length = static_cast<uint8_t>(effectPath.length());
	uint8_t numberofTextures = static_cast<uint8_t>(textureSamplerList.size());
	std::string str(m_path_target);
	std::ofstream ofile(m_path_target, std::ios::binary);
	
	/* Hex Format
	Length of Effect Path - Effect Path - 
	Size of Uniform Data - Uniform Data
	*/
	ofile.write((char*)&length, sizeof(uint8_t));
	ofile.write(effectPath.c_str(), effectPath.size() + sizeof(uint8_t));
	ofile.write((char*)&uniformCount, sizeof(uint8_t));
	ofile.write((char*)uniformData, uniformCount*sizeof(sParameterToSet));
	for (size_t i = 0; i<handleList.size(); i++)
		ofile.write(handleList[i].c_str(), handleList[i].size() + sizeof(uint8_t));
	
	ofile.write((char*)&numberofTextures, sizeof(uint8_t));

	for (uint8_t i = 0; i<numberofTextures; i++)
		ofile.write(textureSamplerList[i].c_str(), textureSamplerList[i].size() + sizeof(uint8_t));

	for (uint8_t i = 0; i<numberofTextures; i++)
		ofile.write(texturePathList[i].c_str(), texturePathList[i].size() + sizeof(uint8_t));


}



