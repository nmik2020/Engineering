// Header Files
//=============

#include "cMeshBuilder.h"

#include <fstream>

#include "../../Engine/Graphics/AABB.h"
#include "../../Engine/Graphics/Octree.h"

// Interface
//==========

// Build
//------

void buildOctree(int stopDepth, eae6320::TAABB& boundingBox, std::vector<eae6320::Triangle> &octantTriangleList)
{
	if (stopDepth < 0)
		return;
	//These are the 8 subdivided boxes, which will then be used to check for intersections with each primitive(triangle) in the whole mesh
	eae6320::TAABB boxes[8];
	boundingBox.splitBoundingBoxes(boxes);

	eae6320::Octree* currentNode = new eae6320::Octree();
	currentNode->m_bounds = boundingBox;
	currentNode->m_objects.insert(currentNode->m_objects.begin(), boxes, boxes + 8); // Inserting all elements of array to the vector of objects in Octree node
	
	std::vector<eae6320::Triangle> trianglesInEachOctant[8];

	//For each triangle check intersection with boxes
	for (int i = 0; i < octantTriangleList.size(); i++)
	{
		eae6320::Triangle &currentTriangleInOctant = octantTriangleList.at(i);
		for (int boxId = 0; boxId < 8; boxId++)
		{
			//Do Triangle Box intersection

			if (true)
			{
				trianglesInEachOctant[i].push_back(currentTriangleInOctant);
			}
		}
	}

	//Now each octant would have a certain number of triangles. We have to further divide each octant
	//to get the leaf octant with certain triangles

	for (int i = 0; i < 8; i++)
	{
		size_t size = trianglesInEachOctant[i].size();

		if (size)
		{
			buildOctree(stopDepth - 1, boxes[i], trianglesInEachOctant[i]);
		}
	}
}

//This will take each vertex of the mesh and fill it in as triangles
void eae6320::cMeshBuilder::fillTriangles()
{
	for (int i = 0; i < indexCount; i += 3)
	{
		int index = indexData[i];
		int index1 = indexData[i + 1];
		int index2 = indexData[i + 2];
		eae6320::Vertex vertexA =  eae6320::Math::cVector(vertexData[index].x, vertexData[index].y, vertexData[index].z);
		eae6320::Vertex vertexB =  eae6320::Math::cVector(vertexData[index1].x, vertexData[index1].y, vertexData[index1].z);
		eae6320::Vertex vertexC =  eae6320::Math::cVector(vertexData[index2].x, vertexData[index2].y, vertexData[index2].z);

		Triangle newTriangle = Triangle(vertexA, vertexB, vertexC);
		triangleList.push_back(newTriangle);

	}
}

bool eae6320::cMeshBuilder::Build( const std::vector<std::string>& )
{
	bool wereThereErrors = false;

	// Copy the source to the target
	{
		if(LoadMesh())
		{
			writeToBinaryFile();
			return true;

		}
		wereThereErrors = true;
	}
	
	return !wereThereErrors;
}

bool  eae6320::cMeshBuilder::LoadMesh()
{

	if (! eae6320::cMeshBuilder::LoadAsset(m_path_source))
	{
		return false;
	}
	return true;
}

bool  eae6320::cMeshBuilder::LoadTableValues(lua_State& io_luaState )
{
	if (!LoadTableValues_vertices(io_luaState))
	{
		return false;
	}

	if (!LoadTableValues_indices(io_luaState))
	{
		return false;
	}
	return true;
}


bool  eae6320::cMeshBuilder::LoadTableValues_indices(lua_State& io_luaState )
{
	bool wereThereErrors = false;

	const char* const key = "indices";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);


	if (lua_istable(&io_luaState, -1))
	{
		if (!LoadTableValue_indices_paths(io_luaState))
		{
			wereThereErrors = true;
			goto OnExit;
		}
	}
	else
	{
		wereThereErrors = true;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
		goto OnExit;
	}

OnExit:

	// Pop the textures table
	lua_pop(&io_luaState, 1);

	return !wereThereErrors;
}

bool  eae6320::cMeshBuilder::LoadTableValues_vertices(lua_State& io_luaState )
{
	bool wereThereErrors = false;

	const char* const key = "vertices";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	if (lua_istable(&io_luaState, -1))
	{
		if (!LoadTableValues_vertices_paths(io_luaState))
		{
			wereThereErrors = true;
			goto OnExit;
		}
	}
	else
	{
		wereThereErrors = true;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
		goto OnExit;
	}


OnExit:

	// Pop the textures table
	lua_pop(&io_luaState, 1);
	return !wereThereErrors;
}


bool  eae6320::cMeshBuilder::LoadTableValue_indices_paths(lua_State& io_luaState )
{

	std::cout << "Iterating through every indices path:"
		"\n";
	//const unsigned int triangleCount = 2;	// How many triangles does a square have?
	const unsigned int vertexCountPerTriangle = 3;
	//const unsigned int trianglesPerFace = 2;
	//const unsigned int numberOfFaces = 6;
	//indexData = new uint32_t[triangleCount * vertexCountPerTriangle * trianglesPerFace * numberOfFaces];

	indexCount = luaL_len(&io_luaState, -1);
	indexData = new uint32_t[indexCount*vertexCountPerTriangle];

	int value = 0;
	int a = 0;
	for (uint32_t i = 1; i <= indexCount; i++)
	{
		//Goes inside each table in Vertices
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		if (lua_istable(&io_luaState, -1))
		{
			lua_pushnil(&io_luaState);

			while (lua_next(&io_luaState, -2))
			{
				a = (int)lua_tonumber(&io_luaState, -1);
				indexData[value] = a;
				//indexData[value] = a;
				lua_pop(&io_luaState, 1);
				++value;

			}
		}

		lua_pop(&io_luaState, 1);
	}
	return true;
}

