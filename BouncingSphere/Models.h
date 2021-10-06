#pragma once
#include "Drawing.h"
#include "Utils.h"
#include "raymath.h"

// COORDINATES SYSTEMS REPRESENTATIONS

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

// GEOMETRICAL OBJECTS REPRESENTATIONS

struct Segment {
	Vector3 pt1;
	Vector3 pt2;

	void draw() {
		DrawLine3D(this->pt1, this->pt2, BLACK);
		MyDrawSphere({ 0 }, this->pt1, .05f, 10, 10, DARKGRAY);
		MyDrawSphere({ 0 }, this->pt2, .03f, 10, 10, BLACK);
	}
};

struct Plane {
	Vector3 n;
	float d;

	void draw(Vector3 center, Color color) {
		Quaternion q = QuaternionFromVector3ToVector3({ 0, 1, 0 }, this->n);
		Vector2 size = { 2, 2 };
		MyDrawQuad(q, center, size, color);
		MyDrawQuadWires(q, center, size, DARKGRAY);
	}
};

struct Sphere {
	Vector3 center;
	float r;

	void draw(Quaternion q, Color color) {
		MyDrawSphere(q, this->center, this->r, 20, 20, color);
		MyDrawSphereWires(q, this->center, this->r, 20, 20, DARKGRAY);
	}
};

struct Cylinder {
	Vector3 pt1;
	Vector3 pt2;
	float r;

	void draw(Quaternion q, Color color) {
		MyDrawCylinder(q, this->pt1, this->pt2, this->r, 40, true, color);
		MyDrawCylinderWires(q, this->pt1, this->pt2, this->r, 40, true, DARKGRAY);
	}

	Quaternion quaternionFromAxisAngle(float angle) {
		return QuaternionFromAxisAngle(Vector3Normalize(Vector3Subtract(this->pt2, this->pt1)), angle);
	}
};

bool IntersectSegmentPlane(Segment segment, Plane plane, Vector3& interPt, Vector3& interNormal);
bool IntersectSegmentSphere(Segment segment, Sphere sphere, Vector3& interPt, Vector3& interNormal);
bool IntersectSegmentCylinderInfinite(Segment segment, Cylinder cylinder, Vector3& interPt, Vector3& interNormal);
bool IntersectSegmentCylinderFinite(Segment segment, Cylinder cylinder, Vector3& interPt, Vector3& interNormal);
