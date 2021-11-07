/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "Models.h"
#include "Drawing.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

void MyUpdateOrbitalCamera(Camera* camera, float deltaTime) {
	static Spherical sphPos = { 10, PI / 4, PI / 4 };
	const static Spherical sphSpeed = { 20, 0.3f, 0.3f };
	const float rhoMin = 2;
	const float rhoMax = 50;

	static Vector2 prevMousePos = { 0, 0 };
	Vector2 mousePos = GetMousePosition();
	Vector2 mouseVect = Vector2Subtract(mousePos, prevMousePos);
	prevMousePos = mousePos;

	bool mouseClicked = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
	Spherical sphDelta = {
		-GetMouseWheelMove() * sphSpeed.rho * deltaTime,
		mouseClicked ? mouseVect.x * sphSpeed.theta * deltaTime : 0,
		mouseClicked ? -mouseVect.y * sphSpeed.phi * deltaTime : 0
	};
	Spherical newSphPos = sphPos + sphDelta;
	newSphPos = { Clamp(newSphPos.rho, rhoMin, rhoMax), newSphPos.theta, Clamp(newSphPos.phi, PI / 100, .99f * PI) };
	sphPos = newSphPos;

	camera->position = sphPos.toCartesian();
}

int main(int argc, char* argv[]) {
	// Initialization
	//--------------------------------------------------------------------------------------
	float screenSizeCoef = .9f;
	const int screenWidth = (int) roundf(1920 * screenSizeCoef);
	const int screenHeight = (int) roundf(1080 * screenSizeCoef);

	InitWindow(screenWidth, screenHeight, "Bouncy Sphere");

	SetTargetFPS(60);

	//CAMERA
	Vector3 cameraPos = { 8.0f, 15.0f, 14.0f };
	Camera camera;
	camera.position = cameraPos;
	camera.target = { 0.0f, 0.0f, 0.0f };
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.type = CAMERA_PERSPECTIVE;
	SetCameraMode(camera, CAMERA_CUSTOM); // Set an orbital camera mode

	//--------------------------------------------------------------------------------------

	// Main game loop
	while (!WindowShouldClose()) { // Detect window close button or ESC key
		// Update
		//----------------------------------------------------------------------------------
		// TODO: Update your variables here
		//----------------------------------------------------------------------------------

		float deltaTime = GetFrameTime();
		float time = static_cast<float>(GetTime());

		MyUpdateOrbitalCamera(&camera, deltaTime);

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(RAYWHITE);

		BeginMode3D(camera);
		{
			//3D REFERENTIAL
			DrawGrid(20, 1.0f); // Draw a grid
			DrawLine3D({  0 }, { 0, 10, 0 }, DARKGRAY);
			DrawSphere({ 10, 0, 0 }, .2f, RED);
			DrawSphere({ 0, 10, 0 }, .2f, GREEN);
			DrawSphere({ 0, 0, 10 }, .2f, BLUE);
		
			//Quaternion qOrient = QuaternionFromAxisAngle(Vector3Normalize({ 1, 3, -4 }), time);
			//MyDrawSphere(qOrient, { 0, 2, 0 }, 3, 20, 20, BLUE);
			// MyDrawSphereWiresEx2(qOrient, { 0, 2, 0 }, 3, 20, 20, BLACK);
			
			// Sphere sphere = { { 0, 1, 2 }, 3 };
			// sphere.draw(qOrient, BLUE);
			/*Cylinder cylinder = { { 0, 1, 2 }, { 1, 4, 3 }, 3 };
			cylinder.draw(cylinder.quaternionFromAxisAngle(time), BLUE);
			Segment segment = { { 5, 6, -1 - 3 * sinf(time / 7) }, { -3, -1, 0 } };
			segment.draw();
			Vector3 interPt;
			Vector3 interNormal;
			// if (IntersectSegmentSphere(segment, sphere, interPt, interNormal)) {
			if (IntersectSegmentCylinderFinite(segment, cylinder, interPt, interNormal)) {
				MyDrawSphere({ 0 }, interPt, .1f, 10, 10, RED);
				DrawLine3D(interPt, Vector3Add(interPt, interNormal), GREEN);
			}*/
			//Quaternion q = QuaternionFromAxisAngle(Vector3Normalize({ 1, 3, -4 }), time);
			//Quaternion q = QuaternionIdentity();
			//MyDrawDisk(q, { 0, 2, 0 }, 3, 20, GREEN); 
			//MyDrawDisk({0} ,interPt ,.2f ,10,PINK);
			//MyDrawDiskWires(q,{0,2,0},3,20,BLACK);

			//MyDrawSpherePortion(q,{0,2,0},3,0,PI,20,0,PI/2,20,RED);
			//MyDrawSphereWiresPortion(q,{0,2,0},3,0,PI,20,0,PI/2,20,BLACK);

			//MyDrawCylinderPortion(q, { 0, 2, 0 }, { 0, 6, 0 }, PI, PI / 2, PI, 20, true, PINK);
			//MyDrawCylinderWiresPortion(q, { 0, 2, 0 }, { 0, 6, 0 }, PI, PI / 2, PI, 20, true, BLACK);

			MySuperTest();
		}
		EndMode3D();

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------   
	CloseWindow(); // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}
