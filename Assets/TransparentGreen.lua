return
{
	
	effectPath = "data/TransparentEffects.lua",
	Uniform = 
	{
		{
			handleName = "g_rgb",
			values = {0.5, 1.0, 0.5},
			-- 0 = Vertex, 1 = Fragment ---
			shaderType = 1,
		},
		
		{
			handleName = "g_alpha_modifier",
			values = {0.4},
		-- 0 = Vertex, 1 = Fragment ---
			shaderType = 1,
		},
		
		
		
	},
	Texture = 
	{
		{
			sampler = "textureSampler",
			texturePath = "data/Texture1.dds",
		},	
		{
			sampler = "textureSampler",
			texturePath = "data/Texture1.dds",
		},
		
		
	},
	
}
