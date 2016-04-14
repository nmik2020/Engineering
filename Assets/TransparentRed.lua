return
{
	
	effectPath = "data/TransparentEffects.lua",
	Uniform = 
	{
		{
			handleName = "g_rgb",
			values = {1.0, 1.0, 1.0},
			-- 0 = Vertex, 1 = Fragment ---
			shaderType = 1,
		},
		
		{
			handleName = "g_alpha_modifier",
			values = {0.7},
		-- 0 = Vertex, 1 = Fragment ---
			shaderType = 1,
		},
		
		
		
	},
	Texture = 
	{
		{
			sampler = "textureSampler",
			texturePath = "data/Gold.dds",
		},	
		
		
	},
	
}
