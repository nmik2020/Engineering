/*
	This file contains all of the function definitions for this example program
*/

// Header Files
//=============



#include "WindowsProgram.h"
#include <math.h>     
#include "../../Engine/Graphics/Camera.h"
#include "../../Engine/Math/cVector.h"
#include "../../Engine/GameObject/GameObject.h"

#include "../../Engine/UserInput/UserInput.h"
#include "../../Engine/Time/Time.h"

#include "../../Engine/Graphics/Graphics.h"
// Resource.h contains the #defines for the icon resources
// that the main window will use
#include "Resources/Resource.h"
// WindowsFunctions.h contains convenience functionality for Windows features;
// in this example program we just use it to get error messages
#include "../../Engine/Windows/Functions.h"
#include "../../Engine/Graphics/DebugShapes.h"
#include "../../Engine/Graphics/GameSprite.h"
#include <d3dx9core.h>

#include <stdio.h>
#include <string.h>
#include "../../../Raknet/Source/RakPeerInterface.h"
#include "../../../Raknet/Source/MessageIdentifiers.h"
#include "../../../Raknet/Source/BitStream.h"
#include "../../../Raknet/Source/RakNetTypes.h"  // MessageID

#define MAX_CLIENTS 10
#define SERVER_PORT 60000

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1
};

struct NetworkPacket
{
	float x, y, z;
	float m_x, m_y, m_z, m_w;
	char type;
};
// Static Data Initialization
//===========================
// Get the horizontal and vertical screen sizes in pixel
void GetDesktopResolution(int& horizontal, int& vertical)
{

	
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}
namespace
{

	NetworkPacket data;
	//To handle network messages
	HDC hDeviceContextHandle;
	RECT ClientRectangle;
	PAINTSTRUCT OurPaintStructure;


	RakNet::RakPeerInterface *peer = RakNet::RakPeerInterface::GetInstance();
	bool isServer;
	bool connectionEstablished;
	RakNet::Packet *packet;
	// Instead of a pointer Windows provides a "handle"
	// to represent each window that is created
	// ("HWND" == "window handle").
	// In Windows every separate element
	// (buttons, text fields, scroll bars, etc.) is a "window",
	// and so a typical Windows program will have many different windows.
	// In our class, however, we will only have a single main window.
	HWND s_mainWindow = NULL;

	// Window classes are almost always identified by name;
	// there is also a unique ATOM associated with them,
	// but in practice Windows expects to use the class name as an identifier.
	// If you don't change the name below from the default then
	// your program could have problems when it is run at the same time on the same computer
	// as one of your classmate's


	float lastUpdateTimer = 0;
	const float FrameTime = 1.0f / 60;
	eae6320::Math::cVector acceleration(0.0f, 0.0f, 0.0f);



	const char* s_mainWindowClass_name = "[Nidal]'s Main Window Class";
	//eae6320::GameObject *cylinderObject;
	eae6320::GameObject *floorObject;
	eae6320::GameObject *collisionLayerObject;

	eae6320::GameObject *serverPlayerObject;	
	eae6320::GameObject *clientPlayerObject;


	eae6320::GameObject *cementWallObject;
	eae6320::GameObject *metalBraceObject;
	eae6320::GameObject *railingObject;
	eae6320::GameObject *wallObject;
	eae6320::GameObject *ceilingObject;
	//eae6320::GameObject *backObject;

	eae6320::Graphics::DebugLine* s_debugLine1;
	eae6320::Graphics::DebugBox* s_debugBox1;
	eae6320::Graphics::DebugSphere* s_debugSphere1;

	eae6320::Graphics::DebugLine* s_debugLine2;
	eae6320::Graphics::DebugBox* s_debugBox2;
	eae6320::Graphics::DebugSphere* s_debugSphere2;
	eae6320::GameObject *debugCylinder;
	eae6320::GameObject *debugCylinder2;

	eae6320::Camera *cameraObject;

	std::vector<eae6320::GameObject*> objects;
	std::vector<eae6320::Graphics::DebugLine*> debugLineObjects;
	std::vector<eae6320::Graphics::DebugBox*> debugBoxObjects;
	std::vector<eae6320::Graphics::DebugSphere*> debugSphereObjects;
	std::vector<GameSprite*> gameSpriteObjects;

	GameSprite *sampleSprite;
	GameSprite *logoSprite;
	DebugMenu debugMenuObj;
	bool isSliderSelected = false;
	bool hoverOnSlider = false;

	bool isCheckBoxSelected = false;
	bool hoverOnCheckBox = false;

	bool upSelected = false;
	bool hoveringUp = false;

	bool downSelected = false;
	bool hoveringDown = false;

	bool isButtonSelected = false;
	bool hoverOnButton = false;

}
void updateDebugMenu()
;
// Main Function
//==============


void initNetwork()
{
	

	printf("(C) or (S)erver?\n");
	//fgets(str, 512, stdin);
	char str[512];
	if (!isServer)
	{
		RakNet::SocketDescriptor sd;
		peer->Startup(1, &sd, 1);
		isServer = false;
	}
	else {
		RakNet::SocketDescriptor sd(SERVER_PORT, 0);
		peer->Startup(MAX_CLIENTS, &sd, 1);
		isServer = true;
	}

	if (isServer)
	{
		printf("Starting the server.\n");
		// We need to let the server accept incoming connections from the clients
		peer->SetMaximumIncomingConnections(MAX_CLIENTS);
	}
	else {
		printf("Enter server IP or hit enter for 127.0.0.1\n");
		/*fgets(str,512,stdin);*/
		strcpy(str, "127.0.0.1");

		/*if (str[0] == 0) {
		strcpy(str, "127.0.0.1");
		}*/
		printf("Starting the client.\n");
		peer->Connect(str, SERVER_PORT, 0, 0);

	}



}

