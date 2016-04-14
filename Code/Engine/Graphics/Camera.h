#ifndef __CAMERA__H
#define __CAMERA__H


#include "../Math/cVector.h"
#include "../Math/cMatrix_transformation.h"
#include "../Math/cQuaternion.h"
#include "../Math/Functions.h"
#include <vector>


namespace eae6320
{

	class Camera
	{
	
	public:
		 Math::cVector position;
		 Math::cVector previousMovementOffset;
		 Math::cVector acceleration;
		 Math::cVector velocity;
		 bool isFlying;
		 Math::cQuaternion rotation;
		 float aspectRatio = (800.0f / 600.0f);
		 float fieldOfView = Math::ConvertDegreesToRadians(60.0f);
		 float nearPlaneZ = 0.1f;
		 float farPlaneZ = 10000.0f;

		 Camera()
		 {

		  position = Math::cVector(0, 0, 10.0f);
		  acceleration = Math::cVector(0, 0, 0);
		  velocity = Math::cVector(0, 0, 0);
		  isFlying = false;
		  previousMovementOffset = Math::cVector(0, 0, 0.0f);

		  rotation = Math::cQuaternion();
		 };
		~Camera()
		{
		};
		eae6320::Math::cVector CalcDirection(eae6320::Math::cQuaternion local, eae6320::Math::cQuaternion fixer)
		{
			eae6320::Math::cQuaternion result = (fixer * local) * fixer.CreateInverse();
			return eae6320::Math::cVector(result.m_x, result.m_y, -result.m_z);
		}

		eae6320::Math::cVector Front()
		{
			return CalcDirection(Math::cQuaternion(0.f, 0.f, 0.f, 1.f), rotation.CreateNormalized());
		}

		Math::cVector Up()
		{
			return CalcDirection(Math::cQuaternion(0.f, 0.f, 1.f, 0.f), rotation.CreateNormalized());
		}

		Math::cVector Right()
		{
			return CalcDirection(Math::cQuaternion(0.f, 1.f, 0.f, 0.f), rotation.CreateNormalized());
		}


		  void Camera::updatePosition(Math::cVector positionOffset);
		  void updateRotation(Math::cQuaternion positionOffset)
		  {
			  rotation = positionOffset * rotation;
		  }
	};
}
#endif