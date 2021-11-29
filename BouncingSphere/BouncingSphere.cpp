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
#include <string>
#include <vector>

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif

#define GRAVITY 10

struct Obstacle {
	Referential ref;
	Vector3 ext;
	float r;
	Color color;

	BoxRounded withRadius(float r) {
		return { this->ref, this->ext, this->r + r };
	}

	void draw() {
		if (this->color.a == 0)
			return;
		BoxRounded{ this->ref, this->ext, this->r }.draw(this->color);
	}
};

Obstacle NewObstacle(Vector3 pos) {
	return {
		localReferential(pos, QuaternionFromAxisAngle({ random(), random(), random() }, random())),
		{ random() * 1.5f, random(), random() * 1.5f },
		0.25f + random() / 4,
		ORANGE
	};
}

typedef std::vector<Obstacle> Obstacles;
typedef std::vector<BoxRounded> Boxes;

struct Ball {
	float r;
	Vector3 pos;
	Vector3 motion;
	Quaternion rotation;
	Color color;

	void draw() {
		Sphere{ this->pos, this->r }.draw(this->rotation, this->color);
	}
};

Ball NewBall(Vector3 pos, Color color) {
	return {
		0.75f + random() / 2,
		pos,
		Vector3Scale(Vector3Normalize({ randPos(), 9 * random() / 10 - 1, randPos() }), 5 + 3 * random()),
		QuaternionIdentity(),
		color
	};
}

bool Collide(Vector3 a, Vector3 b, Boxes boxes, Ball* ball, int except) {
	Segment segment = { a, b };
	int count = boxes.size();
	for (int i = 0; i < count; i++) {
		Vector3 interPt;
		Vector3 interNormal;
		if (i != except && IntersectSegmentBoxRounded(segment, boxes[i], interPt, interNormal)) {
			Vector3 c = Vector3Reflect(Vector3Subtract(b, interPt), interNormal);
			ball->motion = Vector3Reflect(ball->motion, interNormal);
			Collide(interPt, Vector3Add(interPt, c), boxes, ball, i);
			return true;
		}
	}
	ball->pos = b;
	return false;
}