bool  eae6320::cMeshBuilder::LoadTableValues_vertices_paths(lua_State& io_luaState )
{

	std::cout << "Iterating through every vertex path:"
		"\n";
	vertexCount = luaL_len(&io_luaState, -1);
	vertexData = new s_vertex[vertexCount];
	int value = 0;
	int a = 0, b = 0, c = 0;
	for (uint32_t i = 1; i <= vertexCount; i++)
	{
		//Goes inside each table in Vertices
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		LoadNormalCoordinates(io_luaState, value);

		LoadPositionCoordinates(io_luaState, value);

		LoadTextureCoordinates(io_luaState, value);
		LoadColorValues(io_luaState, value);
		++value;
		lua_pop(&io_luaState, 1);
	}
	return true;
}


void  eae6320::cMeshBuilder::LoadPositionCoordinates(lua_State& io_luaState,int& value )
{
	float a = 0, b = 0, c = 0;
	if (lua_istable(&io_luaState, -1))
	{

		{//Each Position Table
			const char*  key = "position";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			if (lua_istable(&io_luaState, -1))
			{
				int index = 1;
				lua_pushnil(&io_luaState);

				while (lua_next(&io_luaState, -2))
				{
					if (index == 1)
						a = (float)lua_tonumber(&io_luaState, -1);
					else if (index== 2)
						b = (float)lua_tonumber(&io_luaState, -1);
					else
						c = (float)lua_tonumber(&io_luaState, -1);
					lua_pop(&io_luaState, 1);
					index++;
				}
			}


		}
	}
	lua_pop(&io_luaState, 1);

	vertexData[value].setVertex((float)a, (float)b, (float)c);

}

void  eae6320::cMeshBuilder::LoadTextureCoordinates(lua_State& io_luaState, int& value)
{
	float a = 0, b = 0;
	if (lua_istable(&io_luaState, -1))
	{

		{//Each Position Table
			const char*  key = "texture";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			if (lua_istable(&io_luaState, -1))
			{
				int index = 1;
				lua_pushnil(&io_luaState);

				while (lua_next(&io_luaState, -2))
				{
					if (index == 1)
						a = (float)lua_tonumber(&io_luaState, -1);
					else if (index == 2)
						b = (float)lua_tonumber(&io_luaState, -1);
					lua_pop(&io_luaState, 1);
					index++;
				}
			}


		}
	}
	lua_pop(&io_luaState, 1);

	vertexData[value].setUV(a, b);

}

void  eae6320::cMeshBuilder::LoadNormalCoordinates(lua_State& io_luaState, int& value)
{
	float a = 0, b = 0, c = 0;
	if (lua_istable(&io_luaState, -1))
	{

		{//Each Position Table
			const char*  key = "normal";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			if (lua_istable(&io_luaState, -1))
			{
				int index = 1;
				lua_pushnil(&io_luaState);

				while (lua_next(&io_luaState, -2))
				{
					if (index == 1)
						a = (float)lua_tonumber(&io_luaState, -1);
					else if (index == 2)
						b = (float)lua_tonumber(&io_luaState, -1);
					else
						c = (float)lua_tonumber(&io_luaState, -1);
					lua_pop(&io_luaState, 1);
					index++;
				}
			}


		}
	}
	lua_pop(&io_luaState, 1);

	vertexData[value].setNormals(a, b, c);

}


void  eae6320::cMeshBuilder::LoadColorValues(lua_State& io_luaState, int& value )
{
	float a = 0, b = 0, c = 0, d =0;

	const char*  key = "color";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	if (lua_istable(&io_luaState, -1))
	{

		int index = 1;
		lua_pushnil(&io_luaState);

		while (lua_next(&io_luaState, -2))
		{
			switch (index)
			{
			case 1:
				a = (float)lua_tonumber(&io_luaState, -1);
				break;
			case 2:
				b = (float)lua_tonumber(&io_luaState, -1);
				break;
			case 3:
				c = (float)lua_tonumber(&io_luaState, -1);
				break;
		/*	case 4:
				d = (float)lua_tonumber(&io_luaState, -1);
				break;*/
			}
			lua_pop(&io_luaState, 1);
			index++;
		}
	}


	//vertexData[value].setColor(a, b, c ,d);
	vertexData[value].setColor(a, b, c,1);


	lua_pop(&io_luaState, 1);
}


bool  eae6320::cMeshBuilder::LoadAsset(const char* i_path )
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

void  eae6320::cMeshBuilder::writeToBinaryFile()
{
	const char * c = m_path_target;
	std::string str(m_path_target);
	std::ofstream ofile(m_path_target, std::ios::binary);
	ofile.write((char*)&vertexCount, sizeof(uint32_t));
	ofile.write((char*)&indexCount, sizeof(uint32_t));

	ofile.write((char*)vertexData, vertexCount*sizeof(s_vertex));
	ofile.write((char*)indexData, indexCount*3*sizeof(uint32_t));
}



