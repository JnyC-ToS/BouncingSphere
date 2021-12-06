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
#define MASS 2

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
	Vector3 rotationAxis;
	float rotationAngle;
	Quaternion rotationQuaternion;
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
		!Vector3{ randPos(), 9 * random() / 10 - 1, randPos() } * (5 + 3 * random()),
		{ 0, 0, 0 },
		0,
		QuaternionIdentity(),
		QuaternionIdentity(),
		color
	};
}

void Bounce(Ball* ball, Vector3 point, Vector3 normal, float dt) {
	Vector3 old = ball->motion;
	ball->motion = ball->motion / normal;
	Vector3 deltaMotion = old - ball->motion;
	ball->rotationAxis = ball->rotationAxis + MASS * (point ^ deltaMotion);
	float i = 2 * MASS * ball->r * ball->r / 5;
	ball->rotationAngle = ball->rotationAngle + Vector3Length(ball->rotationAxis) * dt / i;
	ball->rotationQuaternion = QuaternionFromAxisAngle(!ball->rotationAxis, ball->rotationAngle);
}

bool StaticCollide(Boxes boxes, Ball* ball, float dt) {
	for (auto box : boxes) {
		Vector3 pos = GlobalToLocalPos(ball->pos, box.ref);
		Vector3 posInBox = {
			Clamp(pos.x, -box.ext.x, box.ext.x),
			Clamp(pos.y, -box.ext.y, box.ext.y),
			Clamp(pos.z, -box.ext.z, box.ext.z)
		};
		Vector3 bounce = pos - posInBox;
		if (~bounce < box.r * box.r - EPSILON) {
			Vector3 normal = !bounce;
			ball->pos = LocalToGlobalPos(posInBox + normal * box.r, box.ref);
			Bounce(ball, ball->pos, normal, dt);
			return true;
		}
	}
	return false;
}

bool DynamicCollide(Vector3 a, Vector3 b, Boxes boxes, Ball* ball, float dt, size_t except = -1) {
	Segment segment = { a, b };
	size_t count = boxes.size();
	for (size_t i = 0; i < count; i++) {
		Vector3 interPt;
		Vector3 interNormal;
		if (i != except && IntersectSegmentBoxRounded(segment, boxes[i], interPt, interNormal)) {
			Vector3 c = (b - interPt) / interNormal;
			Bounce(ball, interPt, interNormal, dt);
			DynamicCollide(interPt, interPt + c, boxes, ball, dt, i);
			return true;
		}
	}
	ball->pos = b;
	return false;
}

bool MoveBall(Ball* ball, Obstacles obstacles, float dt) {
	Vector3 b = ball->pos + ball->motion * dt;
	size_t count = obstacles.size();
	Boxes boxes(count);
	for (size_t i = 0; i < count; i++)
		boxes[i] = obstacles[i].withRadius(ball->r);
	return StaticCollide(boxes, ball, dt) || DynamicCollide(ball->pos, b, boxes, ball, dt);
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

	// CAMERA
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
	Obstacles obstacles(0);

	for (int x = -5; x <= 5; x += 5)
		for (int z = -5; z <= 5; z += 5)
			obstacles.push_back(NewObstacle({ (float) x, -5, (float) z }));

	Color transparentPink = { 255, 109, 194, 128 };
	obstacles.push_back({ localReferential({ 0, -10.5, 0 }, QuaternionIdentity()), { 10, 0.5, 10 }, 0, transparentPink });
    obstacles.push_back({ localReferential({ 0, 10.5, 0 }, QuaternionIdentity()), { 10, 0.5, 10 }, 0, BLANK });
    obstacles.push_back({ localReferential({ -10.5, 0, 0 }, QuaternionIdentity()), { 0.5, 10, 10 }, 0, transparentPink });
    obstacles.push_back({ localReferential({ 10.5, 0, 0 }, QuaternionIdentity()), { 0.5, 10, 10 }, 0, transparentPink });
    obstacles.push_back({ localReferential({ 0, 0, -10.5 }, QuaternionIdentity()), { 10, 10, 0.5 }, 0, transparentPink });
    obstacles.push_back({ localReferential({ 0, 0, 10.5 }, QuaternionIdentity()), { 10, 10, 0.5 }, 0, transparentPink });

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
			if (deltaTime > 0) {
				ball.motion.y -= GRAVITY * deltaTime;
				ball.rotation = ball.rotation * ball.rotationQuaternion;
				bool collide = MoveBall(&ball, obstacles, deltaTime);
				if (collide)
					PlaySoundMulti(sounds[rand() % 4]);
			}

            ball.draw();
            for (auto obstacle : obstacles)
	            obstacle.draw();
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
