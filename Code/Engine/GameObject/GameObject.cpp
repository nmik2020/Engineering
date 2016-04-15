#include "GameObject.h"
#include "../Graphics/Graphics.h"

void eae6320::GameObject::setOffsetPosition(Math::cVector& offset)
{
	//velocity  += offset * eae6320::timeLeftInFrame;

	//position += velocity + offset * eae6320::timeLeftInFrame*eae6320::timeLeftInFrame*0.5f;


	offset = offset*0.4 ;
	position += offset;
}