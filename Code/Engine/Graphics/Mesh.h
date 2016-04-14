#ifndef MESH_H
#define MESH_H

#include "../Windows/Includes.h"
#if defined( EAE6320_PLATFORM_D3D )
#include <d3d9.h>
#elif defined( EAE6320_PLATFORM_GL )
#include <gl/GLU.h>
#include <gl/GL.h>
#endif
#include <cstdint>
#include "../UserOutput/UserOutput.h"
#include <cassert>
#include <iostream>
#include "../Graphics/GraphicsContext.h"
#include "../Graphics/s_vertex.h"

namespace eae6320
{
	class Mesh
	{
		
	public:
		char *path;
		uint32_t numberOfIndices;
		uint32_t numberOfVertices;
		eae6320::s_vertex* vertices;
		uint32_t* indices;
#if defined( EAE6320_PLATFORM_D3D )
		IDirect3DVertexDeclaration9* s_vertexDeclaration = NULL;
		IDirect3DVertexBuffer9* s_vertexBuffer = NULL;
		IDirect3DIndexBuffer9* s_indexBuffer = NULL;
#elif defined( EAE6320_PLATFORM_GL )
		GLuint s_vertexArrayId = 0;
#endif


	public:
		void DrawMesh(GraphicsContext& i_context);
		bool LoadMesh(GraphicsContext& i_context);
		bool ReadFromBinaryFile(const char * path, GraphicsContext& i_context);

		bool LoadAsset(const char* i_path, GraphicsContext& i_context);
#if defined( EAE6320_PLATFORM_D3D )
		bool CreateVertexBuffer(GraphicsContext& i_context);
		bool CreateIndexBuffer(GraphicsContext& i_context);
		HRESULT Mesh::GetVertexProcessingUsage(DWORD& o_usage, GraphicsContext& i_context);
#elif defined( EAE6320_PLATFORM_GL )
		bool Mesh::CreateVertexArray(GraphicsContext& i_context);
#endif
		void ShutDown(GraphicsContext& i_context);




	};









}


#endif


