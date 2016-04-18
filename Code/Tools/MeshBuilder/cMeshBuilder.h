/*
	This is an example builder program that just copies the source file to the target path
	(as opposed to actually "building" it in some way so that the target is different than the source)
*/

#ifndef EAE6320_CMESHBUILDER_H
#define EAE6320_CMESHBUILDER_H

// Header Files
//=============

#include "../BuilderHelper/cbBuilder.h"
#include "../../External/Lua/Includes.h"
#include <sstream>
#include "../../Engine/Windows/Functions.h"
#include "../../Engine/UserOutput/UserOutput.h"
#include <cassert>
#include <iostream>
#include "../../Engine/Graphics/Triangle.h"
// Class Declaration
//==================

namespace eae6320
{
	class cMeshBuilder : public cbBuilder
	{

		struct s_vertex
		{
		public:
			float x, y, z;
			float u, v;
			float nx, ny, nz; // Normals
#if defined( EAE6320_PLATFORM_D3D )
			uint8_t b, g, r, a;

#elif defined( EAE6320_PLATFORM_GL )
			uint8_t r, g, b, a;
#endif

		public:
			void setVertex(float xValue, float yValue, float zValue)
			{
				x = xValue;
				y = yValue;
				z = zValue;
			}

			void setNormals(float xValue, float yValue, float zValue)
			{
				nx = xValue;
				ny = yValue;
				nz = zValue;
			}
			void setColor(float red, float green, float blue, float alpha)
			{
				r = static_cast<uint8_t>(red * 255.0f);
				g = static_cast<uint8_t>(green * 255.0f);
				b = static_cast<uint8_t>(blue * 255.0f);
				a = static_cast<uint8_t>(alpha * 255.0f);
			}
			void setUV(float textureU, float textureV)
			{
				u = textureU;
				v = 1 - textureV;
			}
		};

		// Interface
		//==========

	public:

		// Build
		//------
		std::vector<Triangle> triangleList;
		s_vertex* vertexData;
		uint32_t vertexCount;
		uint32_t indexCount;
		uint32_t *indexData;
		virtual bool Build( const std::vector<std::string>& i_arguments );
		bool LoadTableValues(lua_State& io_luaState);
		bool LoadMesh();
		bool LoadAsset(const char* i_path);
		void LoadColorValues(lua_State& io_luaState, int& value);
		void LoadPositionCoordinates(lua_State& io_luaState,int& value);
		void LoadTextureCoordinates(lua_State& io_luaState, int& value);
		void LoadNormalCoordinates(lua_State& io_luaState, int& value);

		bool LoadTableValues_vertices(lua_State& io_luaState);
		bool LoadTableValues_vertices_paths(lua_State& io_luaState);
		bool LoadTableValue_indices_paths(lua_State& io_luaState);
		bool LoadTableValues_indices(lua_State& io_luaState);

		void writeToBinaryFile();
		void fillTriangles();
	};
}

#endif	
