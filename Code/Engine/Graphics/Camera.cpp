#include "Camera.h"
#include <math.h>     

namespace eae6320
{
	  void Camera::updatePosition(Math::cVector positionOffset)
	{
		//movementOffset += positionOffset;
		previousMovementOffset = position;
		acceleration = positionOffset;
		acceleration.x = positionOffset.x *  cos(2 * asinf(rotation.m_y)) - positionOffset.z*sin(2 * asinf(rotation.m_y));
		acceleration.z = positionOffset.x *  sin(2 * asinf(rotation.m_y)) + positionOffset.z*cos(2 * asinf(rotation.m_y));
		/*rotation.m_x *= cos(2 * asinf(rotation.m_y)) - positionOffset.z*sin(2 * asinf(rotation.m_y));
		rotation.m_z *=  sin(2 * asinf(rotation.m_y)) + positionOffset.z*cos(2 * asinf(rotation.m_y));*/

	/*	movementOffset.x += positionOffset.x *  cos(2*asinf(rotationOffset.m_y)) - positionOffset.z*sin(2*asinf(rotationOffset.m_y));
		movementOffset.z += positionOffset.x *  sin(2*asinf(rotationOffset.m_y)) + positionOffset.z*cos(2*asinf(rotationOffset.m_y));

		movementOffset.y += positionOffset.y;*/

	  }
}
