// Header Files
//=============

#include "cEffectBuilder.h"

#include <fstream>

#define ALPHA_TRANSPARENCY 0
#define DEPTH_TESTING 1
#define DEPTH_WRITE 2
#define FACE_CULING 3

// Interface
//==========

// Build
//------

bool eae6320::EffectBuilder::Build( const std::vector<std::string>& )
{
	bool wereThereErrors = false;

	// Copy the source to the target
	{
		if(LoadEffect())
		{
			writeToBinaryFile();
			return true;

		}
		wereThereErrors = true;
	}
	
	return !wereThereErrors;
}

bool  eae6320::EffectBuilder::LoadEffect()
{

	if (! eae6320::EffectBuilder::LoadAsset(m_path_source))
	{
		return false;
	}
	return true;
}

bool  eae6320::EffectBuilder::LoadTableValues(lua_State& io_luaState )
{
	

	bool success = setVertexShaderPath(io_luaState)&& setFragmentShaderPath(io_luaState) 
		&& setAlphaTransparency(io_luaState) && setDepthTesting(io_luaState) 
		&& setDepthWriting(io_luaState);


	return success;
}
bool eae6320::EffectBuilder::setVertexShaderPath(lua_State& io_luaState)
{
	// Get the value of "name"
	{
		// Remember in Lua you access a table's value like this:
		//	v = t[k]
		// where t is the table, k is the key, and v is the value

		// In C/C++ code you access a table's value like this pseudocode:
		//	lua_push( lua_State*, k );
		//	lua_gettable( lua_State*, t );
		//	v = lua_tovalue( lua_State*, -1 );
		// where t is the _index_ of the table.
		// In words, you:
		//	* Push a table on the stack
		//	* Push the key on the top of the stack
		//	* Call lua_gettable(), providing the index of the table
		//		(the index of the key is required to be -1)
		//	* lua_gettable() will pop the key, and push the value.
		//		In other words, everything on the stack will be as it was before the call
		//		except the key will have been replaced with a value
		//		(which will be nil if you asked for a key that doesn't exist in the table).

		// Let's do it for real, and be clear at every step what the stack looks like:

		// Right now, the stack only has _one_ object,
		// which is the table from the asset file, at index -1.
		// We want to look up the value associated with the key "name":
		const char* const key = "vertexShaderPath";
		lua_pushstring(&io_luaState, key);

		// Now, the key is at index -1
		// and the table is at index -2.
		// (Does that make sense why the table is at -2?
		// I think of it like "the table was on top,
		// but when I pushed something new on the top it got pushed down".)

		// We can now call lua_gettable() to get the value associated
		// with the key at index -1:
		{
			const int currentIndexOfTable = -2;
			lua_gettable(&io_luaState, currentIndexOfTable);
		}

		// Now, the value that was at t[k] is at index -1
		// and the table is still at index -2.

		// The following situations are possible:
		//	* There _was_ a value at the key
		//		* It may be the value type that we expect (a string, in this example)
		//		* It may be a different value type (like a boolean, in our current example)
		//	* There was no value at the key

		// It is up to you how explicit you want to make your error checking.
		// In general, writing more error checking code is annoying to do
		// and makes the code harder to read,
		// but it makes life easier for the users of your software
		// (and for you, in cases where something that "should never happen" does.

		// In this first example we will do as much error checking as we can,
		// and then in the next example we will not do any.
		// This will allow you to see both and decide how much you personally prefer.

		// First, then, we will make sure that a value (_any_ value) existed for the key:
		if (lua_isnil(&io_luaState, -1))
		{
			std::cerr << "No value for \"" << key << "\" was found in the asset table"
				"\n";
			// When using Lua in C/C++ it is important
			// to always return the stack to its original state.
			// Leaving objects on the stack is kind of like a Lua memory leak.
			// Remember that our value is currently at -1 and the table is at -2;
			// In order to restore it to the way it was when we entered this function
			// we need to pop the value off the stack:
			lua_pop(&io_luaState, 1);
			// Now the only thing on the stack is the asset table at -1,
			// and the calling function will deal with this
			// (regardless of whether this function succeeds or fails).
			return false;
		}

		// If we reach this code then we know that a value exists,
		// but we don't know if it's the right type or not.
		// One way we could find out in the current example is this:
		//	if ( lua_isstring( &io_luaState, -1 ) )
		// This would work (and is perfectly valid),
		// but it actually returns true if the value is a number
		// (because numbers are always convertible to strings).
		// If we really want to be strict, we can do the following instead:
		if (lua_type(&io_luaState, -1) != LUA_TSTRING)
		{
			std::cerr << "The value for \"" << key << "\" must be a string "
				"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
			// Pop the value
			lua_pop(&io_luaState, 1);
			// (The asset table is now at -1)
			return false;
		}

		// Now we know that the value stored in the table at the key is valid:
		{
			vertexShaderPath = lua_tostring(&io_luaState, -1);
			// You can now do whatever you want with the value.
			// NOTE! The string is a pointer to a char array,
			// which means that Lua owns the memory.
			// You should do whatever you need with the value
			// _before_ popping the value from the stack.
			// If you can't use the value right away,
			// then either copy it to your own char array or store it in a std::string.
			std::cout << "The value for \"" << key << "\" is \"" << vertexShaderPath << "\"\n";
			// Pop the value
			lua_pop(&io_luaState, 1);
			// (The asset table is now at -1)
			return true;
		}
	}

}

