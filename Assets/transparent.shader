/*
	This is an example of a fragment shader
*/

// Platform-specific setup
#include "shaders.inc"
uniform float3 g_rgb;
uniform float g_alpha_modifier;
uniform sampler2D textureSampler;

#if defined( EAE6320_PLATFORM_D3D )

// Entry Point
//============

void main(

	// Input
	//======

	// Whatever arbitrary data (i.e. everything excluding position) was output from the vertex shader
	// will be interpolated across the triangle and given as input to the fragment shader

	in float4 i_color : COLOR0,
	in float2 o_texture : TEXCOORD0,


	// Output
	//=======

	// Whatever color value is output from the fragment shader
	// will determine the color of the corresponding pixel on the screen
	out float4 o_color : COLOR0

	)

#elif defined( EAE6320_PLATFORM_GL )

// Input
//======

// Whatever arbitrary data (i.e. everything excluding position) was output from the vertex shader
// will be interpolated across the triangle and given as input to the fragment shader
layout(location = 1) in vec2 o_texture;
layout( location = 0 ) in vec4 i_color;

// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
out vec4 o_color;

// Entry Point
//============

void main()

#endif
{
	// Set the fragment to the interpolated color that originated as per-vertex data
	// (where color is represented by 4 floats representing "RGBA" == "Red/Green/Blue/Alpha")
	{
		o_color = i_color;
		o_color.rgb *= g_rgb;
		o_color.a *= g_alpha_modifier;
#if defined( EAE6320_PLATFORM_D3D )
		o_color *= tex2D(textureSampler, o_texture);
		#elif defined( EAE6320_PLATFORM_GL )
		o_color *= texture2D(textureSampler, o_texture);

#endif
	}
}
