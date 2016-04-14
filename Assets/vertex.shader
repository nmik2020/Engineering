/*
	This is an example of a fragment shader
*/

// Platform-specific setup
#include "shaders.inc"
//Position Offset
//uniform float2 g_position_offset;

//matrices
uniform float4x4 g_transform_localToWorld;
uniform float4x4 g_transform_worldToView;
uniform float4x4 g_transform_viewToScreen;

#if defined( EAE6320_PLATFORM_D3D )
// Entry Point
//============

void main(
	
	// Input
	//======
	// Whatever arbitrary data (i.e. everything excluding position) was output from the vertex shader
	// will be interpolated across the triangle and given as input to the fragment shader
	in float4 i_color : COLOR0,
	in const float3 i_position_local : POSITION,
	in float2 i_texture : TEXCOORD0,
	//in const float3 i_position : POSITION,

	// Output
	//=======

	// Whatever color value is output from the fragment shader
	// will determine the color of the corresponding pixel on the screen
	out float4 o_color : COLOR0,
	out float4 o_position_screen : POSITION,
	out float2 o_texture : TEXCOORD0

	//out float4 o_position : POSITION

	)

#elif defined( EAE6320_PLATFORM_GL )

// Input
//======

// Whatever arbitrary data (i.e. everything excluding position) was output from the vertex shader
// will be interpolated across the triangle and given as input to the fragment shader

layout( location = 2 ) in vec4 i_color;
layout(location = 1) in vec2 i_texture;
layout(location = 0) in vec3 i_position_local;

layout(location = 1) out vec2 o_texture;
layout(location = 0) out vec4 o_color;


// Entry Point
//============

void main()

#endif
{
	#if defined( EAE6320_PLATFORM_D3D )

	{
		float4 position_world = Transform(float4(i_position_local, 1.0), g_transform_localToWorld);
		float4 position_view = Transform(position_world, g_transform_worldToView);
		O_POSITION_SCREEN = Transform(position_view, g_transform_viewToScreen);
	}

	#elif defined( EAE6320_PLATFORM_GL )
	// Calculate position
	{
		float4 position_world = Transform(float4(i_position_local, 1.0), g_transform_localToWorld);
		float4 position_view = Transform(position_world, g_transform_worldToView);
		O_POSITION_SCREEN = Transform(position_view, g_transform_viewToScreen);
	}	
	#endif

	{
		o_color = i_color;
		o_texture = i_texture;
	}
}