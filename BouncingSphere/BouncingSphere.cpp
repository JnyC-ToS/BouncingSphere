#include "raylib.h"
#include "raymath.h"
#include "Models.h"
#include "Utils.h"
#include <string>
#include <vector>

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif

#define FPS 60
#define GRAVITY 10
#define MASS 2
#define GAME_TITLE_SCREEN 0b01
#define GAME_RUNNING 0b10
#define GAME_PAUSED 0b11

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
	bool ctrlPressed = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
	Spherical sphDelta = {
		-GetMouseWheelMove() * sphSpeed.rho * deltaTime,
		mouseClicked && !ctrlPressed ? mouseVect.x * sphSpeed.theta * deltaTime : 0,
		mouseClicked && !ctrlPressed ? -mouseVect.y * sphSpeed.phi * deltaTime : 0
	};
	Spherical newSphPos = sphPos + sphDelta;
	newSphPos = { Clamp(newSphPos.rho, rhoMin, rhoMax), newSphPos.theta, Clamp(newSphPos.phi, PI / 100, .99f * PI) };
	sphPos = newSphPos;
	Vector3 sphPosVect = sphPos.toCartesian();

	static Vector3 centerPos = { 0, 0, 0 };
	const static Vector3 centerSpeed = { 0.7f, 0.7f, 0.7f };

	if (mouseClicked && ctrlPressed) {
		Spherical ySph = { sphPos.rho, sphPos.theta, modulof(sphPos.phi - PI / 2, 2 * PI) };
        if (ySph.phi > PI) {
        	ySph.phi = 2 * PI - ySph.phi;
        	ySph.theta += PI;
        }
        Vector3 y = ySph.toCartesian();
        Vector3 x = sphPosVect ^ y;
        Referential ref = { centerPos, !x, !y, !sphPosVect };

        Vector3 centerDelta = { mouseVect.x * centerSpeed.x * deltaTime, mouseVect.y * centerSpeed.y * deltaTime, 0 };
        Vector3 newCenterPos = centerPos + LocalToGlobalVect(centerDelta, ref);
        centerPos = newCenterPos;
	}

	camera->position = sphPosVect + centerPos;
	camera->target = centerPos;
}

void SetupGameObjects(Ball &ball, Obstacles &obstaclesOut) {
	ball = NewBall({ 0, 0, 0 }, BLUE);
	Obstacles obstacles(0);

	for (int x = -5; x <= 5; x += 5)
		for (int z = -5; z <= 5; z += 5)
			obstacles.push_back(NewObstacle({ (float) x, -5, (float) z }));

	// Environment walls
	Color transparentPink = { 255, 109, 194, 90 };
	obstacles.push_back({ localReferential({ 0, -10.5, 0 }, QuaternionIdentity()), { 10, 0.5, 10 }, 0, transparentPink });
	obstacles.push_back({ localReferential({ 0, 10.5, 0 }, QuaternionIdentity()), { 10, 0.5, 10 }, 0, BLANK });
	obstacles.push_back({ localReferential({ -10.5, 0, 0 }, QuaternionIdentity()), { 0.5, 10, 10 }, 0, transparentPink });
	obstacles.push_back({ localReferential({ 10.5, 0, 0 }, QuaternionIdentity()), { 0.5, 10, 10 }, 0, transparentPink });
	obstacles.push_back({ localReferential({ 0, 0, -10.5 }, QuaternionIdentity()), { 10, 10, 0.5 }, 0, transparentPink });
	obstacles.push_back({ localReferential({ 0, 0, 10.5 }, QuaternionIdentity()), { 10, 10, 0.5 }, 0, transparentPink });

	obstaclesOut = obstacles;
}