void updateNetwork()
{
	/*while (1)
	{*/
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			std::string networkMessage;

			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
					networkMessage = "Another Client has disconnected :";
				break;
			case ID_REMOTE_CONNECTION_LOST:
				networkMessage = "Another Client has lost the connection :";
				printf("Another client has lost the connection.\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				printf("Another client has connected.\n");
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				//Client writes data to be sent to server
				networkMessage = "Our connection request has been accepted:";

				//DrawText(hDeviceContextHandle, networkMessage.c_str(), -1, &ClientRectangle, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				// Use a BitStream to write a custom user message
				// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);

				bsOut.Write(clientPlayerObject->camObject->position.x);
				bsOut.Write(clientPlayerObject->camObject->position.y);
				bsOut.Write(clientPlayerObject->camObject->position.z);

				bsOut.Write(clientPlayerObject->camObject->rotation.m_w);
				bsOut.Write(clientPlayerObject->camObject->rotation.m_x);
				bsOut.Write(clientPlayerObject->camObject->rotation.m_y);
				bsOut.Write(clientPlayerObject->camObject->rotation.m_z);
				networkMessage = "Sending message from client to server";

				//DrawText(hDeviceContextHandle, networkMessage.c_str(), -1, &ClientRectangle, DT_SINGLELINE | DT_CENTER | DT_VCENTER);		
				peer->Send(&bsOut, HIGH_PRIORITY, UNRELIABLE, 0, packet->systemAddress, false);
			}
			break;
			case ID_NEW_INCOMING_CONNECTION:
				networkMessage = "A connection is incoming";

				//DrawText(hDeviceContextHandle, networkMessage.c_str(), -1, &ClientRectangle, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				printf("A connection is incoming.\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				if (isServer) {
					printf("A client has disconnected.\n");
				}
				else {
					printf("We have been disconnected.\n");
				}
				break;
			case ID_CONNECTION_LOST:
				if (isServer) {
					printf("A client lost the connection.\n");
				}
				else {
					printf("Connection lost.\n");
				}
				break;

			case ID_GAME_MESSAGE_1:
			{
				//Server reading from client or Client reading from server
				RakNet::RakString rs;
				float x, y, z;
				float m_w, m_x, m_y, m_z;
				connectionEstablished = true;

				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				//This will read the camera position of the client, so that we can set the client object's position from here
				bsIn.Read(x);
				bsIn.Read(y);
				bsIn.Read(z);

				bsIn.Read(m_w);
				bsIn.Read(m_x);
				bsIn.Read(m_y);
				bsIn.Read(m_z);
				bsIn.Read(rs);
				printf("%s\n", rs.C_String());
				
				data.x = x; data.y = y; data.z = z;
				data.m_w = m_w; data.m_x = m_x; data.m_y = m_y; data.m_z = m_z;
				RakNet::BitStream bsOut;
				//This will send the current Server/Client 's camera position
				bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
				if (isServer){
				bsOut.Write(serverPlayerObject->camObject->position.x);
				bsOut.Write(serverPlayerObject->camObject->position.y);
				bsOut.Write(serverPlayerObject->camObject->position.z);

				bsOut.Write(serverPlayerObject->camObject->rotation.m_w);
				bsOut.Write(serverPlayerObject->camObject->rotation.m_x);
				bsOut.Write(serverPlayerObject->camObject->rotation.m_y);
				bsOut.Write(serverPlayerObject->camObject->rotation.m_z);


				bsOut.Write("Server to Client");
				networkMessage = "Sending from server to client";

				}else {
					bsOut.Write(clientPlayerObject->camObject->position.x);
					bsOut.Write(clientPlayerObject->camObject->position.y);
					bsOut.Write(clientPlayerObject->camObject->position.z);

					bsOut.Write(clientPlayerObject->camObject->rotation.m_w);
					bsOut.Write(clientPlayerObject->camObject->rotation.m_x);
					bsOut.Write(clientPlayerObject->camObject->rotation.m_y);
					bsOut.Write(clientPlayerObject->camObject->rotation.m_z);


					bsOut.Write("Client to Server");
					networkMessage = "Sending from Client to server";

				}

				//DrawText(hDeviceContextHandle, networkMessage.c_str(), -1, &ClientRectangle, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				peer->Send(&bsOut, HIGH_PRIORITY, UNRELIABLE, 0, packet->systemAddress, false);
			}
			break;

			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}
		}
	/*}*/

}

void destroyNetwork()
{
	RakNet::RakPeerInterface::DestroyInstance(peer);

}
int CreateMainWindowAndReturnExitCodeWhenItCloses( const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState, bool iamServer )
{
	isServer = iamServer;

	objectInit();
	char str[512];

	initNetwork();
	// Try to create the main window
	if ( CreateMainWindow( i_thisInstanceOfTheProgram, i_initialWindowDisplayState ) )
	{
		// If the main window was successfully created wait for it to be closed
		const int exitCode = WaitForMainWindowToCloseAndReturnExitCode( i_thisInstanceOfTheProgram );
		return exitCode;
	}
	else
	{
		// If the main window wasn't created return a made-up error code
		return -1;
	}
}

