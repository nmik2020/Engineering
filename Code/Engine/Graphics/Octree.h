#pragma once

#include "AABB.h"
#include <vector>
namespace eae6320
{

	class Octree
	{
	public:
		TAABB m_bounds; //Root Node
		Octree *m_childNode[8] ; //Child nodes
		std::vector<Octree> m_objects; // Objects
	};


}