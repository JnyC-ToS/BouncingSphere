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

float min(float a, float b) {
	return a < b ? a : b;
}

void MyDrawSphereEx2(Quaternion q, Vector3 centerPos, float radius, int nSegmentsTheta, int nSegmentsPhi, Color color);
void MyDrawSphereWiresEx2(Quaternion q, Vector3 centerPos, float radius, int nSegmentsTheta, int nSegmentsPhi, Color color);

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

	void draw() {
		DrawLine3D(this->pt1, this->pt2, BLACK);
		MyDrawSphereEx2({ 0 }, this->pt1, .05f, 10, 10, DARKGRAY);
		MyDrawSphereEx2({ 0 }, this->pt2, .03f, 10, 10, BLACK);
	}
};

struct Plane {
	Vector3 n;
	float d;
};

struct Sphere {
	Vector3 center;
	float r;

	void draw(Quaternion q, Color color) {
		MyDrawSphereEx2(q, this->center, this->r, 20, 20, color);
		MyDrawSphereWiresEx2(q, this->center, this->r, 20, 20, DARKGRAY);
	}
};

bool IntersectSegmentPlane(Segment segment, Plane plane, Vector3& interPt, Vector3& interNormal) {
	Vector3 ab = Vector3Subtract(segment.pt2, segment.pt1);
	float dotabn = Vector3DotProduct(ab, plane.n);
	if (approxZero(dotabn))
		return false;
	float t = (plane.d - Vector3DotProduct(segment.pt1, plane.n)) / dotabn;
	if (t < 0 || t > 1)
		return false;
	interPt = Vector3Add(segment.pt1, Vector3Scale(ab, t));
	if (dotabn < 0)
		interNormal = plane.n;
	else
		interNormal = Vector3Negate(plane.n);
	return true;
}

bool IntersectSegmentSphere(Segment segment, Sphere sphere, Vector3& interPt, Vector3& interNormal) {
	Vector3 ab = Vector3Subtract(segment.pt2, segment.pt1);
	Vector3 ca = Vector3Subtract(segment.pt1, sphere.center);
	float b = 2 * Vector3DotProduct(ab, ca);
	float a = Vector3DotProduct(ab, ab);
	float delta = b * b - 4 * a * (Vector3DotProduct(ca, ca) - sphere.r * sphere.r);
	if (delta < -EPSILON)
		return false;
	float t;
	if (delta < EPSILON)
		t = -b / (2 * a);
	else {
		float deltaSqrt = sqrtf(delta);
		t = min((-b - deltaSqrt) / (2 * a), (-b + deltaSqrt) / (2 * a));
	}
	if (t < 0 || t > 1)
		return false;
	interPt = Vector3Add(segment.pt1, Vector3Scale(ab, t));
	interNormal = Vector3Normalize(Vector3Subtract(interPt, sphere.center));
	return true;
}

