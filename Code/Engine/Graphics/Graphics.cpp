#include "Graphics.h"
#include "../../Engine/UserInput/UserInput.h"
#include "../../Engine/Time/Time.h"


  std::vector<eae6320::GameObject*> gameObjects;
#ifdef _DEBUG

  std::vector<eae6320::Graphics::DebugBox*> debugBoxes;
  std::vector<eae6320::Graphics::DebugLine*> debugLines;
  std::vector<eae6320::Graphics::DebugSphere*> debugSpheres;
  DebugMenu debugMenu; 
#endif
  std::vector<GameSprite*> spriteObjects;

 eae6320::GraphicsContext context;
 //eae6320::Effect effects;
#if defined (EAE6320_PLATFORM_D3D)
 HWND s_renderingWindow = NULL;
 IDirect3D9* s_direct3dInterface = NULL;
 IDirect3DDevice9* s_direct3dDevice = NULL;

#endif

 bool hoverOnTilde = false;
 bool isTildeSelected = false;
 bool isKSelected = false;
 bool hoveronK = false;


 void eae6320::Graphics::RunDebugMenu()
 {
	 debugMenu.Activate(true, context.s_direct3dDevice);
 }



 // Given segment pq and triangle abc, returns whether segment intersects
// triangle and if so, also returns the barycentric coordinates (u,v,w)
// of the intersection point
int eae6320::Graphics::IntersectSegmentTriangle(eae6320::Math::cVector p, eae6320::Math::cVector q,
	eae6320::Math::cVector a, eae6320::Math::cVector b, eae6320::Math::cVector c,
                             float &u, float &v, float &w, float &t, eae6320::Math::cVector& normal)
{
    eae6320::Math::cVector ab = b - a;
    eae6320::Math::cVector ac = c - a;
    eae6320::Math::cVector qp = p - q;

    // Compute triangle normal. Can be precalculated or cached if
    // intersecting multiple segments against the same triangle
    normal = Cross(ab, ac);

    // Compute denominator d. If d <= 0, segment is parallel to or points
    // away from triangle, so exit early
    float d = Dot(qp, normal);
    if (d <= 0.0f) return 0;

    // Compute intersection t value of pq with plane of triangle. A ray
    // intersects if 0 <= t. Segment intersects if 0 <= t <= 1. Delay
    // dividing by d until intersection has been found to pierce triangle
    eae6320::Math::cVector ap = p - a;
    t = Dot(ap, normal);
    if (t < 0.0f) return 0;
    if (t > d) return 0; // For segment; exclude this code line for a ray test

    // Compute barycentric coordinate components and test if within bounds
    eae6320::Math::cVector e = Cross(qp, ap);
    v = Dot(ac, e);
    if (v < 0.0f || v > d) return 0;
    w = -Dot(ab, e);
    if (w < 0.0f || v + w > d) return 0;

    // Segment/ray intersects triangle. Perform delayed division and
    // compute the last barycentric coordinate component
    float ood = 1.0f / d;
    t *= ood;
    v *= ood;
    w *= ood;
    u = 1.0f - v - w;
    return 1;
}



void  eae6320::handleCollisions(float i_frameTime)
{
	//Not Flying
	if (!gameObjects[0]->camObject->isFlying) {
		int maxTries = 10;

		timeLeftInFrame = i_frameTime;

		while ((timeLeftInFrame > 0.0f) && (--maxTries > 0))
		{
			float timeCollided;

			//if (!onTheGround) 
			//{
			//	eae6320::Graphics::traceRayVertical();
			//}
			//eae6320::Graphics::traceRayHorizontal(timeCollided);

			eae6320::Graphics::updateWorld();
			timeLeftInFrame = 0.0f;
		}

		if (timeLeftInFrame)
			eae6320::Graphics::updateWorld();
	}// Is Flying
	else {
		timeLeftInFrame = i_frameTime;

		while ((timeLeftInFrame > 0.0f)) {
			eae6320::Graphics::updateWorld();
			timeLeftInFrame = 0.0f;

		}
		/*if (timeLeftInFrame)
			eae6320::Graphics::updateWorld();*/

	}
}
void eae6320::Graphics::updateWorld()
{
	if (!onTheGround) {
		//gameObjects[0]->camObject->acceleration += Math::cVector(0, -10.f, 0); // gravity
	}
	else
	{
		gameObjects[0]->camObject->acceleration.y = 0;
		gameObjects[0]->camObject->velocity.y = 0;
	}
	//Add friction
	gameObjects[0]->camObject->acceleration -= eae6320::Math::cVector
		(gameObjects[0]->camObject->velocity.x * 4.0f, gameObjects[0]->camObject->velocity.y * 0.2f,
			gameObjects[0]->camObject->velocity.z * 4.0f);
	//Velocity = u + at
	gameObjects[0]->camObject->velocity += gameObjects[0]->camObject->acceleration * eae6320::timeLeftInFrame;
	
	if (!gameObjects[0]->camObject->isFlying)
		gameObjects[0]->camObject->position += gameObjects[0]->camObject->velocity
		+ gameObjects[0]->camObject->acceleration*eae6320::timeLeftInFrame*eae6320::timeLeftInFrame*0.5f;
	else {
		gameObjects[0]->camObject->position.y = 100.0f;
		gameObjects[0]->camObject->position.x += gameObjects[0]->camObject->velocity.x
			+ gameObjects[0]->camObject->acceleration.x*eae6320::timeLeftInFrame*eae6320::timeLeftInFrame*0.5f;
		gameObjects[0]->camObject->position.z += gameObjects[0]->camObject->velocity.z
			+ gameObjects[0]->camObject->acceleration.z*eae6320::timeLeftInFrame*eae6320::timeLeftInFrame*0.5f;
	}
	
	
	eae6320::Time::OnNewFrame();
}

