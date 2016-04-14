#ifndef S_VERTEX_H
#define S_VERTEX_H
#include <cstdint>

namespace eae6320 {

	struct s_vertex
	{
		float x, y, z;
		float nx, ny, nz; // Normals

	private:
		float u, v;

#if defined( EAE6320_PLATFORM_D3D )
		uint8_t b, g, r, a;

#elif defined( EAE6320_PLATFORM_GL )
		uint8_t r, g, b, a;
		//Textures
#endif

	public:
		void setNormals(float xValue, float yValue, float zValue)
		{
			nx = xValue;
			ny = yValue;
			nz = zValue;
		}
		void setVertex(float xValue, float yValue, float zValue)
		{
			x = xValue;
			y = yValue;
			z = zValue;
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
			v = textureV;
		}
	};
}

#endif