bool eae6320::EffectBuilder::setFragmentShaderPath(lua_State& io_luaState)
{
	{

		const char* const key = "fragmentShaderPath";
		lua_pushstring(&io_luaState, key);

		{
			const int currentIndexOfTable = -2;
			lua_gettable(&io_luaState, currentIndexOfTable);
		}


		if (lua_isnil(&io_luaState, -1))
		{
			std::cerr << "No value for \"" << key << "\" was found in the asset table"
				"\n";
			lua_pop(&io_luaState, 1);
			return false;
		}


		if (lua_type(&io_luaState, -1) != LUA_TSTRING)
		{
			std::cerr << "The value for \"" << key << "\" must be a string "
				"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
			lua_pop(&io_luaState, 1);
			return false;
		}

		{
			fragmentShaderPath = lua_tostring(&io_luaState, -1);
			std::cout << "The value for \"" << key << "\" is \"" << fragmentShaderPath << "\"\n";
			lua_pop(&io_luaState, 1);
			return true;
		}
	}
}

bool eae6320::EffectBuilder::setAlphaTransparency(lua_State& io_luaState)
{

		const char* const key = "alphaTransparency";
		lua_pushstring(&io_luaState, key);

		{
			const int currentIndexOfTable = -2;
			lua_gettable(&io_luaState, currentIndexOfTable);
		}


		if (lua_isnil(&io_luaState, -1))
		{
			std::cerr << "No value for \"" << key << "\" was found in the asset table"
				"\n";
			lua_pop(&io_luaState, 1);
			return false;
		}


		if (lua_type(&io_luaState, -1) != LUA_TBOOLEAN)
		{
			std::cerr << "The value for \"" << key << "\" must be a boolean "
				"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
			lua_pop(&io_luaState, 1);
			return false;
		}

		{
			renderStates |= (lua_toboolean(&io_luaState, -1) << ALPHA_TRANSPARENCY);
			std::cout << "The value for \"" << key << "\" is \"" << (renderStates << ALPHA_TRANSPARENCY) <<"\"\n";
			lua_pop(&io_luaState, 1);
			return true;
		}
}