void  eae6320::Graphics::traceRayHorizontal(float& timeCollided)
{
	float shortestTime = FLT_MAX;
	Math::cVector intersectionPoint;
	Math::cVector normalAtIntersection;

	Math::cVector parallelStart = gameObjects[0]->camObject->position;
	/*Math::cVector parallelEnd = gameObjects[0]->camObject->position + gameObjects[0]->camObject->velocity + 
		gameObjects[0]->camObject->acceleration * timeLeftInFrame * timeLeftInFrame * 0.5f;*/
	Math::cVector parallelEnd = gameObjects[0]->camObject->position +10.0f;
	bool intersected = false;
	float shortestTimeReal = FLT_MAX;

	for (int i = 0; i < context.numberOfIndices; i += 3)
	{
		int index = context.indexData[i];
		int index1 = context.indexData[i + 1];
		int index2 = context.indexData[i + 2];
		eae6320::Math::cVector vertexA = eae6320::Math::cVector(context.vertexData[index].x, context.vertexData[index].y, context.vertexData[index].z);
		eae6320::Math::cVector vertexB = eae6320::Math::cVector(context.vertexData[index1].x, context.vertexData[index1].y, context.vertexData[index1].z);
		eae6320::Math::cVector vertexC = eae6320::Math::cVector(context.vertexData[index2].x, context.vertexData[index2].y, context.vertexData[index2].z);
		eae6320::Math::cVector normal  = eae6320::Math::cVector(context.vertexData[i].nx, context.vertexData[i].ny, context.vertexData[i].nz);

		float u = 0, v = 0, w = 0, t = 0;
		int a = IntersectSegmentTriangle(parallelStart, parallelEnd,
			vertexA, vertexC, vertexB, u, v, w, t,normal);
		 //normal = eae6320::Math::cVector(context.vertexData[i].nx, context.vertexData[i].ny, context.vertexData[i].nz);

		if (a == 1)
		{
			if (t < shortestTime) {
				shortestTime = t;
				//intersectionPoint = vertexA*u + vertexB*v + vertexC*w;
				intersectionPoint = parallelStart + (parallelEnd - parallelStart) * t;
				//normalAtIntersection = normal * -1.0f;
				shortestTimeReal = (intersectionPoint - parallelStart).GetLength() / 
					(parallelStart - parallelEnd).GetLength();
				intersected = true;

			}
		}

	}

	if (intersected)
	{
		float oneByNormal = 1 /( normalAtIntersection.GetLength());
		Math::cVector verticalProjectionAcceleration = normalAtIntersection * eae6320::Math::Dot(gameObjects[0]->camObject->acceleration, normalAtIntersection)*oneByNormal*oneByNormal;
		gameObjects[0]->camObject->acceleration = gameObjects[0]->camObject->acceleration - verticalProjectionAcceleration;

		Math::cVector verticalProjectionVelocity = normalAtIntersection * eae6320::Math::Dot(gameObjects[0]->camObject->velocity, normalAtIntersection)*oneByNormal*oneByNormal;
		gameObjects[0]->camObject->velocity = gameObjects[0]->camObject->velocity - verticalProjectionVelocity;

		gameObjects[0]->camObject->position = intersectionPoint - normalAtIntersection.CreateNormalized();
		timeCollided = shortestTimeReal;
	}
	
}