bool MoveBall(Ball* ball, Obstacles obstacles, float dt) {
	Vector3 b = Vector3Add(ball->pos, Vector3Scale(ball->motion, dt));
	int count = obstacles.size();
	Boxes boxes(count);
	for (int i = 0; i < count; i++)
		boxes[i] = obstacles[i].withRadius(ball->r);
	return Collide(ball->pos, b, boxes, ball, -1);
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

	InitAudioDevice();
	Sound sounds[4] = {
		LoadSound("sounds/1.mp3"),
		LoadSound("sounds/2.mp3"),
		LoadSound("sounds/3.mp3"),
		LoadSound("sounds/4.mp3")
	};
	SetMasterVolume(0.25);

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

	Ball ball = NewBall({ 0, 0, 0 }, BLUE);
	Obstacles obstacles(6);
	obstacles[0] = { localReferential({ 0, -10.5, 0 }, QuaternionIdentity()), { 10, 0.5, 10 }, 0, PINK };
	obstacles[1] = { localReferential({ 0, 10.5, 0 }, QuaternionIdentity()), { 10, 0.5, 10 }, 0, { 0, 0, 0, 0 } };
	obstacles[2] = { localReferential({ -10.5, 0, 0 }, QuaternionIdentity()), { 0.5, 10, 10 }, 0, PINK };
	obstacles[3] = { localReferential({ 10.5, 0, 0 }, QuaternionIdentity()), { 0.5, 10, 10 }, 0, PINK };
	obstacles[4] = { localReferential({ 0, 0, -10.5 }, QuaternionIdentity()), { 10, 10, 0.5 }, 0, PINK };
	obstacles[5] = { localReferential({ 0, 0, 10.5 }, QuaternionIdentity()), { 10, 10, 0.5 }, 0, PINK };

	for (int x = -5; x <= 5; x += 5)
		for (int z = -5; z <= 5; z += 5)
			obstacles.push_back(NewObstacle({ (float) x, -5, (float) z }));

	//ball.r = 1;
	//ball.pos = { 2.847386, -4.941321, -4.071001 };
	//ball.motion = { 8.174915, 3.608876, -6.956378 };
	//obstacles.push_back({ localReferential({ 0, 0, 0 }, QuaternionIdentity()), { 2, 2, 2 }, 0, ORANGE });

	// Main game loop
	while (!WindowShouldClose()) { // Detect window close button or ESC key
		// Update
		float deltaTime = GetFrameTime();
		// float time = static_cast<float>(GetTime());

		MyUpdateOrbitalCamera(&camera, deltaTime);

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(RAYWHITE);

		BeginMode3D(camera);
		{
			//3D REFERENTIAL
			/*DrawGrid(20, 1.0f); // Draw a grid
			DrawLine3D({  0 }, { 0, 10, 0 }, DARKGRAY);
			DrawSphere({ 10, 0, 0 }, .2f, RED);
			DrawSphere({ 0, 10, 0 }, .2f, GREEN);
			DrawSphere({ 0, 0, 10 }, .2f, BLUE);*/
		
			//Quaternion qOrient = QuaternionFromAxisAngle(Vector3Normalize({ 1, 3, -4 }), time);
			//MyDrawSphere(qOrient, { 0, 2, 0 }, 3, 20, 20, BLUE);
			// MyDrawSphereWiresEx2(qOrient, { 0, 2, 0 }, 3, 20, 20, BLACK);
			
			// Sphere sphere = { { 0, 1, 2 }, 3 };
			// sphere.draw(qOrient, BLUE);
			/*Cylinder cylinder = { { 0, 1, 2 }, { 1, 4, 3 }, 3 };
			cylinder.draw(cylinder.quaternionFromAxisAngle(time), BLUE, CYLINDER_CAPS_ROUNDED);
			Segment segment = { { 4, 6, -1 - 5 * sinf(time / 6) }, { -3, -1, 0 } };
			segment.draw();
			Vector3 interPt;
			Vector3 interNormal;
			// if (IntersectSegmentSphere(segment, sphere, interPt, interNormal)) {
			if (IntersectSegmentCylinderRounded(segment, cylinder, interPt, interNormal)) {
				dist = Vector3Distance(cylinder.pt2, interPt);
				MyDrawSphere(QuaternionIdentity(), interPt, .1f, 10, 10, RED);
				DrawLine3D(interPt, Vector3Add(interPt, interNormal), GREEN);
			}

			Vector3 top = Vector3Add(cylinder.pt1, Vector3Scale(cylinder.axisNormalized(), cylinder.r + Vector3Length(cylinder.axis())));
			DrawSphere(top, 0.5, RED);*/
			//Quaternion q = QuaternionFromAxisAngle(Vector3Normalize({ 1, 3, -4 }), time);
			//Quaternion q = QuaternionIdentity();
			//MyDrawDisk(q, { 0, 2, 0 }, 3, 20, GREEN); 
			//MyDrawDisk({0} ,interPt ,.2f ,10,PINK);
			//MyDrawDiskWires(q,{0,2,0},3,20,BLACK);

			//MyDrawSpherePortion(q,{0,2,0},3,0,PI,20,0,PI/2,20,RED);
			//MyDrawSphereWiresPortion(q,{0,2,0},3,0,PI,20,0,PI/2,20,BLACK);

			//MyDrawCylinderPortion(q, { 0, 2, 0 }, { 0, 6, 0 }, PI, PI / 2, PI, 20, true, PINK);
			//MyDrawCylinderWiresPortion(q, { 0, 2, 0 }, { 0, 6, 0 }, PI, PI / 2, PI, 20, true, BLACK);

			/*float r = 4;
			//Sphere sphere = { { 0, 0, 0}, r };
			//sphere.draw(QuaternionIdentity(), BLUE);
			MyDrawSpherePortion(QuaternionIdentity(), { 0, 0, 0 }, r, 0, 2 * PI, 40, 0, PI / 2, 10, BLUE);
			MyDrawSphereWiresPortion(QuaternionIdentity(), { 0, 0, 0 }, r, 0, 2 * PI, 40, 0, PI / 2, 10, DARKGRAY);
			DrawSphere({ r, 0, 0 }, 0.05, BLACK);
			DrawSphere({ 0, r, 0 }, 0.05, BLACK);
			DrawSphere({ 0, 0, r }, 0.05, BLACK);
			DrawSphere({ -r, 0, 0 }, 0.05, BLACK);
			DrawSphere({ 0, -r, 0 }, 0.05, BLACK);
			DrawSphere({ 0, 0, -r }, 0.05, BLACK);*/

			/*BoxRounded box = { localReferential({ 2, 3, -1 }, QuaternionFromAxisAngle({ 4, 9, 2 }, PI / 3)), { 0.5, 1.25, 2 }, .5f };
			box.draw(BLUE);
			Segment segment = { { 4, 6, -3 - 4 * sinf(time / 6) }, { -3, -1, 0 } };
			segment.draw();
			Vector3 interPt;
			Vector3 interNormal;
			if (IntersectSegmentBoxRounded(segment, box, interPt, interNormal)) {
				MyDrawSphere(QuaternionIdentity(), interPt, .1f, 10, 10, RED);
				DrawLine3D(interPt, Vector3Add(interPt, interNormal), GREEN);
			}*/

			if (deltaTime > 0) {
				ball.motion.y -= GRAVITY * deltaTime;
				bool collide = MoveBall(&ball, obstacles, deltaTime);
				if (collide)
					PlaySoundMulti(sounds[rand() % 4]);
			}

            for (auto obstacle : obstacles)
                obstacle.draw();
            ball.draw();
		}
		EndMode3D();

		//DrawText(TextFormat("(%.02f %.02f %.02f) : %.02f", ball.pos.x, ball.pos.y, ball.pos.z, Vector3Length(ball.pos)), 10, 10, 20, BLACK);

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	for (auto sound : sounds)
		UnloadSound(sound);
	CloseAudioDevice();
	// De-Initialization
	//--------------------------------------------------------------------------------------   
	CloseWindow(); // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}