// Helper Functions
//=================
void objectInit()
{
	sampleSprite = new GameSprite(100, 200);
	sampleSprite->file = "data/numbers.png";
	
	sampleSprite->width = 512;
	sampleSprite->height = 64;
	logoSprite = new GameSprite(300, -100);
	logoSprite->file = "data/logo.png";
	cameraObject = new eae6320::Camera();

	
	floorObject = new eae6320::GameObject();
	floorObject->mesh->path = "data/Floor.lua";
	floorObject->material->path = "data/FloorMaterial.lua";
	floorObject->camObject = cameraObject;

	clientPlayerObject = new eae6320::GameObject();
	clientPlayerObject->mesh->path = "data/ServerPlayerObject.lua";
	clientPlayerObject->material->path = "data/ClientPlayerMaterial.lua";


	serverPlayerObject = new eae6320::GameObject();
	serverPlayerObject->mesh->path = "data/ServerPlayerObject.lua";
	serverPlayerObject->material->path = "data/ServerPlayerMaterial.lua";

	collisionLayerObject = new eae6320::GameObject();
	collisionLayerObject->mesh->path = "data/CollisionLayer.lua";
	collisionLayerObject->material->path = "data/TransparentSquare.lua";
	collisionLayerObject->camObject = cameraObject;

	cementWallObject = new eae6320::GameObject();
	cementWallObject->mesh->path = "data/Cement.lua";
	cementWallObject->material->path = "data/CementMaterial.lua";
	cementWallObject->camObject = cameraObject;
	
	ceilingObject = new eae6320::GameObject();
	ceilingObject->mesh->path = "data/ceiling.lua";
	ceilingObject->material->path = "data/CementMaterial.lua";
	ceilingObject->camObject = cameraObject;


	metalBraceObject = new eae6320::GameObject();
	metalBraceObject->mesh->path = "data/Metal.lua";
	metalBraceObject->material->path = "data/MetalBraceMaterial.lua";
	metalBraceObject->camObject = cameraObject;

	railingObject = new eae6320::GameObject();
	railingObject->mesh->path = "data/Railing.lua";
	railingObject->material->path = "data/RailingMaterial.lua";
	railingObject->camObject = cameraObject;

	wallObject = new eae6320::GameObject();
	wallObject->mesh->path = "data/Walls.lua";
	wallObject->material->path = "data/WallMaterial.lua";
	wallObject->camObject = cameraObject;

	debugCylinder = new eae6320::GameObject();
	debugCylinder->mesh->path = "data/CylinderWireFrame.lua";
	debugCylinder->material->path = "data/WallMaterial.lua";
	debugCylinder->camObject = cameraObject;

	debugCylinder2 = new eae6320::GameObject();
	debugCylinder2->mesh->path = "data/CylinderWireFrame2.lua";
	debugCylinder2->material->path = "data/WallMaterial.lua";
	debugCylinder2->camObject = cameraObject;

	//cylinderObject->setOffsetPosition(coneObject->movmentOffset - 6);
#ifdef _DEBUG
	s_debugLine1 = new eae6320::Graphics::DebugLine(eae6320::Math::cVector(50.0f, -40.0f, 0.0f), eae6320::Math::cVector(50.0f, 50.0f, 50.0f), eae6320::Math::cVector(1.0f, 0.0f, 0.0f));
	s_debugBox1 = new eae6320::Graphics::DebugBox(eae6320::Math::cVector(30.0f, -60.0f, -40.0f), 20.0f, eae6320::Math::cVector(0.0f, 0.0f, 1.0f));
	s_debugSphere1 = new eae6320::Graphics::DebugSphere(eae6320::Math::cVector(150.0f, 0.0f, -150.0f), 30.0f, 40, 40, eae6320::Math::cVector(1.0f, 0.0f, 0.0f));

	s_debugLine2 = new eae6320::Graphics::DebugLine(eae6320::Math::cVector(150.0f, -140.0f, 0.0f), eae6320::Math::cVector(50.0f, 50.0f, 50.0f), eae6320::Math::cVector(0.0f, 0.0f, 1.0f));
	s_debugBox2 = new eae6320::Graphics::DebugBox(eae6320::Math::cVector(-30.0f, 0.0f, -40.0f), 20.0f, eae6320::Math::cVector(0.0f, 1.0f, 0.0f));
	s_debugSphere2 = new eae6320::Graphics::DebugSphere(eae6320::Math::cVector(150.0f, 0.0f, -150.0f), 30.0f, 40, 40, eae6320::Math::cVector(1.0f, 0.0f, 0.0f));

	/*debugBoxObjects.push_back(s_debugBox1);
	debugLineObjects.push_back(s_debugLine1);*/
	debugSphereObjects.push_back(s_debugSphere1);

	/*debugBoxObjects.push_back(s_debugBox2);
	debugLineObjects.push_back(s_debugLine2);
	debugSphereObjects.push_back(s_debugSphere2);*/

	/*objects.push_back(debugCylinder);
	objects.push_back(debugCylinder2);*/
#endif

	//gameSpriteObjects.push_back(sampleSprite);
	//gameSpriteObjects.push_back(logoSprite);
	//objects.push_back(collisionLayerObject);
	//serverPlayerObject->position.x -= 100;
	objects.push_back(railingObject);
	objects.push_back(ceilingObject);

	objects.push_back(cementWallObject);
	objects.push_back(wallObject);

	objects.push_back(metalBraceObject);
	objects.push_back(floorObject);
	//objects.push_back(clientPlayerObject);
	objects.push_back(serverPlayerObject);


	if (!isServer)
	{
		//I am Client
		clientPlayerObject->camObject = cameraObject;
		serverPlayerObject->camObject = new eae6320::Camera();

		serverPlayerObject->camObject->position = serverPlayerObject->position;
		serverPlayerObject->camObject->position.y += 80;
		serverPlayerObject->camObject->position.z += 300;


		clientPlayerObject->camObject->position.z = 100;


		cameraObject->position.z = clientPlayerObject->camObject->position.z;
	}
	else 
	{
		//I am server
		clientPlayerObject->camObject = new eae6320::Camera();
		serverPlayerObject->camObject = cameraObject;

		clientPlayerObject->camObject->position.z = 100;
		//Changing value from -100 to -30 to implement 3rd person camera
		serverPlayerObject->camObject->position.z = -30;
		cameraObject->position.z = serverPlayerObject->camObject->position.z;

		eae6320::Math::cQuaternion rotation = eae6320::Math::cQuaternion(600, eae6320::Math::cVector(0.0f, 1.0f, 0.0f));
		serverPlayerObject->camObject->updateRotation(rotation);

	}

	//objects.push_back(collisionLayerObject);

	
}


bool CreateMainWindow( const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState )
{
	// Every window that Windows creates must belong to a "class".
	// Note that this is different than a C++ class (but similar in theory):
	// A windows class defines things that will be the same for every window
	// that belongs to the class.
	// (In other words, every window will have exactly one class,
	// but a windows class can have many windows.)
	// To create a new windows class it must be "registered" with Windows.
	ATOM mainWindowClass = RegisterMainWindowClass( i_thisInstanceOfTheProgram );
	if ( mainWindowClass != NULL )
	{
		eae6320::Graphics::SubmitSpriteObjects(gameSpriteObjects);

		eae6320::Graphics::Submit(objects);
		eae6320::Graphics::SubmitDebugObjects(debugBoxObjects, debugLineObjects, debugSphereObjects);



		s_mainWindow = CreateMainWindowHandle( i_thisInstanceOfTheProgram, i_initialWindowDisplayState );
		eae6320::Graphics::Initialize(s_mainWindow);

		if ( s_mainWindow == NULL )
		{
			goto OnError;
		}

		return true;

OnError:

		// Unregister the main window class
		{
			UnregisterMainWindowClass( i_thisInstanceOfTheProgram );
			mainWindowClass = NULL;
		}

		return false;
	}
	else
	{
		return false;
	}
}

