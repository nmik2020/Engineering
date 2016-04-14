/*
	This is an example of a fragment shader
*/

// Platform-specific setup
#include "shaders.inc"
uniform float3 g_rgb;
uniform sampler2D textureSampler;

#if defined( EAE6320_PLATFORM_D3D )

// Entry Point
//============

void main(

	in float4 i_color : COLOR0,
	in float2 o_texture : TEXCOORD0,

	out float4 o_color : COLOR0

	)

#elif defined( EAE6320_PLATFORM_GL )

layout( location = 1 ) in vec2 o_texture;
layout(location = 0) in vec4 i_color;


out vec4 o_color;


void main()

#endif
{

	{
		o_color = i_color;
		o_color.rgb *= g_rgb;
#if defined( EAE6320_PLATFORM_D3D )
		o_color *= tex2D(textureSampler, o_texture);
#elif defined( EAE6320_PLATFORM_GL )
		o_color *= texture2D(textureSampler, o_texture);
#endif
	}
}
