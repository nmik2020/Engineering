return
{
	
	effectPath = "data/TransparentEffects.lua",
	Uniform = 
	{
		{
			handleName = "g_rgb",
			values = {0.0, 0.0, 1.0},
			-- 0 = Vertex, 1 = Fragment ---
			shaderType = 1,
		},
		
		
		{
			handleName = "g_alpha_modifier",
			values = {1},
		-- 0 = Vertex, 1 = Fragment ---
			shaderType = 1,
		},
		
		
	},
	Texture = 
	{
		{
			sampler = "textureSampler",
			texturePath = "data/Texture1.jpg",
		},	
		{
			sampler = "textureSampler",
			texturePath = "data/Texture2.jpg",
		},
		
		
	},
	
}
