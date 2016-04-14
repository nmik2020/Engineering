
#include "Mesh.h"
#include "../UserOutput/UserOutput.h"

#include <fstream>
namespace eae6320
{
		
		bool Mesh::LoadMesh(GraphicsContext& context)
		{


			if (!Mesh::ReadFromBinaryFile(path,context))
			{
				return false;
			}
#if defined( EAE6320_PLATFORM_D3D )
			if (!CreateVertexBuffer(context))
			{
				goto OnError;
			}
			if (!CreateIndexBuffer(context))
			{
				goto OnError;
			}
			
#elif defined( EAE6320_PLATFORM_GL )
			if (!CreateVertexArray(context))
			{
				goto OnError;
			}
#endif
			return true;

			OnError:

			ShutDown(context);
			return false;

		}
		bool Mesh::ReadFromBinaryFile(const char * path, GraphicsContext& context)
		{

			std::ifstream inputFile(path, std::ifstream::in | std::ifstream::binary);
			if (inputFile)
			{
				inputFile.seekg(0, inputFile.end);
				int length = inputFile.tellg();
				inputFile.seekg(0, inputFile.beg);

				char *buffer = new char[length];

				inputFile.read(buffer, length);

				uint32_t vertexCount = *reinterpret_cast<uint32_t*>(buffer);
				uint32_t indexCount = *reinterpret_cast<uint32_t*>(buffer + sizeof(uint32_t));
				vertices = reinterpret_cast<eae6320::s_vertex*>(buffer + sizeof(uint32_t) + sizeof(uint32_t));
				 indices = reinterpret_cast<uint32_t*>(buffer + sizeof(uint32_t) + 
					sizeof(uint32_t) + sizeof(eae6320::s_vertex)*vertexCount);

				context.vertexData = vertices;
				context.indexData = indices;
				context.numberOfIndices  = numberOfIndices   =  indexCount * 3;
				context.numberOfVertices = numberOfVertices  =  vertexCount;
			}
			return true;
		}

		

}