bool eae6320::EffectBuilder::setDepthTesting(lua_State& io_luaState)
{

	const char* const key = "depthTesting";
	lua_pushstring(&io_luaState, key);

	{
		const int currentIndexOfTable = -2;
		lua_gettable(&io_luaState, currentIndexOfTable);
	}


	if (lua_isnil(&io_luaState, -1))
	{
		std::cerr << "No value for \"" << key << "\" was found in the asset table"
			"\n";
		lua_pop(&io_luaState, 1);
		return false;
	}


	if (lua_type(&io_luaState, -1) != LUA_TBOOLEAN)
	{
		std::cerr << "The value for \"" << key << "\" must be a boolean "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
		lua_pop(&io_luaState, 1);
		return false;
	}

	{
		renderStates |= (lua_toboolean(&io_luaState, -1) << DEPTH_TESTING);
		std::cout << "The value for \"" << key << "\" is \"" << (renderStates) << "\"\n";
		lua_pop(&io_luaState, 1);
		return true;
	}
}


bool eae6320::EffectBuilder::setDepthWriting(lua_State& io_luaState)
{

	const char* const key = "depthWriting";
	lua_pushstring(&io_luaState, key);

	{
		const int currentIndexOfTable = -2;
		lua_gettable(&io_luaState, currentIndexOfTable);
	}


	if (lua_isnil(&io_luaState, -1))
	{
		std::cerr << "No value for \"" << key << "\" was found in the asset table"
			"\n";
		lua_pop(&io_luaState, 1);
		return false;
	}


	if (lua_type(&io_luaState, -1) != LUA_TBOOLEAN)
	{
		std::cerr << "The value for \"" << key << "\" must be a boolean "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
		lua_pop(&io_luaState, 1);
		return false;
	}

	{
		renderStates |= (lua_toboolean(&io_luaState, -1) << DEPTH_WRITE);
		std::cout << "The value for \"" << key << "\" is \"" << unsigned(renderStates ) << "\"\n";
		lua_pop(&io_luaState, 1);
		return true;
	}
}

bool eae6320::EffectBuilder::setFaceCuling(lua_State& io_luaState)
{

	const char* const key = "faceCuling";
	lua_pushstring(&io_luaState, key);

	{
		const int currentIndexOfTable = -2;
		lua_gettable(&io_luaState, currentIndexOfTable);
	}


	if (lua_isnil(&io_luaState, -1))
	{
		std::cerr << "No value for \"" << key << "\" was found in the asset table"
			"\n";
		lua_pop(&io_luaState, 1);
		return false;
	}


	if (lua_type(&io_luaState, -1) != LUA_TBOOLEAN)
	{
		std::cerr << "The value for \"" << key << "\" must be a boolean "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
		lua_pop(&io_luaState, 1);
		return false;
	}

	{
		renderStates |= (lua_toboolean(&io_luaState, -1) << FACE_CULING);
		std::cout << "The value for \"" << key << "\" is \"" << unsigned(renderStates) << "\"\n";
		lua_pop(&io_luaState, 1);
		return true;
	}
}

bool  eae6320::EffectBuilder::LoadAsset(const char* i_path )
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

	if (luaState)
	{
		// If I haven't made any mistakes
		// there shouldn't be anything on the stack,
		// regardless of any errors encountered while loading the file:
		assert(lua_gettop(luaState) == 0);

		lua_close(luaState);
		luaState = NULL;
	}

	return !wereThereErrors;
}

void  eae6320::EffectBuilder::writeToBinaryFile()
{
	uint8_t length = static_cast<uint8_t>(vertexShaderPath.length());
	const char * c = m_path_target;
	std::string str(m_path_target);
	std::ofstream ofile(m_path_target, std::ios::binary);
	/* Hex Format
	Render State - Length of Vertex Shader Path -
	Vertex Shader Path - Fragment Shader Path
	*/
	//Store Render States
	ofile.write((char*)&renderStates, sizeof(uint8_t));
	//Store Length of Vertex Shader Path
	ofile.write((char*)&length, sizeof(uint8_t));
	//Store Vertex Shader Path..+1 to account for null string
	ofile.write(vertexShaderPath.c_str(), vertexShaderPath.size() + sizeof(uint8_t));
	//Store Fragment Shader Path..+1 to account for null string
	ofile.write(fragmentShaderPath.c_str(), fragmentShaderPath.size() + sizeof(uint8_t));


}



