#include  "Mesh.h"
#include "Graphics.h"

namespace eae6320
{
	LPD3DXMESH meshBox;

	
	void Mesh::DrawMesh(GraphicsContext& context)
	{
		// Bind a specific vertex buffer to the device as a data source
		{
			std::string b = "data/CylinderWireFrame.lua";
			std::string b2 = "data/CylinderWireFrame2.lua";

			if (strcmp(path, b.c_str()) == 0 || strcmp(path, b2.c_str()) == 0) {
				s_direct3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

			}
			else {
				s_direct3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

			}

			// There can be multiple streams of data feeding the display adaptor at the same time
			const unsigned int streamIndex = 0;
			// It's possible to start streaming data in the middle of a vertex buffer
			const unsigned int bufferOffset = 0;
			// The "stride" defines how large a single vertex is in the stream of data
			const unsigned int bufferStride = sizeof(eae6320::s_vertex);
			HRESULT result = context.s_direct3dDevice->SetStreamSource(streamIndex, s_vertexBuffer, bufferOffset, bufferStride);
			assert(SUCCEEDED(result));
		}
		// Bind a specific index buffer to the device as a data source
		{
			HRESULT result = context.s_direct3dDevice->SetIndices( s_indexBuffer);
			assert(SUCCEEDED(result));
		}
		// Render objects from the current streams
		{
			// We are using triangles as the "primitive" type,
			// and we have defined the vertex buffer as a triangle list
			// (meaning that every triangle is defined by three vertices)
			const D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST;
			// It's possible to start rendering primitives in the middle of the stream
			const unsigned int indexOfFirstVertexToRender = 0;
			const unsigned int indexOfFirstIndexToUse = 0;
			// We are drawing a square
			const unsigned int vertexCountToRender = numberOfVertices;	//  How vertices from the vertex buffer will be used?
			const unsigned int primitiveCountToRender = numberOfIndices;	// How many triangles will be drawn?
			HRESULT result = context.s_direct3dDevice->DrawIndexedPrimitive(primitiveType,
				indexOfFirstVertexToRender, indexOfFirstVertexToRender, vertexCountToRender,
				indexOfFirstIndexToUse, primitiveCountToRender);
			assert(SUCCEEDED(result));
		}
	}