int WaitForMainWindowToCloseAndReturnExitCode( const HINSTANCE i_thisInstanceOfTheProgram )
{
	// Wait for the main window to close
	int exitCode;
	bool wereThereErrors = WaitForMainWindowToClose( exitCode );



	// Clean up anything that was created/registered/initialized
	if ( OnMainWindowClosed( i_thisInstanceOfTheProgram ) )
	{
		return exitCode;
	}
	else
	{
		if ( wereThereErrors )
		{
			// If the program already had errors while waiting for the main window to close
			// the existing error exit code can be returned
			return exitCode;
		}
		else
		{
			// In a real program you might decide to define a special constant for this situation,
			// but for this example program -1 is a good arbitrary error code
			return -1;
		}
	}
}

// CreateMainWindow
//-----------------

HWND CreateMainWindowHandle( const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState )
{
	// Create the main window
	HWND mainWindow;
	{
		// The window's "caption"
		// (The text that is displayed in the title bar)
		const char* windowCaption;
		if(isServer)
			 windowCaption = "[Nidal]'s EAE6320 Game Server";
		else
			 windowCaption = "[Nidal]'s EAE6320 Game Client";

		// The window's style
		const DWORD windowStyle =
			// "Overlapped" is basically the same as "top-level"
			WS_OVERLAPPED
			// The caption is the title bar when in windowed-mode
			| WS_CAPTION
			// The window should never change dimensions, so only a minimize box is allowed
			| WS_MINIMIZEBOX
			// The system menu appears when you right-click the title bar
			| WS_SYSMENU;
		// The window's extended style
		const DWORD windowStyle_extended =
			// The following is a macro to make the extended style the default top-level look
			WS_EX_OVERLAPPEDWINDOW;
		// The width and height of the window.
		// A game cares about the width and height of the actual "client area",
		// which is the part of the window that doesn't include the borders and title bar;
		// this means that if we say that a game runs at a resolution of 800 x 600,
		// the actual window will be slightly bigger than that.
		// Initially, then, the window will be created with default values that Windows chooses
		// and then resized after creation)
		const int width = CW_USEDEFAULT;
		const int height = CW_USEDEFAULT;
		// The initial position of the window
		// (We don't care, and will let Windows decide)
		const int position_x = CW_USEDEFAULT;
		const int position_y = CW_USEDEFAULT;
		// Handle to the parent of this window
		// (Since this is our main window, it can't have a parent)
		const HWND hParent = NULL;
		// Handle to the menu for this window
		// (The main window won't have a menu)
		const HMENU hMenu = NULL;
		// Handle to the instance of the program that this window should be associated with
		const HINSTANCE hProgram = i_thisInstanceOfTheProgram;
		// Any arbitrary pointer can be associated with this window;
		// usually a program's own custom representation of the window will be used, like this:
		// void* userData = someCustomStructOrClass;
		// Since this example program is simple, though, no data will be associated:
		void* userData = NULL;

		// Ask Windows to create the specified window.
		// CreateWindowEx() will return a handle to the window,
		// which is what we'll use to communicate with Windows about this window
		mainWindow = CreateWindowEx( windowStyle_extended, s_mainWindowClass_name, windowCaption, windowStyle,
			position_x, position_y, width, height,
			hParent, hMenu, hProgram, userData );
		if ( mainWindow == NULL )
		{
			const char* errorCaption = "No Main Window";
			std::string errorMessage( "Windows failed to create the main window: " );
			errorMessage += eae6320::GetLastWindowsError();
			MessageBox( NULL, errorMessage.c_str(), errorCaption, MB_OK | MB_ICONERROR );
			return NULL;
		}
	}

	// Change the window's size based on the desired client area resolution
	{
		// In a real game these values would come from an external source
		// rather than be hard-coded
		const int desiredWidth = 800;
		const int desiredHeight = 600;

		// Calculate how much of the window is coming from the "non-client area"
		// (the borders and title bar)
		RECT windowCoordinates;
		struct
		{
			long width;
			long height;
		} nonClientAreaSize;
		{
			// Get the coordinates of the entire window
			if ( GetWindowRect( mainWindow, &windowCoordinates ) == FALSE )
			{
				std::string errorMessage( "Windows failed to get the coordinates of the main window: " );
				errorMessage += eae6320::GetLastWindowsError();
				MessageBox( NULL, errorMessage.c_str(), NULL, MB_OK | MB_ICONERROR );
				goto OnError;
			}
			// Get the dimensions of the client area
			RECT clientDimensions;
			if ( GetClientRect( mainWindow, &clientDimensions ) == FALSE )
			{
				std::string errorMessage( "Windows failed to get the dimensions of the main window's client area: " );
				errorMessage += eae6320::GetLastWindowsError();
				MessageBox( NULL, errorMessage.c_str(), NULL, MB_OK | MB_ICONERROR );
				goto OnError;
			}
			// Get the difference between them
			nonClientAreaSize.width = ( windowCoordinates.right - windowCoordinates.left ) - clientDimensions.right;
			nonClientAreaSize.height = ( windowCoordinates.bottom - windowCoordinates.top ) - clientDimensions.bottom;
		}
		// Resize the window
		{
			float a = desiredWidth + nonClientAreaSize.width;
			float b = desiredHeight + nonClientAreaSize.height;
			sampleSprite->aspect = a / b;
			BOOL shouldTheResizedWindowBeRedrawn = TRUE;
			if ( MoveWindow( mainWindow, windowCoordinates.left, windowCoordinates.top,
				desiredWidth + nonClientAreaSize.width, desiredHeight + nonClientAreaSize.height,
				shouldTheResizedWindowBeRedrawn ) == FALSE )
			{
				std::string errorMessage( "Windows failed to resize the main window: " );
				errorMessage += eae6320::GetLastWindowsError();
				MessageBox( NULL, errorMessage.c_str(), NULL, MB_OK | MB_ICONERROR );
				goto OnError;
			}
		}
	}

	// Display the window in the initial state that Windows requested
	ShowWindow( mainWindow, i_initialWindowDisplayState );


	return mainWindow;

OnError:

	if ( DestroyWindow( mainWindow ) == FALSE )
	{
		std::string errorMessage( "Windows failed to destroy the main window "
			"after an error in creation: " );
		errorMessage += eae6320::GetLastWindowsError();
		MessageBox( NULL, errorMessage.c_str(), NULL, MB_OK | MB_ICONERROR );
	}
	mainWindow = NULL;

	return NULL;
}

