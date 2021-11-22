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

	inline Cylindrical operator +(Cylindrical other) {
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

	inline Spherical operator +(Spherical other) {
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

	inline Cartesian operator +(Cartesian other) {
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
			Vector3Add(this->origin, LocalToGlobalVect(translate, *this)),
			this->i,
			this->j,
			this->k
		};
	}

	Referential operator*(Quaternion rotate) {
		return {
			this->origin,
			Vector3RotateByQuaternion(this->i, rotate),
			Vector3RotateByQuaternion(this->j, rotate),
			Vector3RotateByQuaternion(this->k, rotate)
		};
	}

	Matrix asMatrix() {
		return {
			this->i.x, this->i.y, this->i.z, 0,
			this->j.x, this->j.y, this->j.z, 0,
			this->k.x, this->k.y, this->k.z, 0,
			0, 0, 0, 0
		};
	}
};

Referential localReferential(Vector3 origin, Quaternion q);

// GEOMETRICAL OBJECTS REPRESENTATIONS

struct Segment {
	Vector3 pt1;
	Vector3 pt2;

	Vector3 asVector() {
		return Vector3Subtract(this->pt2, this->pt1);
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
		if (!approxZero(Vector3DotProduct(Vector3Subtract(center, Vector3Scale(this->n, this->d)), this->n)))
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

	Plane asPlane() {
		return { this->ref.j, Vector3DotProduct(this->ref.origin, this->ref.j) };
	}

	void draw(Color color) {
		if (this->ext.x < -EPSILON || this->ext.y < -EPSILON)
			return;
		Quaternion q = QuaternionFromMatrix(this->ref.asMatrix());
		MyDrawQuad(q, this->ref.origin, this->ext, color);
		MyDrawQuadWires(q, this->ref.origin, this->ext, DARKGRAY);
	}
};

struct Disk {
	Referential ref;
	float r;

	Plane asPlane() {
		return { this->ref.j, Vector3DotProduct(this->ref.origin, this->ref.j) };
	}

	void draw(Color color) {
		if (this->r < 0)
			return;
		Quaternion q = QuaternionFromMatrix(this->ref.asMatrix());
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

	Vector3 axis() {
		return Vector3Subtract(this->pt2, this->pt1);
	}

	Vector3 axisNormalized() {
		return Vector3Normalize(this->axis());
	}

	Disk bottom() {
		return { localReferential(this->pt1, QuaternionFromVector3ToVector3({ 0, 1, 0 }, this->axisNormalized())), this->r };
	}

	Disk top() {
		return { localReferential(this->pt2, QuaternionFromVector3ToVector3({ 0, 1, 0 }, Vector3Negate(this->axisNormalized()))), this->r };
	}

	Sphere bottomSphere() {
		return { this->pt1, this->r };
	}

	Sphere topSphere() {
		return { this->pt2, this->r };
	}

	void draw(Quaternion q, Color color, int capsType = CYLINDER_CAPS_FLAT) {
		if (this->r < 0)
			return;
		MyDrawCylinder(q, this->pt1, this->pt2, this->r, 40, capsType, color);
		MyDrawCylinderWires(q, this->pt1, this->pt2, this->r, 40, capsType, DARKGRAY);
	}

	Quaternion quaternionFromAxisAngle(float angle) {
		return QuaternionFromAxisAngle(Vector3Normalize(Vector3Subtract(this->pt2, this->pt1)), angle);
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
		Vector3 bottomFrontLeft = Vector3Add(Vector3Add(Vector3Add(this->ref.origin, Vector3Scale(this->ref.i, -this->ext.x)), Vector3Scale(this->ref.j, -this->ext.y)), Vector3Scale(this->ref.k, this->ext.z));
		Vector3 bottomFrontRight = Vector3Add(Vector3Add(Vector3Add(this->ref.origin, Vector3Scale(this->ref.i, this->ext.x)), Vector3Scale(this->ref.j, -this->ext.y)), Vector3Scale(this->ref.k, this->ext.z));
		Vector3 bottomBackLeft = Vector3Add(Vector3Add(Vector3Add(this->ref.origin, Vector3Scale(this->ref.i, -this->ext.x)), Vector3Scale(this->ref.j, -this->ext.y)), Vector3Scale(this->ref.k, -this->ext.z));
		Vector3 bottomBackRight = Vector3Add(Vector3Add(Vector3Add(this->ref.origin, Vector3Scale(this->ref.i, this->ext.x)), Vector3Scale(this->ref.j, -this->ext.y)), Vector3Scale(this->ref.k, -this->ext.z));
		Vector3 topFrontLeft = Vector3Add(Vector3Add(Vector3Add(this->ref.origin, Vector3Scale(this->ref.i, -this->ext.x)), Vector3Scale(this->ref.j, this->ext.y)), Vector3Scale(this->ref.k, this->ext.z));
		Vector3 topFrontRight = Vector3Add(Vector3Add(Vector3Add(this->ref.origin, Vector3Scale(this->ref.i, this->ext.x)), Vector3Scale(this->ref.j, this->ext.y)), Vector3Scale(this->ref.k, this->ext.z));
		Vector3 topBackLeft = Vector3Add(Vector3Add(Vector3Add(this->ref.origin, Vector3Scale(this->ref.i, -this->ext.x)), Vector3Scale(this->ref.j, this->ext.y)), Vector3Scale(this->ref.k, -this->ext.z));
		Vector3 topBackRight = Vector3Add(Vector3Add(Vector3Add(this->ref.origin, Vector3Scale(this->ref.i, this->ext.x)), Vector3Scale(this->ref.j, this->ext.y)), Vector3Scale(this->ref.k, -this->ext.z));

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
		std::vector<Cylinder> cylinders = this->listCylinders();
		for (auto cylinder : cylinders)
			cylinder.draw(QuaternionIdentity(), color, CYLINDER_CAPS_ROUNDED);
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