void  eae6320::Graphics::traceRayVertical()
{
	float shortestTime = FLT_MAX;
	Math::cVector intersectionPoint;
	bool intersected = false;

	for (int i = 0; i <context.numberOfIndices; i += 3)
	{
		int index = context.indexData[i];
		int index1 = context.indexData[i + 1];
		int index2 = context.indexData[i + 2];
		eae6320::Math::cVector vertexA = eae6320::Math::cVector(context.vertexData[index].x, context.vertexData[index].y, context.vertexData[index].z);
		eae6320::Math::cVector vertexB = eae6320::Math::cVector(context.vertexData[index1].x, context.vertexData[index1].y, context.vertexData[index1].z);
		eae6320::Math::cVector vertexC = eae6320::Math::cVector(context.vertexData[index2].x, context.vertexData[index2].y, context.vertexData[index2].z);
		float u = 0, v = 0, w = 0, t = 0;
		eae6320::Math::cVector normal = eae6320::Math::cVector(context.vertexData[i].nx, context.vertexData[i].ny, context.vertexData[i].nz);

		int a = IntersectSegmentTriangle(gameObjects[0]->camObject->position, gameObjects[0]->camObject->position + eae6320::Math::cVector(0, -10.0f, 0),
			vertexA, vertexC, vertexB, u, v, w, t,normal);
		if (a == 1)
		{
			if (t < shortestTime) {
				shortestTime = t;
				intersectionPoint = vertexA*u + vertexB*v + vertexC*w;
			}
			intersected = true;
		}
		
	}

	if (intersected)
	{
		if(!onTheGround)
			gameObjects[0]->camObject->position =  intersectionPoint + eae6320::Math::cVector(0, 50.0f, 0);
		onTheGround = true;
	}
	else 
	{
		onTheGround = false;
	}
}

void eae6320::Graphics::Render()
{
	
	context.clear();
	{
			context.StartScene();
			{
			
				
				for (unsigned int i = 0; i < gameObjects.size(); i++)
				{
				
					//Set Shaders
					gameObjects[i]->material->effect->setEffects(context);
					gameObjects[i]->material->effect->setPositionOffset(context, 
						gameObjects[i]->positionOffsetMatrix, 

						Math::cMatrix_transformation::CreateWorldToViewTransform(
							gameObjects[i]->rotationOffset, gameObjects[i]->position), 
						gameObjects[i]->camObject);

					gameObjects[i]->material->effect->setUniform(context,
						gameObjects[i]->material->uniformData, 
						gameObjects[i]->material->lengthOfUniform,
						gameObjects[i]->material->handleList);

					gameObjects[i]->material->setTexture(context);

					gameObjects[i]->mesh->DrawMesh(context);
				}
	
				debugMenu.menuOffset =  1/ eae6320::Time::GetSecondsElapsedThisFrame();
#ifdef _DEBUG
				//Enable Flycam
				if (eae6320::UserInput::IsKeyPressed('I'))
				{

					if (!hoveronK)
					{
						if (!isKSelected)
						{
							isKSelected = true;
							gameObjects[0]->camObject->isFlying = true;
							gameObjects[0]->flyCamObject->position = gameObjects[0]->camObject->position;
						}
						else
						{
							isKSelected = false;
							gameObjects[0]->camObject->isFlying = false;
							gameObjects[0]->camObject->position = gameObjects[0]->flyCamObject->position;
								//gameObjects[0]->camObject->position.x = gameObjects[0]->flyCamObject->position.x - 50.0f;
							;

						}
					}
					hoveronK = true;
				}
				else
				{
					hoveronK = false;
				}
				
			
				//Enable menu
				if (eae6320::UserInput::IsKeyPressed('B'))
				{

					if (!hoverOnTilde)
					{
						if (!isTildeSelected)
						{
							isTildeSelected = true;
							isDebugMenuActive = true;

						}
						else
						{
							isTildeSelected = false;
							isDebugMenuActive = false;
							debugMenu.isMenuActive = false;

						}
					}
					hoverOnTilde = true;
				}
				else
				{
					hoverOnTilde = false;
				}
				//Draw sphere where the player is standing once the flycam is activated
				if (gameObjects[0]->camObject->isFlying)
				{
					DebugSphere *currentObject;
					currentObject = debugSpheres[0];
					currentObject->m_center = gameObjects[0]->flyCamObject->position;
					currentObject->LoadDebugSphere();
					currentObject->DrawSphere();
				}
				if(isTildeSelected)
				 RunDebugMenu();
#endif

				for (unsigned int i = 0; i < spriteObjects.size(); i++)
				{
					GameSprite *currentSprite;
					currentSprite = spriteObjects[i];
					if (currentSprite && currentSprite->isInitialized())
						currentSprite->Draw(0.0f);
				}

				for (unsigned int i = 0; i < debugBoxes.size(); i++)
				{
					DebugBox *currentObject;
					currentObject = debugBoxes[i];
					currentObject->DrawBox();
				}

				for (unsigned int i = 0; i < debugLines.size(); i++)
				{
					DebugLine *currentObject;
					currentObject = debugLines[i];
					currentObject->DrawLine();
				}

				for (unsigned int i = 0; i < debugSpheres.size(); i++)
				{
					DebugSphere *currentObject;
					currentObject = debugSpheres[i];
					

					if ((debugMenu.checkBoxChecked && debugMenu.sliderSelected) || debugMenu.checkBoxChecked)
					{
						currentObject->DrawSphere();
					}
					
				}

			}
			context.EndScene();
	}
	context.displayRenderedBuffer();
}