ATOM RegisterMainWindowClass( const HINSTANCE i_thisInstanceOfTheProgram )
{
	WNDCLASSEX wndClassEx = { 0 };
	wndClassEx.cbSize = sizeof( WNDCLASSEX );
	wndClassEx.hInstance = i_thisInstanceOfTheProgram;

	// The class's style
	wndClassEx.style = 0;
#ifdef EAE6320_PLATFORM_GL
	// Reserve a unique device context for this window for OpenGL's use
	wndClassEx.style |= CS_OWNDC;
#endif
	// The function that will process all of the messages
	// that Windows will send to windows of this class
	wndClassEx.lpfnWndProc = OnMessageReceived;
	// Extra bytes can be set aside in the class for user data
	wndClassEx.cbClsExtra = 0;
	// Extra bytes can be set aside for each window of this class,
	// but this is usually specified for each window individually
	wndClassEx.cbWndExtra = 0;
	// The large and small icons that windows of this class should use
	// (These can be found in the Resources folder; feel free to change them)
	wndClassEx.hIcon = LoadIcon( i_thisInstanceOfTheProgram, MAKEINTRESOURCE( IDI_BIG ) );
	wndClassEx.hIconSm = LoadIcon( i_thisInstanceOfTheProgram, MAKEINTRESOURCE( IDI_SMALL ) );
	// The cursor that should display when the mouse pointer is over windows of this class
	wndClassEx.hCursor = LoadCursor( NULL, IDC_ARROW );
	// The "brush" that windows of this class should use as a background
	// (Setting this is a bit confusing but not important,
	// so don't be alarmed if the next line looks scary)
	wndClassEx.hbrBackground = reinterpret_cast<HBRUSH>( IntToPtr( COLOR_BACKGROUND + 1 ) );
	// A menu can be specified that all windows of this class would use by default,
	// but usually this is set for each window individually
	wndClassEx.lpszMenuName = NULL;
	// The class name (see comments where this is initialized)
	wndClassEx.lpszClassName = s_mainWindowClass_name;

	// Now all of the above information is given to Windows.
	// If all goes well, the class will be successfully registered
	// and it can be specified by name when creating the main window.
	const ATOM mainWindowClass = RegisterClassEx( &wndClassEx );
	if ( mainWindowClass == NULL )
	{
		const char* errorCaption = "No Main Window Class";
		std::string errorMessage( "Windows failed to register the main window's class: " );
		errorMessage += eae6320::GetLastWindowsError();
		MessageBox( NULL, errorMessage.c_str(), errorCaption, MB_OK | MB_ICONERROR );
	}
	return mainWindowClass;
}

// WaitForMainWindowToCloseAndReturnExitCode
//------------------------------------------

bool CleanupMainWindow()
{
	if ( s_mainWindow != NULL )
	{
		if ( DestroyWindow( s_mainWindow ) != FALSE )
		{
			s_mainWindow = NULL;
		}
		else
		{
			const char* errorCaption = "Couldn't Destroy Main Window";
			std::string errorMessage( "Windows failed to destroy the main window: " );
			errorMessage += eae6320::GetLastWindowsError();
			MessageBox( NULL, errorMessage.c_str(), errorCaption, MB_OK | MB_ICONERROR );
			return false;
		}
	}
	return true;
}

bool OnMainWindowClosed( const HINSTANCE i_thisInstanceOfTheProgram )
{
	bool wereThereErrors = false;

	if ( !CleanupMainWindow() )
	{
		wereThereErrors = true;
	}

	if ( !UnregisterMainWindowClass( i_thisInstanceOfTheProgram ) )
	{
		wereThereErrors = true;
	}

	return !wereThereErrors;
}

LRESULT CALLBACK OnMessageReceived( HWND i_window, UINT i_message, WPARAM i_wParam, LPARAM i_lParam )
{
	// DispatchMessage() will send messages to the main window here.
	// There are many messages that get sent to a window,
	// but games can ignore most of them
	// and let Windows handle them in the default way.

	// Process any messages that the game cares about
	// (any messages that are processed here should return a value
	// rather than letting the default processing function try to handle them a second time)
	switch( i_message )
	{
	// A key has been pressed down, and this is the translated character
	case WM_CHAR:
		{
			// This isn't usually the best way to handle keyboard input in a real-time game,
			// but it is a convenient way to handle an exit key
			// (The WPARAM input parameter indicates which key was pressed,
			// but this example program only cares about the escape key)
			if ( i_wParam == VK_ESCAPE )
			{
				// You do _not_ need to do this in your game,
				// but this example program shows you how to display a "message box"
				// that asks the user for confirmation
				int result;
				{
					const char* caption = "Exit Program?";
					const char* message = "Are you sure you want to quit?";
					result = MessageBox( s_mainWindow, message, caption, MB_YESNO | MB_ICONQUESTION );
				}
				if ( result == IDYES )
				{
					// Instruct Windows to send a WM_QUIT message
					{
						// The exit code is ignored,
						// but just as an example of what's possible a different exit code is sent here
						// than when the user closes the program in the usual way
						// (by clicking the X in the upper-right hand corner).
						int exitCode = 1;
						PostQuitMessage( exitCode );
					}

					// For WM_CHAR messages, return 0 to indicate that it was processed
					return 0;
				}
				else
				{
					// If the user doesn't select "Yes" to quit
					// then do nothing to keep running the program
				}
			}

			// If the key press wasn't handled pass it on to Windows to process in the default way
			break;
		}
	// The window's nonclient area is being destroyed
	case WM_NCDESTROY:
		{
			// This is the last message a window will receive
			// (Any child windows have already been destroyed).
			// After this message has been processed the window's handle will be invalid:
			s_mainWindow = NULL;

			// When the main window is destroyed
			// a WM_QUIT message should be sent
			// (if this isn't done the application would continue to run with no window).
			// This is where the exitCode in WaitForShutdown() comes from:
			int exitCode = 0;	// Arbitrary de facto success code
			PostQuitMessage( exitCode );	// This sends a WM_QUIT message

			// For WM_NCDESTROY messages, return 0 to indicate that it was processed
			return 0;
		}
	}

	// Pass any messages that weren't handled on to Windows
	return DefWindowProc( i_window, i_message, i_wParam, i_lParam );
}

