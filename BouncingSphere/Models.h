#ifndef __MODEL_H__
#define __MODEL_H__

#include "Drawing.h"
#include "Utils.h"
#include "raymath.h"
#include <vector>

// COORDINATES SYSTEMS REPRESENTATIONS

struct Cylindrical {
	float rho;
	float theta;
	float y;

	inline Cylindrical operator+(Cylindrical other) {
		return { this->rho + other.rho, this->theta + other.theta, this->y + other.y };
	}

	Vector3 toCartesian() {
		return { this->rho * sinf(this->theta), this->y, this->rho * cosf(this->theta) };
	}
};

struct Spherical {
	float rho;
	float theta;
	float phi;

	inline Spherical operator+(Spherical other) {
		return { this->rho + other.rho, this->theta + other.theta, this->phi + other.phi };
	}

	Vector3 toCartesian() {
		return {
			this->rho * sinf(this->phi) * cosf(this->theta),
			this->rho * cosf(this->phi),
			this->rho * sinf(this->phi) * sinf(this->theta)
		};
	}
};

struct Cartesian {
	float x;
	float y;
	float z;

	inline Cartesian operator+(Cartesian other) {
		return { this->x + other.x, this->y + other.y, this->z + other.z };
	}

	Cylindrical toCylindrical() {
		Cylindrical cyl;
		cyl.rho = sqrtf(this->x * this->x + this->z * this->z);
		cyl.y = this->y;

		if (cyl.rho < EPSILON)
			cyl.theta = 0;
		else {
			cyl.theta = atan2f(this->x, this->z);
			if (cyl.theta < 0)
				cyl.theta += PI * 2;
		}
		return cyl;
	}

	static Cartesian fromVector3(Vector3 vector) {
		return { vector.x, vector.y, vector.z };
	}
};

// Coordinate convertions
typedef struct ref Referential;

Vector3 GlobalToLocalPos(Vector3 posGlobal, Referential localRef);
Vector3 GlobalToLocalVect(Vector3 vectGlobal, Referential localRef);
Vector3 LocalToGlobalPos(Vector3 posLocal, Referential localRef);
Vector3 LocalToGlobalVect(Vector3 vectLocal, Referential localRef);

struct ref {
	Vector3 origin;
	Vector3 i;
	Vector3 j;
	Vector3 k;

	Referential operator+(Vector3 translate) {
		return {
			this->origin + LocalToGlobalVect(translate, *this),
			this->i,
			this->j,
			this->k
		};
	}

	Referential operator*(Quaternion rotate) {
		return {
			this->origin,
			this->i * rotate,
			this->j * rotate,
			this->k * rotate
		};
	}

	inline Matrix asMatrix() {
		return {
			this->i.x, this->i.y, this->i.z, 0,
			this->j.x, this->j.y, this->j.z, 0,
			this->k.x, this->k.y, this->k.z, 0,
			0, 0, 0, 0
		};
	}

	Quaternion asQuaternion() {
		Matrix m = this->asMatrix();
		if (m.m0 == 1 && m.m5 == 1 && m.m10 == 1 && m.m4 == 0 && m.m8 == 0 && m.m9 == 0 && m.m1 == 0 && m.m2 == 0 && m.m6 == 0)
			return QuaternionIdentity();
		return QuaternionFromMatrix(m);
	}
};

Referential localReferential(Vector3 origin, Quaternion q);

// GEOMETRICAL OBJECTS REPRESENTATIONS

struct Segment {
	Vector3 pt1;
	Vector3 pt2;

	inline Vector3 asVector() {
		return this->pt2 - this->pt1;
	}

	void draw() {
		DrawLine3D(this->pt1, this->pt2, BLACK);
		MyDrawSphere(QuaternionIdentity(), this->pt1, .05f, 10, 10, DARKGRAY);
		MyDrawSphere(QuaternionIdentity(), this->pt2, .03f, 10, 10, BLACK);
	}
};

struct Plane {
	Vector3 n;
	float d;

