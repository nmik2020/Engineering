#ifndef __GAMEOBJECT__H
#define __GAMEOBJECT__H

//#include "../Graphics/Effect.h"
#include "../Graphics/Mesh.h"
#include "../Math/cVector.h"
#include "../Math/cMatrix_transformation.h"
#include "../Math/cQuaternion.h"
#include <gl/GLU.h>
#include <gl/GL.h>
#include "../../External/OpenGlExtensions/OpenGlExtensions.h"
//#include "../Graphics/Camera.h"
#include "../Graphics/Materials.h"


namespace eae6320
{

	class GameObject
	{
	public:
		GameObject() {
		mesh = new Mesh();
		//effect = new Effect();
		material = new Materials();
		camObject = new Camera();
		flyCamObject = new Camera();

		};

		void setOffsetPosition(Math::cVector& offset)
		{
			offset = offset * 0.1;
			position += offset;
		}


		~GameObject()
		{
			delete mesh;
			delete camObject;
			delete flyCamObject;
			delete material;
		};
		//current Position
		 Camera *camObject;
		 Camera *flyCamObject;

		 //Effect *effect;
		 Materials* material;
		 Mesh *mesh;
		 Math::cMatrix_transformation positionOffsetMatrix ;
		 Math::cVector position;
		 Math::cQuaternion rotationOffset;

	};
}
#endif