	HRESULT Mesh::GetVertexProcessingUsage(DWORD& o_usage, GraphicsContext& context)
	{
		D3DDEVICE_CREATION_PARAMETERS deviceCreationParameters;
		const HRESULT result = context.s_direct3dDevice->GetCreationParameters(&deviceCreationParameters);
		if (SUCCEEDED(result))
		{
			DWORD vertexProcessingType = deviceCreationParameters.BehaviorFlags &
				(D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING);
			o_usage = (vertexProcessingType != D3DCREATE_SOFTWARE_VERTEXPROCESSING) ? 0 : D3DUSAGE_SOFTWAREPROCESSING;
		}
		else
		{
			eae6320::UserOutput::Print("Direct3D failed to get the device's creation parameters");
		}
		return result;
	}
	bool Mesh::CreateVertexBuffer(GraphicsContext& context)
	{
		// The usage tells Direct3D how this vertex buffer will be used
		DWORD usage = 0;
		{
			// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
			const HRESULT result = GetVertexProcessingUsage(usage,context);
			if (FAILED(result))
			{
				return false;
			}
			// Our code will only ever write to the buffer
			usage |= D3DUSAGE_WRITEONLY;
		}

		// Initialize the vertex format
		{
			// These elements must match the VertexFormat::sVertex layout struct exactly.
			// They instruct Direct3D how to match the binary data in the vertex buffer
			// to the input elements in a vertex shader
			// (by using D3DDECLUSAGE enums here and semantics in the shader,
			// so that, for example, D3DDECLUSAGE_POSITION here matches with POSITION in shader code).
			// Note that OpenGL uses arbitrarily assignable number IDs to do the same thing.
			D3DVERTEXELEMENT9 vertexElements[] =
			{
				// Stream 0

				// POSITION
				// 2 floats == 8 bytes
				// Offset = 0
				// 3 floats == 12 bytes
				// Offset = 0
				{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

				// COLOR0
				// D3DCOLOR == 4 bytes
				// Offset = 8
				// D3DCOLOR == 4 bytes
				// Offset = 12
				{ 0, 32, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
				{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

				// The following marker signals the end of the vertex declaration
				D3DDECL_END()
			};
			HRESULT result = context.s_direct3dDevice->CreateVertexDeclaration(vertexElements, &s_vertexDeclaration);
			if (SUCCEEDED(result))
			{
				result = context.s_direct3dDevice->SetVertexDeclaration(s_vertexDeclaration);
				if (FAILED(result))
				{
					eae6320::UserOutput::Print("Direct3D failed to set the vertex declaration");
					return false;
				}
			}
			else
			{
				eae6320::UserOutput::Print("Direct3D failed to create a Direct3D9 vertex declaration");
				return false;
			}
		}

		// Create a vertex buffer
		{
			context.changeWindingOrder();
			// We are drawing one square
			//const unsigned int vertexCount = 4;	// What is the minimum number of vertices a square needs (so that no data is duplicated)?
			const unsigned int bufferSize = (numberOfVertices) * sizeof(eae6320::s_vertex) ;
			// We will define our own vertex format
			const DWORD useSeparateVertexDeclaration = 0;
			// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
			const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
			HANDLE* const notUsed = NULL;
			const HRESULT result = context.s_direct3dDevice->CreateVertexBuffer(bufferSize, usage, useSeparateVertexDeclaration, useDefaultPool,
				&s_vertexBuffer, notUsed);
			if (FAILED(result))
			{
				eae6320::UserOutput::Print("Direct3D failed to create a vertex buffer");
				return false;
			}
		}
		// Fill the vertex buffer with the triangle's vertices
		{
			// Before the vertex buffer can be changed it must be "locked"
			//sVertex* vertexData = vertexDataMesh.returnStructVertex();
			eae6320::s_vertex* vertexData;
			{
				const unsigned int lockEntireBuffer = 0;
				const DWORD useDefaultLockingBehavior = 0;
				const HRESULT result = s_vertexBuffer->Lock(lockEntireBuffer, lockEntireBuffer,
					reinterpret_cast<void**>(&vertexData), useDefaultLockingBehavior);

				if (FAILED(result))
				{
					eae6320::UserOutput::Print("Direct3D failed to lock the vertex buffer");
					return false;
				}

			}
			// Fill the buffer
			{
				for (int i = 0; i < context.numberOfVertices; i++) {
					vertexData[i] = context.vertexData[i];
				}

			}
			// The buffer must be "unlocked" before it can be used
			{
				const HRESULT result = s_vertexBuffer->Unlock();
				if (FAILED(result))
				{
					eae6320::UserOutput::Print("Direct3D failed to unlock the vertex buffer");
					return false;
				}
			}
		}

		return true;
	}


	bool Mesh::CreateIndexBuffer(GraphicsContext& context)
	{
		// The usage tells Direct3D how this vertex buffer will be used
		DWORD usage = 0;
		{
			// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
			const HRESULT result = GetVertexProcessingUsage(usage,context);
			if (FAILED(result))
			{
				return false;
			}
			// Our code will only ever write to the buffer
			usage |= D3DUSAGE_WRITEONLY;
		}

		// Create an index buffer
		unsigned int bufferSize;
		{
			// We are drawing a square
			const unsigned int triangleCount = numberOfIndices /3;	// How many triangles does a square have?
			const unsigned int vertexCountPerTriangle = 3;


			//const unsigned int numberOfFaces = (numberOfIndices / 6);
			//if (numberOfFaces < 0)
			//	const unsigned int numberOfFaces = 1;


			bufferSize = triangleCount * vertexCountPerTriangle * sizeof(uint32_t);
			// We'll use 32-bit indices in this class to keep things simple
			// (i.e. every index will be a 32 bit unsigned integer)
			const D3DFORMAT format = D3DFMT_INDEX32;
			// Place the index buffer into memory that Direct3D thinks is the most appropriate
			const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
			HANDLE* notUsed = NULL;
			const HRESULT result = context.s_direct3dDevice->CreateIndexBuffer(bufferSize, usage, format, useDefaultPool,
				&s_indexBuffer, notUsed);
			if (FAILED(result))
			{
				eae6320::UserOutput::Print("Direct3D failed to create an index buffer");
				return false;
			}
		}
		// Fill the index buffer with the triangles' connectivity data
		{
			// Before the index buffer can be changed it must be "locked"
			uint32_t* indexData;
			{
				const unsigned int lockEntireBuffer = 0;
				const DWORD useDefaultLockingBehavior = 0;
				const HRESULT result = s_indexBuffer->Lock(lockEntireBuffer, lockEntireBuffer,
					reinterpret_cast<void**>(&indexData), useDefaultLockingBehavior);
				if (FAILED(result))
				{
					eae6320::UserOutput::Print("Direct3D failed to lock the index buffer");
					return false;
				}
			}
			// Fill the buffer
			{

				for (int i = 0; i < context.numberOfIndices; i++)
				{
					indexData[i] = context.indexData[i];
				}

			}
			// The buffer must be "unlocked" before it can be used
			{
				const HRESULT result = s_indexBuffer->Unlock();
				if (FAILED(result))
				{
					eae6320::UserOutput::Print("Direct3D failed to unlock the index buffer");
					return false;
				}
			}
		}

		return true;
	}



	void Mesh::ShutDown(GraphicsContext& context)
	{
		if (s_vertexBuffer)
		{
			s_vertexBuffer->Release();
			s_vertexBuffer = NULL;
		}
		if (s_indexBuffer)
		{
			s_indexBuffer->Release();
			s_indexBuffer = NULL;
		}
		if (s_vertexDeclaration)
		{
			context.s_direct3dDevice->SetVertexDeclaration(NULL);
			s_vertexDeclaration->Release();
			s_vertexDeclaration = NULL;
		}
	}
}