	void draw(Vector3 center, Color color) {
		if (!approxZero((center - this->n * this->d) * this->n))
			return;
		Quaternion q = QuaternionFromVector3ToVector3({ 0, 1, 0 }, this->n);
		Vector2 size = { 2, 2 };
		MyDrawQuad(q, center, size, color);
		MyDrawQuadWires(q, center, size, DARKGRAY);
	}
};

struct Quad {
	Referential ref;
	Vector2 ext;

	inline Plane asPlane() {
		return { this->ref.j, this->ref.origin * this->ref.j };
	}

	void draw(Color color) {
		if (this->ext.x < -EPSILON || this->ext.y < -EPSILON)
			return;
		Quaternion q = this->ref.asQuaternion();
		MyDrawQuad(q, this->ref.origin, this->ext, color);
		MyDrawQuadWires(q, this->ref.origin, this->ext, DARKGRAY);
	}
};

struct Disk {
	Referential ref;
	float r;

	inline Plane asPlane() {
		return { this->ref.j, this->ref.origin * this->ref.j };
	}

	void draw(Color color) {
		if (this->r < 0)
			return;
		Quaternion q = this->ref.asQuaternion();
		MyDrawDisk(q, this->ref.origin, this->r, 20, color);
		MyDrawDiskWires(q, this->ref.origin, this->r, 20, DARKGRAY);
	}
};

struct Sphere {
	Vector3 center;
	float r;

	void draw(Quaternion q, Color color) {
		if (this->r < 0)
			return;
		MyDrawSphere(q, this->center, this->r, 20, 20, color);
		MyDrawSphereWires(q, this->center, this->r, 20, 20, DARKGRAY);
	}
};

struct Cylinder {
	Vector3 pt1;
	Vector3 pt2;
	float r;

	inline Vector3 axis() {
		return this->pt2 - this->pt1;
	}

	inline Vector3 axisNormalized() {
		return !this->axis();
	}

	inline Disk bottom() {
		return { localReferential(this->pt1, QuaternionFromVector3ToVector3({ 0, 1, 0 }, this->axisNormalized())), this->r };
	}

	inline Disk top() {
		return { localReferential(this->pt2, QuaternionFromVector3ToVector3({ 0, 1, 0 }, -this->axisNormalized())), this->r };
	}

	inline Sphere bottomSphere() {
		return { this->pt1, this->r };
	}

	inline Sphere topSphere() {
		return { this->pt2, this->r };
	}

	void draw(Color color, int capsType = CYLINDER_CAPS_FLAT, float angle = 0) {
		if (this->r < 0)
			return;
		Quaternion q = this->quaternionFromAxisAngle(angle);
		MyDrawCylinder(q, this->pt1, this->pt2, this->r, 20, capsType, color);
		MyDrawCylinderWires(q, this->pt1, this->pt2, this->r, 20, capsType, DARKGRAY);
	}

	Quaternion quaternionFromAxisAngle(float angle) {
		return QuaternionFromAxisAngle(!(this->pt2 - this->pt1), angle);
	}
};

struct BoxRounded {
	Referential ref;
	Vector3 ext;
	float r;

	std::vector<Quad> listQuads() {
		std::vector<Quad> quads(6);
		quads[0] = { this->ref + Vector3{ 0, this->ext.y + this->r, 0 }, { this->ext.x, this->ext.z } }; // Top
		quads[1] = { (this->ref + Vector3{ 0, -(this->ext.y + this->r), 0 }) * QuaternionFromAxisAngle(this->ref.i, PI), { this->ext.x, this->ext.z } }; // Bottom
		quads[2] = { (this->ref + Vector3{ this->ext.x + this->r, 0, 0 }) * QuaternionFromAxisAngle(this->ref.k, -PI / 2), { this->ext.y, this->ext.z } }; // Right
		quads[3] = { (this->ref + Vector3{ -(this->ext.x + this->r), 0, 0 }) * QuaternionFromAxisAngle(this->ref.k, PI / 2), { this->ext.y, this->ext.z } }; // Left
		quads[4] = { (this->ref + Vector3{ 0, 0, this->ext.z + this->r }) * QuaternionFromAxisAngle(this->ref.i, PI / 2), { this->ext.x, this->ext.y } }; // Front
		quads[5] = { (this->ref + Vector3{ 0, 0, -(this->ext.z + this->r) }) * QuaternionFromAxisAngle(this->ref.i, -PI / 2), { this->ext.x, this->ext.y } }; // Back
		return quads;
	}

