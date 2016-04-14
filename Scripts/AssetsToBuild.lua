
return
{
	{
		builder = "ShaderBuilder.exe",
		assets =
		{
			{
				"vertex.shader",
				"transparent.shader",
				"fragment.shader",
			},
		},
		shaderType = 
		{
				"vertex",
				"fragment",
				"fragment",
		},
		dependencies = 
		{
			"shaders.inc",
		},
		

	},
	{
		builder = "EffectBuilder.exe",
		assets =
		{
			{
				"Effects.lua",
				"TransparentEffects.lua",
			},
		},
	},
	{
		builder = "MeshBuilder.exe",
		assets =
		{
			{
				"Cement.lua",
				"Floor.lua",
				"Metal.lua",
				"Railing.lua",
				"Walls.lua",
				"ceiling.lua",
				"CylinderWireFrame.lua",
				"CylinderWireFrame2.lua",
				"CollisionLayer.lua",
				"ServerPlayerObject.lua",
			},
		},
	},

	{
		builder = "MaterialBuilder.exe",
		assets =
		{
			{
				"WallMaterial.lua",
				"CementMaterial.lua",
				"FloorMaterial.lua",
				"MetalBraceMaterial.lua",
				"RailingMaterial.lua",
				"TransparentSquare.lua",
				"ServerPlayerMaterial.lua",
				"ClientPlayerMaterial.lua",


			},
		},
	},

	{
		builder = "TextureBuilder.exe",
		assets =
		{
			{
				"wall_D.PNG",
				"cement_wall_D.PNG",
				"floor_D.PNG",
				"metal_brace_D.PNG",
				"railing_D.PNG",
				"numbers.png",
				"logo.png",
				"button.png",
				"checked_checkbox.png",
				"unchecked_checkbox.png",
				"ag-square.png",
			},
		},
	},
	
}