int main(int argc, char* argv[]) {
	// Window initialization
	float screenSizeCoef = .9f;
	const int screenWidth = (int) roundf(1920 * screenSizeCoef);
	const int screenHeight = (int) roundf(1080 * screenSizeCoef);

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "Bouncing Sphere - Jenny CAO & Théo SZANTO");
	MaximizeWindow();
	SetExitKey(-1);
	SetTargetFPS(FPS);

	// Images loading
	Image icon = LoadImage("resources/images/icon.png");
	SetWindowIcon(icon);
	Image githubIcon = LoadImage("resources/images/github.png");
	Texture2D github = LoadTextureFromImage(githubIcon);
	UnloadImage(githubIcon);
	UnloadImage(icon);

	// Sound management
	InitAudioDevice();
	Sound sounds[4] = {
		LoadSound("resources/sounds/1.mp3"),
		LoadSound("resources/sounds/2.mp3"),
		LoadSound("resources/sounds/3.mp3"),
		LoadSound("resources/sounds/4.mp3")
	};
	Sound easterEgg = LoadSound("resources/sounds/easter_egg.mp3");
	SetMasterVolume(0.25);
	bool soundEffects = false;

	// 3D Camera
	Camera camera;
	camera.position = Spherical{ 10, PI / 4, PI / 4 }.toCartesian();
	camera.target = { 0.0f, 0.0f, 0.0f };
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.type = CAMERA_PERSPECTIVE;
	SetCameraMode(camera, CAMERA_CUSTOM); // Set an orbital camera mode

	// Game objects
	Ball ball;
	Obstacles obstacles;
	int gameState = GAME_TITLE_SCREEN;

	// Main game loop
	while (!WindowShouldClose()) { // Detect window close button or ESC key
		float deltaTime = min(GetFrameTime(), 2.0f / FPS); // Limit FPS loss to half of target

		if (IsKeyPressed(KEY_F1)) {
			ToggleFullscreen();
			if (IsWindowFullscreen()) // Bugfix to correctly update fullscreen mode & flags
				ToggleFullscreen();
		}

		// Draw
		BeginDrawing();
		ClearBackground(RAYWHITE);

		if (gameState == GAME_TITLE_SCREEN) {
			// Scene objects
			const char* title = "Bouncing Sphere";
			DrawText(title, GetScreenWidth() / 2 - MeasureText(title, 150) / 2, GetScreenHeight() / 2 - 75, 150, PINK);
			const char* start = "Press ENTER to start";
			DrawText(start, GetScreenWidth() / 2 - MeasureText(start, 45) / 2, GetScreenHeight() - 60, 45, DARKGRAY);
			const char* authors = "Jenny CAO & Théo SZANTO";
			DrawText(authors, 15, 15, 30, DARKGRAY);
			DrawTexture(github, GetScreenWidth() - github.width - 15, 15, WHITE);

			Vector2 mouse = GetMousePosition();
			if (mouse.y < 30 + github.height && mouse.x > GetScreenWidth() - 30 - github.width) { // GitHub logo (window top right)
				SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
				if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
					OpenURL("https://github.com/JnyC-ToS/BouncingSphere");
			} else if (mouse.y < 60 && mouse.x < 30 + MeasureText(authors, 30)) { // Easter Egg (window top left)
				SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					soundEffects = true;
					PlaySoundMulti(easterEgg);
				}
			} else
				SetMouseCursor(MOUSE_CURSOR_ARROW);

			// Game start
			if (IsKeyDown(KEY_ENTER)) {
				gameState = GAME_RUNNING;
				SetupGameObjects(ball, obstacles);
			}
		} else {
			// Update camera
			MyUpdateOrbitalCamera(&camera, deltaTime);

			BeginMode3D(camera);

			// Game physics: only when window is focused and game is playing
            if (deltaTime > 0 && IsWindowFocused() && gameState == GAME_RUNNING) {
            	// Gravity & rotation
            	ball.motion.y -= GRAVITY * deltaTime;
            	ball.rotation = ball.rotation * ball.rotationQuaternion;

            	// Collision
            	bool collide = MoveBall(&ball, obstacles, deltaTime);
            	if (collide && soundEffects)
            		PlaySoundMulti(sounds[rand() % 4]);
            }

			// Object drawing
            ball.draw();
            for (auto obstacle : obstacles)
                obstacle.draw();

            EndMode3D();

			// Back to title
			const char* text = "Press ESCAPE to go back to title screen";
			DrawText(text, GetScreenWidth() - MeasureText(text, 30) - 15, GetScreenHeight() - 45, 30, DARKGRAY);
			if (IsKeyDown(KEY_ESCAPE))
				gameState = GAME_TITLE_SCREEN;

			// Toggle pause
			DrawText("Press SPACE to toggle pause", 15, GetScreenHeight() - 45, 30, DARKGRAY);
			if (IsKeyPressed(KEY_SPACE))
				gameState ^= 0b1;

			// Pause indicator
			if (gameState == GAME_PAUSED || !IsWindowFocused()) {
				const char* pause = "PAUSED";
				DrawText(pause, GetScreenWidth() / 2 - MeasureText(pause, 45) / 2, 15, 45, DARKGRAY);
			}
		}

		EndDrawing();
	}

	// De-Initialization
	for (auto sound : sounds)
		UnloadSound(sound);
	UnloadSound(easterEgg);
	UnloadTexture(github);
	CloseAudioDevice();
	CloseWindow(); // Close window and OpenGL context

	return EXIT_SUCCESS;
}