	std::vector<Cylinder> listCylinders() {
		std::vector<Cylinder> cylinders(12);
		Vector3 bottomFrontLeft = this->ref.origin + this->ref.i * -this->ext.x + this->ref.j * -this->ext.y + this->ref.k * this->ext.z;
		Vector3 bottomFrontRight = this->ref.origin + this->ref.i * this->ext.x + this->ref.j * -this->ext.y + this->ref.k * this->ext.z;
		Vector3 bottomBackLeft = this->ref.origin + this->ref.i * -this->ext.x + this->ref.j * -this->ext.y + this->ref.k * -this->ext.z;
		Vector3 bottomBackRight = this->ref.origin + this->ref.i * this->ext.x + this->ref.j * -this->ext.y + this->ref.k * -this->ext.z;
		Vector3 topFrontLeft = this->ref.origin + this->ref.i * -this->ext.x + this->ref.j * this->ext.y + this->ref.k * this->ext.z;
		Vector3 topFrontRight = this->ref.origin + this->ref.i * this->ext.x + this->ref.j * this->ext.y + this->ref.k * this->ext.z;
		Vector3 topBackLeft = this->ref.origin + this->ref.i * -this->ext.x + this->ref.j * this->ext.y + this->ref.k * -this->ext.z;
		Vector3 topBackRight = this->ref.origin + this->ref.i * this->ext.x + this->ref.j * this->ext.y + this->ref.k * -this->ext.z;

		cylinders[0] = { bottomFrontLeft, bottomFrontRight, this->r };
		cylinders[1] = { bottomFrontRight, bottomBackRight, this->r };
		cylinders[2] = { bottomBackRight, bottomBackLeft, this->r };
		cylinders[3] = { bottomBackLeft, bottomFrontLeft, this->r };

		cylinders[4] = { bottomFrontLeft, topFrontLeft, this->r };
		cylinders[5] = { bottomFrontRight, topFrontRight, this->r };
		cylinders[6] = { bottomBackRight, topBackRight, this->r };
		cylinders[7] = { bottomBackLeft, topBackLeft, this->r };

		cylinders[8] = { topFrontLeft, topFrontRight, this->r };
		cylinders[9] = { topFrontRight, topBackRight, this->r };
		cylinders[10] = { topBackRight, topBackLeft, this->r };
		cylinders[11] = { topBackLeft, topFrontLeft, this->r };
		return cylinders;
	}

	void draw(Color color) {
		std::vector<Quad> quads = this->listQuads();
		for (auto quad : quads)
			quad.draw(color);
		if (this->r > EPSILON) {
			std::vector<Cylinder> cylinders = this->listCylinders();
			for (auto cylinder : cylinders)
				cylinder.draw(color, CYLINDER_CAPS_ROUNDED);
		}
	}
};

// Intersections
bool IntersectSegmentPlane(Segment segment, Plane plane, Vector3& interPt, Vector3& interNormal);
bool IntersectSegmentQuad(Segment segment, Quad quad, Vector3& interPt, Vector3& interNormal);
bool IntersectSegmentDisk(Segment segment, Disk disk, Vector3& interPt, Vector3& interNormal);
bool IntersectSegmentSphere(Segment segment, Sphere sphere, Vector3& interPt, Vector3& interNormal);
bool IntersectSegmentCylinderInfinite(Segment segment, Cylinder cylinder, Vector3& interPt, Vector3& interNormal);
bool IntersectSegmentCylinderFinite(Segment segment, Cylinder cylinder, Vector3& interPt, Vector3& interNormal);
bool IntersectSegmentCylinderRounded(Segment segment, Cylinder cylinder, Vector3& interPt, Vector3& interNormal);
bool IntersectSegmentBoxRounded(Segment segment, BoxRounded box, Vector3& interPt, Vector3& interNormal);

#endif
