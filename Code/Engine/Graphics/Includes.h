#ifndef EAE6320_GRAPHICS_INCLUDES_H
#define EAE6320_GRAPHICS_INCLUDES_H
#ifdef _DEBUG
#define EAE6320_GRAPHICS_AREDEBUGSHADERSENABLED
#endif
namespace eae6320
{
	namespace Graphics
	{
		enum ShaderTypes
		{
			Vertex,
			Fragment,
			Unknown
		};
		
	}
}

#endif