void MyDrawSphereEx2(Quaternion q, Vector3 centerPos, float radius, int nSegmentsTheta, int nSegmentsPhi, Color color) {
	if (nSegmentsTheta < 3 || nSegmentsPhi < 2)
		return;

	std::vector<Vector3> vertexBufferTheta(nSegmentsTheta + 1);
	std::fill(vertexBufferTheta.begin(), vertexBufferTheta.end(), Vector3{ 0, 1, 0 });

	int numVertex = nSegmentsTheta * nSegmentsPhi * 6;
	if (rlCheckBufferLimit(numVertex))
		rlglDraw();

	rlPushMatrix();

	// NOTE: Transformation is applied in inverse order (scale -> translate)
	rlTranslatef(centerPos.x, centerPos.y, centerPos.z);

	// ROTATION
	Vector3 vect;
	float angle;
	QuaternionToAxisAngle(q, &vect, &angle);
	rlRotatef(angle * RAD2DEG, vect.x, vect.y, vect.z);
	rlScalef(radius, radius, radius);


	rlBegin(RL_TRIANGLES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	float deltaPhi = PI / nSegmentsPhi;
	float deltaTheta = 2 * PI / nSegmentsTheta;

	float phi = 0;
	for (int i = 0; i < nSegmentsPhi; i++) {
		float theta = 0;
		Vector3 tmpBottomLeft = Spherical{ 1, theta, phi + deltaPhi }.toCartesian();

		for (int j = 0; j < nSegmentsTheta; j++) {
			Vector3 topLeft = vertexBufferTheta[j];
			Vector3 bottomLeft = tmpBottomLeft;
			Vector3 topRight = vertexBufferTheta[j+1];
			Vector3 bottomRight = Spherical{ 1, theta + deltaTheta, phi + deltaPhi }.toCartesian();

			rlVertex3f(bottomLeft.x, bottomLeft.y, bottomLeft.z);
			rlVertex3f(topRight.x, topRight.y, topRight.z);
			rlVertex3f(topLeft.x, topLeft.y, topLeft.z);

			rlVertex3f(bottomLeft.x, bottomLeft.y, bottomLeft.z);
			rlVertex3f(bottomRight.x, bottomRight.y, bottomRight.z);
			rlVertex3f(topRight.x, topRight.y, topRight.z);

			theta += deltaTheta;

			vertexBufferTheta[j] = tmpBottomLeft;
			tmpBottomLeft = bottomRight;
		}
		vertexBufferTheta[vertexBufferTheta.size() - 1] = vertexBufferTheta[0];
		phi += deltaPhi;
	}
	rlEnd();
	rlPopMatrix();
}

void MyDrawSphereWiresEx2(Quaternion q, Vector3 centerPos, float radius, int nSegmentsTheta, int nSegmentsPhi, Color color) {
	if (nSegmentsTheta < 3 || nSegmentsPhi < 2)
		return;

	std::vector<Vector3> vertexBufferTheta(nSegmentsTheta + 1);
	std::fill(vertexBufferTheta.begin(), vertexBufferTheta.end(), Vector3{ 0, 1, 0 });

	int numVertex = nSegmentsTheta * nSegmentsPhi * 4;
	if (rlCheckBufferLimit(numVertex))
		rlglDraw();

	rlPushMatrix();
	// NOTE: Transformation is applied in inverse order (scale -> translate)
	rlTranslatef(centerPos.x, centerPos.y, centerPos.z);

	// ROTATION
	Vector3 vect;
	float angle;
	QuaternionToAxisAngle(q, &vect, &angle);
	rlRotatef(angle * RAD2DEG, vect.x, vect.y, vect.z);
	rlScalef(radius, radius, radius);

	rlBegin(RL_LINES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	float deltaPhi = PI / nSegmentsPhi;
	float deltaTheta = 2 * PI / nSegmentsTheta;

	float phi = 0;
	for (int i = 0; i < nSegmentsPhi; i++) {
		float theta = 0;

		for (int j = 0; j < nSegmentsTheta; j++) {
			Vector3 topLeft = vertexBufferTheta[j];
			Vector3 bottomLeft = Spherical{ 1, theta, phi + deltaPhi }.toCartesian();
			Vector3 topRight = vertexBufferTheta[j + 1];

			rlVertex3f(topLeft.x, topLeft.y, topLeft.z);
			rlVertex3f(topRight.x, topRight.y, topRight.z);

			rlVertex3f(topLeft.x, topLeft.y, topLeft.z);
			rlVertex3f(bottomLeft.x, bottomLeft.y, bottomLeft.z);

			theta += deltaTheta;

			vertexBufferTheta[j] = bottomLeft;
		}
		vertexBufferTheta[vertexBufferTheta.size() - 1] = vertexBufferTheta[0];
		phi += deltaPhi;
	}
	rlEnd();
	rlPopMatrix();
}

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
	Camera camera = {};
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
			//

			//3D REFERENTIAL
			DrawGrid(20, 1.0f); // Draw a grid
			DrawLine3D({  0 }, { 0, 10, 0 }, DARKGRAY);
			DrawSphere({ 10, 0, 0 }, .2f, RED);
			DrawSphere({ 0, 10, 0 }, .2f, GREEN);
			DrawSphere({ 0, 0, 10 }, .2f, BLUE);
			Quaternion qOrient = QuaternionFromAxisAngle(Vector3Normalize({ 1, 3, -4 }), time);
			// MyDrawSphereEx2(qOrient, { 0, 2, 0 }, 3, 20, 20, BLUE);
			// MyDrawSphereWiresEx2(qOrient, { 0, 2, 0 }, 3, 20, 20, BLACK);

			Sphere sphere = {{ 0, 1, 2 }, 3 };
			Segment segment = { { 5, 6, -1 - 3 * sinf(time / 10) }, { -3, -1, 0 } };
			sphere.draw(qOrient, BLUE);
			segment.draw();
			Vector3 interPt;
			Vector3 interNormal;
			if (IntersectSegmentSphere(segment, sphere, interPt, interNormal)) {
				MyDrawSphereEx2({ 0 }, interPt, .1f, 10, 10, RED);
				DrawLine3D(interPt, Vector3Add(interPt, interNormal), GREEN);
			}
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