bool UnregisterMainWindowClass( const HINSTANCE i_thisInstanceOfTheProgram )
{
	if ( UnregisterClass( s_mainWindowClass_name, i_thisInstanceOfTheProgram ) != FALSE )
	{
		return true;
	}
	else
	{
		const char* errorCaption = "Couldn't Unregister Main Window Class";
		std::string errorMessage( "Windows failed to unregister the main window's class: " );
		errorMessage += eae6320::GetLastWindowsError();
		MessageBox( NULL, errorMessage.c_str(), errorCaption, MB_OK | MB_ICONERROR );
		return false;
	}
}

void updateDebugMenu()
{


}
bool UpdateEntities_vector()
{
	bool wereThereErrors = false;

	RECT rect;
	GetWindowRect(s_mainWindow, &rect);
	eae6320::Time::OnNewFrame();

	 //eae6320::Math::cVector offset(0.0f, 0.0f,0.0f);
	 eae6320::Math::cVector forwardVector(0.0f, 0.0f, 0.0f);
	 eae6320::Math::cQuaternion rotation;
	 eae6320::Math::cVector rotationOffset(0.0f, 0.0f, 0.0f);


	 //Animation 
	 {

		 //if (eae6320::UserInput::IsKeyPressed('B'))
			// sampleSprite->animationIndex = 1;
		 if (eae6320::UserInput::IsKeyPressed(VK_NUMPAD0) || eae6320::UserInput::IsKeyPressed(0x30))
		 {
				sampleSprite->animationIndex = 0;
		 }
		 if (eae6320::UserInput::IsKeyPressed(VK_NUMPAD1) || eae6320::UserInput::IsKeyPressed(0x31))
		 {
			 sampleSprite->animationIndex = 1;

		 }
		 if (eae6320::UserInput::IsKeyPressed(VK_NUMPAD2) || eae6320::UserInput::IsKeyPressed(0x32))
		 {
			 sampleSprite->animationIndex = 2;

		 }
		 if (eae6320::UserInput::IsKeyPressed(VK_NUMPAD3) || eae6320::UserInput::IsKeyPressed(0x33))
		 {
			 sampleSprite->animationIndex = 3;

		 }

		 if (eae6320::UserInput::IsKeyPressed(VK_NUMPAD4) || eae6320::UserInput::IsKeyPressed(0x34))
		 {
			 sampleSprite->animationIndex = 4;
		 }
		 if (eae6320::UserInput::IsKeyPressed(VK_NUMPAD5) || eae6320::UserInput::IsKeyPressed(0x35))
		 {
			 sampleSprite->animationIndex = 5;

		 }
		 if (eae6320::UserInput::IsKeyPressed(VK_NUMPAD6) || eae6320::UserInput::IsKeyPressed(0x36))
		 {
			 sampleSprite->animationIndex = 6;

		 }
		 if (eae6320::UserInput::IsKeyPressed(VK_NUMPAD7) || eae6320::UserInput::IsKeyPressed(0x37))
		 {
			 sampleSprite->animationIndex = 7;

		 }
		 if (eae6320::UserInput::IsKeyPressed(VK_NUMPAD8) || eae6320::UserInput::IsKeyPressed(0x38))
		 {
			 sampleSprite->animationIndex = 8;

		 }
		 if (eae6320::UserInput::IsKeyPressed(VK_NUMPAD9) || eae6320::UserInput::IsKeyPressed(0x39))
		 {
			 sampleSprite->animationIndex = 9;

		 }
		 sampleSprite->rct.left = 51.2 * sampleSprite->animationIndex;
		 sampleSprite->rct.right = 51.2 * (sampleSprite->animationIndex + 1);
		 sampleSprite->rct.top = 0;
		 sampleSprite->rct.bottom = 64;

	 }
	{
		// Get the direction
		{
			if (!debugMenu.isMenuActive)
			{
				if (isServer && connectionEstablished)
				{
					//I am server
					clientPlayerObject->camObject->position.x = data.x;
					clientPlayerObject->camObject->position.y = data.y;
					clientPlayerObject->camObject->position.z = data.z;

					clientPlayerObject->camObject->rotation.m_w = data.m_w;
					clientPlayerObject->camObject->rotation.m_x = data.m_x;
					clientPlayerObject->camObject->rotation.m_y = data.m_y;
					clientPlayerObject->camObject->rotation.m_z = data.m_z;
				}
				else if(!isServer && connectionEstablished) {
					//I am client
					serverPlayerObject->camObject->position.x = data.x;
					serverPlayerObject->camObject->position.y = data.y;
					serverPlayerObject->camObject->position.z = data.z;

					serverPlayerObject->camObject->rotation.m_w = data.m_w;
					serverPlayerObject->camObject->rotation.m_x = data.m_x;
					serverPlayerObject->camObject->rotation.m_y = data.m_y;
					serverPlayerObject->camObject->rotation.m_z = data.m_z;
				}

				if (isServer) {
					if (eae6320::UserInput::IsKeyPressed('F'))
					{
						rotationOffset.y += 0.01f;

					}
					if (eae6320::UserInput::IsKeyPressed('G'))
					{
						rotationOffset.y -= 0.01f;
					}

					if (eae6320::UserInput::IsKeyPressed('A'))
					{
						acceleration.x = 0;

						acceleration.x -= 10.0f;
					}
					if (eae6320::UserInput::IsKeyPressed('D'))
					{
						acceleration.x += 10.0f;

					}
					if (eae6320::UserInput::IsKeyPressed('W'))
					{
						acceleration.z -= 10.0f;
					}

					if (eae6320::UserInput::IsKeyPressed('S'))
					{
						acceleration.z += 10.0f;

					}
				}
				else {

					if (eae6320::UserInput::IsKeyPressed('K'))
					{
						rotationOffset.y += 0.01f;

					}
					if (eae6320::UserInput::IsKeyPressed('L'))
					{
						rotationOffset.y -= 0.01f;
					}

					if (eae6320::UserInput::IsKeyPressed(VK_LEFT))
					{
						acceleration.x = 0;

						acceleration.x -= 10.0f;
					}
					if (eae6320::UserInput::IsKeyPressed(VK_RIGHT))
					{
						acceleration.x += 10.0f;

					}
					if (eae6320::UserInput::IsKeyPressed(VK_UP))
					{
						acceleration.z -= 10.0f;
					}

					if (eae6320::UserInput::IsKeyPressed(VK_DOWN))
					{
						acceleration.z += 10.0f;
					}
				}
				

				if (eae6320::UserInput::IsKeyPressed('Z'))
				{
					acceleration.y += 10.0f;
				}
				if (eae6320::UserInput::IsKeyPressed('X'))
				{
					acceleration.y -= 10.0f;
				}
			}
		}

	/*	acceleration -= eae6320::Math::cVector(velocity.x * 4.0f, velocity.y * 0.2f, velocity.z * 4.0f);
		velocity += acceleration * eae6320::timeLeftInFrame;
		position += velocity + acceleration*eae6320::timeLeftInFrame*eae6320::timeLeftInFrame*0.5;*/

		// Get the speed
		const float unitsPerSecond = 300.0f;	// This is arbitrary
		const float unitsToMove = unitsPerSecond * eae6320::Time::GetSecondsElapsedThisFrame();	// This makes the speed frame-rate-independent																				// Normalize the offset
		//offset *= unitsToMove;
		acceleration *= unitsToMove/100.0f;
		rotationOffset *= unitsToMove;
		rotation = eae6320::Math::cQuaternion(rotationOffset.y, eae6320::Math::cVector(0.0f, 1.0f, 0.0f));

	}
	

	if (isServer) 
	{
		/*serverPlayerObject->camObject->updatePosition(acceleration);*/
		//serverPlayerObject->setOffsetPosition(acceleration);

		serverPlayerObject->camObject->updateRotation(rotation);


		// Updating player's position
		eae6320::Math::cVector oldPosition = serverPlayerObject->position;
		acceleration.y = 0.0f;
		eae6320::Math::cMatrix_transformation i_localToWorldTransformPlayer = eae6320::Math::cMatrix_transformation(
			serverPlayerObject->camObject->rotation , serverPlayerObject->position);
		eae6320::Math::cVector newPosition = eae6320::Math::cMatrix_transformation::matrixMulVector(i_localToWorldTransformPlayer, -acceleration);
		serverPlayerObject->position.z = newPosition.z;

		//float difference = serverPlayerObject->position.z - serverPlayerObject->camObject->position.z;
			//serverPlayerObject->camObject->updatePosition(acceleration);
			// Updating third person camera according to the player's position
		eae6320::Math::cMatrix_transformation i_localToWorldTransformCamera = eae6320::Math::cMatrix_transformation(
				serverPlayerObject->camObject->rotation, serverPlayerObject->camObject->position);
		eae6320::Math::cVector camOffset = serverPlayerObject->position - serverPlayerObject->camObject->position ;
		eae6320::Math::cVector val = eae6320::Math::cMatrix_transformation::matrixMulVector(i_localToWorldTransformCamera, -camOffset);
		serverPlayerObject->camObject->position += (val - serverPlayerObject->camObject->position) * eae6320::Time::GetSecondsElapsedThisFrame() * 1.f;
		//serverPlayerObject->camObject->position.y += 3;
		//serverPlayerObject->camObject->position.z += 12;

		if(connectionEstablished)
			clientPlayerObject->camObject->updateRotation(clientPlayerObject->camObject->rotation);
	}
	else
	{
		clientPlayerObject->camObject->updateRotation(rotation);
		clientPlayerObject->camObject->updatePosition(acceleration);
		if (connectionEstablished)
			serverPlayerObject->camObject->updateRotation(serverPlayerObject->camObject->rotation);
	}

	eae6320::Time::OnNewFrame();


	return !wereThereErrors;
}
bool WaitForMainWindowToClose( int& o_exitCode )
{

	hDeviceContextHandle = BeginPaint(s_mainWindow, &OurPaintStructure);
	GetClientRect(s_mainWindow, &ClientRectangle);
	std::string networkMessage;
	
	// Any time something happens that Windows cares about, it will send the main window a message.

	// One of the messages it sends is that the application should quit;
	// this can be sent when a user closes the window
	// (e.g. presses the X in the upper-right corner),
	// but we can also tell Windows to send a quit message if the user chooses to quit from within the game
	// (e.g. from an in-game menu)
	for (DWORD i = 0; i < 8; ++i)
	{
		s_direct3dDevice->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		s_direct3dDevice->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		s_direct3dDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	}
	 debugMenuObj = DebugMenu();

	// Enter an infinite loop that will continue until a quit message (WM_QUIT) is received from Windows
	MSG message = { 0 };

	if (isServer)
		networkMessage = "Server Started :";
	else
		networkMessage = "Client Started :";

	DrawText(hDeviceContextHandle, networkMessage.c_str(), -1, &ClientRectangle, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	//clientPlayerObject->position.y = cameraObject->position.y + 20;
	do
	{

		// To send us a message, Windows will add it to a queue.
		// Most Windows applications should wait until a message is received and then react to it.
		// Real-time programs, though, must continually draw new images to the screen as fast as possible
		// and only pause momentarily when there is a Windows message to deal with.
		//eae6320::Graphics::Render();
		// This means that the first thing that must be done every iteration of the game loop is to "peek" at the message queue
		// and see if there are any messages from Windows that need to be handled
		bool hasWindowsSentAMessage;
		{
			HWND getMessagesFromAnyWindowBelongingToTheCurrentThread = NULL;
			unsigned int getAllMessageTypes = 0;
			unsigned int ifAMessageExistsRemoveItFromTheQueue = PM_REMOVE;
			hasWindowsSentAMessage = PeekMessage( &message, getMessagesFromAnyWindowBelongingToTheCurrentThread,
				getAllMessageTypes, getAllMessageTypes, ifAMessageExistsRemoveItFromTheQueue ) == TRUE;
		}
		if ( !hasWindowsSentAMessage )
		{
			// Usually there will be no messages in the queue, and the game can run
			printf("");
			
			
			eae6320::Time::OnNewFrame();

			if (lastUpdateTimer == 0.f || eae6320::Time::GetTotalSecondsElapsed() >= lastUpdateTimer + FrameTime)
			{
				eae6320::handleCollisions(eae6320::Time::GetTotalSecondsElapsed() - lastUpdateTimer);
				lastUpdateTimer = eae6320::Time::GetTotalSecondsElapsed();
			}
			eae6320::Graphics::SubmitSpriteObjects(gameSpriteObjects);
			if (connectionEstablished) {

				if (isServer) 
				{
					if (std::find(objects.begin(), objects.end(), clientPlayerObject) != objects.end())
					{

					}
					else {
						objects.push_back(clientPlayerObject);
						clientPlayerObject->material->Init(context);
						clientPlayerObject->mesh->LoadMesh(context);
						clientPlayerObject->material->effect->LoadVertexShader(context);
						clientPlayerObject->material->effect->LoadFragmentShader(context);
					}
				}
				else {
					if (std::find(objects.begin(), objects.end(), serverPlayerObject) != objects.end())
					{

					}
					else {
						objects.push_back(serverPlayerObject);
						serverPlayerObject->material->Init(context);
						serverPlayerObject->mesh->LoadMesh(context);
						serverPlayerObject->material->effect->LoadVertexShader(context);
						serverPlayerObject->material->effect->LoadFragmentShader(context);
					}
								
				}
				eae6320::Graphics::Submit(objects);
			}

			if(isServer)
				networkMessage = "Server Started :";
			else
					networkMessage = "Client Started :";
			
			if (connectionEstablished) {
				if(isServer)
					networkMessage = "Connection Established with Client";
				else
					networkMessage = "Connection Established with Server";

			}
			DrawText(hDeviceContextHandle, networkMessage.c_str(), -1, &ClientRectangle, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

#ifdef _DEBUG
			if (eae6320::UserInput::IsKeyPressed(VK_LEFT))
			{
				switch ((int)debugMenu.selectedMenu)
				{
				case 3:
					if (!hoverOnSlider) 
					{
						if (!isSliderSelected)
						{
							isSliderSelected = true;
							debugMenuObj.sliderSelected = true;
							s_debugSphere1->m_radius -= debugMenu.sliderCurrentPosition * 0.3f;
						}
						else
						{
							isSliderSelected = false;
							debugMenuObj.sliderSelected = false;
						}
					}

					if (s_debugSphere1->m_radius >= 500.0f)
						s_debugSphere1->m_radius = 500.0f;

					//if (gameObjects[0]->camObject->isFlying) {
						s_debugSphere2->m_radius = 30.0f;
						s_debugSphere2->LoadDebugSphere();
					//}

					if (isSliderSelected)
					{
						s_debugSphere1->LoadDebugSphere();
						debugMenuObj.sliderCurrentPosition--;
					}

					if (debugMenuObj.sliderCurrentPosition <= 0)
						debugMenuObj.sliderCurrentPosition = 0;
					break;
				default:
					break;
				}
				hoverOnSlider = true;
			}
			else {
				hoverOnSlider = false;
			}
			
			
			
			if (eae6320::UserInput::IsKeyPressed(VK_RIGHT))
			{
				switch ((int)debugMenuObj.selectedMenu)
				{
				case 1:
					if (!hoverOnButton) 
					{
						if (!isButtonSelected) 
						{
							isButtonSelected = true;
							debugMenuObj.buttonSelected = true;
						}
						else {
							isButtonSelected = false;
							debugMenuObj.buttonSelected = false;

						}
					}
					if (debugMenuObj.buttonSelected)
					{
						s_debugSphere1->m_radius = 30.0f;
						s_debugSphere1->LoadDebugSphere();
					}
					break;
					case 2:
						if (!hoverOnCheckBox) {
							if (!isCheckBoxSelected) {
								isCheckBoxSelected = true;
								debugMenuObj.checkBoxChecked = true;
							}
							else {
								isCheckBoxSelected = false;
								debugMenuObj.checkBoxChecked = false;

							}
						}
						isSliderSelected = false;
						break;
				case 3:
					if (!hoverOnSlider) {
						if (!isSliderSelected) {
							isSliderSelected = true;
							debugMenuObj.sliderSelected = true;
							if (s_debugSphere1->m_radius <= 85.0f)
							 s_debugSphere1->m_radius += debugMenu.sliderCurrentPosition * 0.3f;

						}
						else {
							isSliderSelected = false;
							debugMenuObj.sliderSelected = false;

						}
					}

					if (s_debugSphere1->m_radius >= 500.0f)
						s_debugSphere1->m_radius = 500.0f;
					if (isSliderSelected) {
						s_debugSphere1->LoadDebugSphere();
						debugMenuObj.sliderCurrentPosition++;
					}

					if (debugMenuObj.sliderCurrentPosition >= 19)
						debugMenuObj.sliderCurrentPosition = 19;
					break;
				default:
					break;
				}
				hoverOnSlider = true;
				hoverOnCheckBox = true;

			}
			else
			{
				hoverOnSlider = false;
				hoverOnCheckBox = false;
			}


			if (eae6320::UserInput::IsKeyPressed(VK_UP))
			{

				if (!hoveringUp) {
					if (!upSelected) {
						upSelected = true;
						debugMenuObj.selectedMenu -= 0.5;
						if (debugMenuObj.selectedMenu < 0)
							debugMenuObj.selectedMenu = 0;
					}
					else {
						upSelected = false;

					}
				}
			
			}
			else {
				hoveringUp = false;
			}
			if (eae6320::UserInput::IsKeyPressed(VK_DOWN))
			{

				if (!hoveringDown) {
					if (!downSelected) {
						downSelected = true;
						debugMenuObj.selectedMenu += 0.5;
						if (debugMenuObj.selectedMenu > 3)
							debugMenuObj.selectedMenu = 3;
					}
					else {
						downSelected = false;

					}
				}
				
			}
			else {
				hoveringDown = false;

			}
		/*	if (gameObjects[0]->camObject->isFlying)
			{*/
			/*	s_debugSphere2->m_radius = 30.0f;
				s_debugSphere2->LoadDebugSphere();*/
			//}
			eae6320::Graphics::SubmitDebugObjects(debugBoxObjects, debugLineObjects, debugSphereObjects);
			eae6320::Graphics::SubmitDebugMenu(debugMenuObj);
#endif
			//updateDebugMenu();
			updateNetwork();
			eae6320::Graphics::Render();
			if (!eae6320::Graphics::isDebugMenuActive)
				UpdateEntities_vector();

		}
		else
		{
			TranslateMessage( &message );

			DispatchMessage( &message );
		}
	} while ( message.message != WM_QUIT );
	objects.clear();
	debugBoxes.clear();
	debugBoxObjects.clear();
	eae6320::Graphics::ShutDown();
	destroyNetwork();
	// The exit code for the application is stored in the WPARAM of a WM_QUIT message
	o_exitCode = static_cast<int>( message.wParam );

	return true;
}
