return
{
	effectPath = "data/Effects.lua",
	Uniform = 
	{
		{
			handleName = "g_rgb",
			values = {1.0, 1.0, 1.0},
			-- 0 = Vertex, 1 = Fragment ---
			shaderType = 1,
		},
		
		
	},
	Texture = 
	{
		{
			sampler = "textureSampler",
			texturePath = "data/floor_D.PNG",
		},	

	},
	
}
