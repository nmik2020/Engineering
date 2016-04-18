#pragma once
#include "../Math/cVector.h"
namespace eae6320
{
	struct TAABB
	{
		Math::cVector m_vecMax;
		Math::cVector m_vecMin;
		bool AABBtoAABB(const TAABB& tBox1, const TAABB& tBox2);
		bool PointInAABB(const TAABB& tBox, const Math::cVector& vecPoint)
			;
		void splitBoundingBoxes(TAABB boxes[]);

	};

	bool TAABB::AABBtoAABB(const TAABB& tBox1, const TAABB& tBox2)
	{

		//Check if Box1's max is greater than Box2's min and Box1's min is less than Box2's max
		return(tBox1.m_vecMax.x > tBox2.m_vecMin.x &&
			tBox1.m_vecMin.x < tBox2.m_vecMax.x &&
			tBox1.m_vecMax.y > tBox2.m_vecMin.y &&
			tBox1.m_vecMin.y < tBox2.m_vecMax.y &&
			tBox1.m_vecMax.z > tBox2.m_vecMin.z &&
			tBox1.m_vecMin.z < tBox2.m_vecMax.z);

		//If not, it will return false

	}

	bool TAABB::PointInAABB(const TAABB& tBox, const Math::cVector& vecPoint)
	{

		//Check if the point is less than max and greater than min
		if (vecPoint.x > tBox.m_vecMin.x && vecPoint.x < tBox.m_vecMax.x &&
			vecPoint.y > tBox.m_vecMin.y && vecPoint.y < tBox.m_vecMax.y &&
			vecPoint.z > tBox.m_vecMin.z && vecPoint.z < tBox.m_vecMax.z)
		{
			return true;
		}

		//If not, then return false
		return false;

	}

	void TAABB::splitBoundingBoxes(TAABB boxes[])
	{

	}
}