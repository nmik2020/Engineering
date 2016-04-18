#pragma once
#pragma once

#include "..\Math\cVector.h"

namespace eae6320
{
	struct Vertex
	{
		Math::cVector m_position;
		

		Vertex::Vertex()
		{
			m_position = Math::cVector();
		}

		Vertex::Vertex(Math::cVector position)
		{
			m_position = position;
		}
	};

	class Triangle
	{
	public:
		Vertex m_vertex[3];

		Triangle::Triangle()
		{
			for (size_t i = 0; i < 3; ++i)
			{
				m_vertex[i] = Vertex();
			}
		}

		Triangle::Triangle(Vertex A, Vertex B, Vertex C)
		{
			m_vertex[0] = A;
			m_vertex[1] = B;
			m_vertex[2] = C;
		}
	};
}