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
#include <raymath.h>
#include "rlgl.h"
#include <math.h>
#include <float.h>
#include <vector>

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

#define EPSILON 1.e-6f

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

bool approxZero(float val) {
	return fabs(val) < EPSILON;
}

struct Cylindrical {
	float rho;
	float theta;
	float y;

	inline Cylindrical operator +(Cylindrical other) {
		return { rho + other.rho, theta + other.theta, y + other.y };
	}

	Vector3 toCartesian() {
		return { rho * sinf(theta), y, rho * cosf(theta) };
	}
};

struct Spherical {
	float rho;
	float theta;
	float phi;

	inline Spherical operator +(Spherical other) {
		return { rho + other.rho, theta + other.theta, phi + other.phi };
	}

	Vector3 toCartesian() {
		return {
			rho * sinf(phi) * cosf(theta),
			rho * cosf(phi),
			rho * sinf(phi) * sinf(theta)
		};
	}
};

struct Cartesian {
	float x;
	float y;
	float z;

	inline Cartesian operator +(Cartesian other) {
		return { x + other.x, y + other.y, z + other.z };
	}

	Cylindrical toCylindrical() {
    	Cylindrical cyl;
    	cyl.rho = sqrtf(x * x + z * z);
    	cyl.y = y;

    	if (cyl.rho < EPSILON)
    		cyl.theta = 0;
    	else {
    		cyl.theta = atan2f(x, z);
    		if (cyl.theta < 0)
    			cyl.theta += PI * 2;
    	}
    	return cyl;
    }

	static Cartesian fromVector3(Vector3 vector) {
		return { vector.x, vector.y, vector.z };
	}
};

struct Segment {
	Vector3 pt1;
	Vector3 pt2;
};

struct Plane {
	Vector3 n;
	float d;
};

bool IntersectSegmentPlane(Segment segment, Plane plane, Vector3 &interPT, Vector3 &interN) {
	Vector3 ab = Vector3Subtract(segment.pt2, segment.pt1);
	float dotabn = Vector3DotProduct(ab,plane.n);
	if (approxZero(dotabn)) {
		return false;
	}
	float t = (plane.d - Vector3DotProduct(segment.pt1,plane.n))/dotabn;
	// TODO - Work in progress
}

void MyUpdateOrbitalCamera(Camera* camera, float deltaTime) {
	static Spherical sphPos = { 10, PI / 4, PI / 4 };
	const static Spherical sphSpeed = { 20, 0.3, 0.3 };
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
	newSphPos = { Clamp(newSphPos.rho, rhoMin, rhoMax), newSphPos.theta, Clamp(newSphPos.phi, PI / 100, .99 * PI) };
	sphPos = newSphPos;

	camera->position = sphPos.toCartesian();
}

int main(int argc, char* argv[]) {
	// Initialization
	//--------------------------------------------------------------------------------------
	float screenSizeCoef = .9f;
	const int screenWidth = 1920 * screenSizeCoef;
	const int screenHeight = 1080 * screenSizeCoef;

	InitWindow(screenWidth, screenHeight, "Bouncy Sphere");

	SetTargetFPS(60);

	//CAMERA
	Vector3 cameraPos = {8.0f, 15.0f, 14.0f};
	Camera camera = {};
	camera.position = cameraPos;
	camera.target = {0.0f, 0.0f, 0.0f};
	camera.up = {0.0f, 1.0f, 0.0f};
	camera.fovy = 45.0f;
	camera.type = CAMERA_PERSPECTIVE;
	SetCameraMode(camera, CAMERA_CUSTOM); // Set an orbital camera mode


	//--------------------------------------------------------------------------------------

	// Main game loop
	while (!WindowShouldClose()) // Detect window close button or ESC key
	{
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
			//

			//3D REFERENTIAL
			DrawGrid(20, 1.0f); // Draw a grid
			DrawLine3D({0}, {0, 10, 0}, DARKGRAY);
			DrawSphere({10, 0, 0}, .2f, RED);
			DrawSphere({0, 10, 0}, .2f, GREEN);
			DrawSphere({0, 0, 10}, .2f, BLUE);
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
