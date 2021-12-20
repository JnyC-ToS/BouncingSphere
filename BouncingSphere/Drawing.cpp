#include "Drawing.h"
#include "Models.h"
#include "rlgl.h"
#include "vector"

void prepareTransformationMatrix(Vector3 scale, Quaternion rotate, Vector3 translate) {
	rlTranslatef(translate.x, translate.y, translate.z);
	Vector3 vect;
	float angle;
	QuaternionToAxisAngle(rotate, &vect, &angle);
	rlRotatef(angle * RAD2DEG, vect.x, vect.y, vect.z);
	rlScalef(scale.x, scale.y, scale.z);
}

inline void vertex(Vector3 v) {
	rlVertex3f(v.x, v.y, v.z);
}

inline void vertex(float x, float y, float z) {
	rlVertex3f(x, y, z);
}

void MyDrawQuad(Quaternion q, Vector3 center, Vector2 size, Color color) {
	int numVertex = 6;
	if (rlCheckBufferLimit(numVertex))
		rlglDraw();

	rlPushMatrix();
	prepareTransformationMatrix({ size.x, 0, size.y }, q, center);

	rlBegin(RL_TRIANGLES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	vertex(-1, 0, -1);
	vertex(-1, 0, 1);
	vertex(1, 0, -1);

	vertex(1, 0, -1);
	vertex(-1, 0, 1);
	vertex(1, 0, 1);

	rlEnd();
	rlPopMatrix();
}

void MyDrawQuadWires(Quaternion q, Vector3 center, Vector2 size, Color color) {
	int numVertex = 12;
	if (rlCheckBufferLimit(numVertex))
		rlglDraw();

	rlPushMatrix();
	prepareTransformationMatrix({ size.x, 0, size.y }, q, center);

	rlBegin(RL_LINES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	// BORDERS
	vertex(-1, 0, -1);
	vertex(-1, 0, 1);

	vertex(-1, 0, 1);
	vertex(1, 0, 1);

	vertex(1, 0, 1);
	vertex(1, 0, -1);

	vertex(1, 0, -1);
	vertex(-1, 0, -1);

	// MIDDLE
	vertex(-1, 0, -1);
	vertex(1, 0, 1);

	vertex(-1, 0, 1);
	vertex(1, 0, -1);

	rlEnd();
	rlPopMatrix();
}

void MyDrawSphere(Quaternion q, Vector3 center, float radius, int nSegmentsTheta, int nSegmentsPhi, Color color) {
	MyDrawSpherePortion(q, center, radius, 0, 2 * PI, nSegmentsTheta, 0, PI, nSegmentsPhi, color);
}

void MyDrawSpherePortion(Quaternion q, Vector3 center, float radius, float startTheta, float endTheta, int nSegmentsTheta, float startPhi, float endPhi, int nSegmentsPhi, Color color) {
	if (nSegmentsTheta < 3 || nSegmentsPhi < 2)
		return;

	float deltaPhi = (endPhi - startPhi) / nSegmentsPhi;
	float deltaTheta = (endTheta - startTheta) / nSegmentsTheta;

	std::vector<Vector3> vertexBufferTheta(nSegmentsTheta + 1);
	for (int n = 0; n <= nSegmentsTheta; n++)
		vertexBufferTheta[n] = Spherical{ 1, startTheta + n * deltaTheta, startPhi }.toCartesian();

	int numVertex = nSegmentsPhi * nSegmentsTheta * 6;
	if (rlCheckBufferLimit(numVertex))
		rlglDraw();

	rlPushMatrix();
	prepareTransformationMatrix({ radius, radius, radius }, q, center);

	rlBegin(RL_TRIANGLES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	float phi = startPhi;
	for (int i = 0; i < nSegmentsPhi; i++) {
		float nextPhi = phi + deltaPhi;
		float theta = startTheta;
		Vector3 tmpBottomLeft = Spherical{ 1, theta, nextPhi }.toCartesian();

		for (int j = 0; j < nSegmentsTheta; j++) {
			float nextTheta = theta + deltaTheta;
			Vector3 topLeft = vertexBufferTheta[j];
			Vector3 bottomLeft = tmpBottomLeft;
			Vector3 topRight = vertexBufferTheta[j + 1];
			Vector3 bottomRight = Spherical{ 1, nextTheta, nextPhi }.toCartesian();

			vertex(bottomLeft);
			vertex(topLeft);
			vertex(topRight);

			vertex(bottomLeft);
			vertex(topRight);
			vertex(bottomRight);

			theta = nextTheta;
			vertexBufferTheta[j] = tmpBottomLeft;
			tmpBottomLeft = bottomRight;
		}
		vertexBufferTheta[nSegmentsTheta] = tmpBottomLeft;
		phi = nextPhi;
	}
	rlEnd();
	rlPopMatrix();
}

void MyDrawSphereWires(Quaternion q, Vector3 center, float radius, int nSegmentsTheta, int nSegmentsPhi, Color color) {
	MyDrawSphereWiresPortion(q, center, radius, 0, 2 * PI, nSegmentsTheta, 0, PI, nSegmentsPhi, color);
}

void MyDrawSphereWiresPortion(Quaternion q, Vector3 center, float radius, float startTheta, float endTheta, int nSegmentsTheta, float startPhi, float endPhi, int nSegmentsPhi, Color color) {
	if (nSegmentsTheta < 3 || nSegmentsPhi < 2)
		return;

	float deltaPhi = (endPhi - startPhi) / nSegmentsPhi;
	float deltaTheta = (endTheta - startTheta) / nSegmentsTheta;

	std::vector<Vector3> vertexBufferTheta(nSegmentsTheta + 1);
	for (int n = 0; n <= nSegmentsTheta; n++)
		vertexBufferTheta[n] = Spherical{ 1, startTheta + n * deltaTheta, startPhi }.toCartesian();

	int numVertex = nSegmentsPhi * (nSegmentsTheta * 4 + 2) + nSegmentsTheta * 2;
	if (rlCheckBufferLimit(numVertex))
		rlglDraw();

	rlPushMatrix();
	prepareTransformationMatrix({ radius, radius, radius }, q, center);

	rlBegin(RL_LINES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	float phi = startPhi;
	for (int i = 0; i < nSegmentsPhi; i++) {
		float nextPhi = phi + deltaPhi;
		float theta = startTheta;

		for (int j = 0; j < nSegmentsTheta; j++) {
			Vector3 topLeft = vertexBufferTheta[j];
			Vector3 bottomLeft = Spherical{ 1, theta, nextPhi }.toCartesian();
			Vector3 topRight = vertexBufferTheta[j + 1];

			vertex(topLeft);
			vertex(topRight);

			vertex(topLeft);
			vertex(bottomLeft);

			theta += deltaTheta;
			vertexBufferTheta[j] = bottomLeft;
		}

		Vector3 topRight = vertexBufferTheta[nSegmentsTheta];
		Vector3 bottomRight = Spherical{ 1, endTheta, nextPhi }.toCartesian();

		vertex(topRight);
		vertex(bottomRight);

		vertexBufferTheta[nSegmentsTheta] = bottomRight;
		phi = nextPhi;
	}

	for (int n = 0; n < nSegmentsTheta; n++) {
		Vector3 bottomLeft = vertexBufferTheta[n];
		Vector3 bottomRight = vertexBufferTheta[n + 1];

		vertex(bottomLeft);
		vertex(bottomRight);
	}

	rlEnd();
	rlPopMatrix();
}

void MyDrawCylinder(Quaternion q, Vector3 start, Vector3 end, float radius, int nSegments, int capsType, Color color) {
	MyDrawCylinderPortion(q, start, end, radius, 0, 2 * PI, nSegments, capsType, color);
}

void MyDrawCylinderPortion(Quaternion q, Vector3 start, Vector3 end, float radius, float startSegments, float endSegments, int nSegments, int capsType, Color color) {
	if (nSegments < 3)
		return;

	int numVertex = nSegments * 6;
	if (rlCheckBufferLimit(numVertex))
		rlglDraw();

	rlPushMatrix();
	Vector3 axis = Vector3Subtract(end, start);
	Quaternion q1 = QuaternionFromVector3ToVector3({ 0, 1, 0 }, Vector3Normalize(axis));
	Quaternion qf = QuaternionMultiply(q, q1);
	prepareTransformationMatrix({ radius, Vector3Length(axis), radius }, qf, start);

	rlBegin(RL_TRIANGLES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	float delta = (endSegments - startSegments) / nSegments;

	float theta = startSegments;
	Vector3 tmpBottomLeft = Cylindrical{ 1, theta, 0 }.toCartesian();

	for (int i = 0; i < nSegments; i++) {
		float nextTheta = theta + delta;
		Vector3 bottomLeft = tmpBottomLeft;
		Vector3 topLeft = { bottomLeft.x, 1, bottomLeft.z };
		Vector3 bottomRight = Cylindrical{ 1, nextTheta, 0 }.toCartesian();
		Vector3 topRight = { bottomRight.x, 1, bottomRight.z };

		vertex(bottomLeft);
		vertex(topRight);
		vertex(topLeft);

		vertex(bottomLeft);
		vertex(bottomRight);
		vertex(topRight);

		theta = nextTheta;
		tmpBottomLeft = bottomRight;
	}

	if (capsType == CYLINDER_CAPS_FLAT) {
		Quaternion aroundX = QuaternionFromAxisAngle({ 1, 0, 0 }, PI);
		Quaternion aroundY = QuaternionFromAxisAngle({ 0, 1, 0 }, startSegments);
		MyDrawDiskPortion(QuaternionMultiply(aroundY, aroundX), { 0, 0, 0 }, 1, startSegments, endSegments, nSegments, color);
		MyDrawDiskPortion(QuaternionIdentity(), { 0, 1, 0 }, 1, startSegments, endSegments, nSegments, color);
	}

	rlEnd();
	rlPopMatrix();

	if (capsType == CYLINDER_CAPS_ROUNDED) {
		MyDrawSpherePortion(qf, start, radius, startSegments, endSegments, nSegments, PI / 2, PI, nSegments / 4, color);
		MyDrawSpherePortion(qf, end, radius, startSegments, endSegments, nSegments, 0, PI / 2, nSegments / 4, color);
	}
}

void MyDrawCylinderWires(Quaternion q, Vector3 start, Vector3 end, float radius, int nSegments, int capsType, Color color) {
	MyDrawCylinderWiresPortion(q, start, end, radius, 0, 2 * PI, nSegments, capsType, color);
}

void MyDrawCylinderWiresPortion(Quaternion q, Vector3 start, Vector3 end, float radius, float startSegments, float endSegments, int nSegments, int capsType, Color color) {
	if (nSegments < 3)
		return;

	int numVertex = nSegments * (2 + (capsType == CYLINDER_CAPS_FLAT ? 0 : 4)) + 2;
	if (rlCheckBufferLimit(numVertex))
		rlglDraw();

	rlPushMatrix();
	Vector3 axis = Vector3Subtract(end, start);
	Quaternion q1 = QuaternionFromVector3ToVector3({ 0, 1, 0 }, Vector3Normalize(axis));
	Quaternion qf = QuaternionMultiply(q, q1);
	prepareTransformationMatrix({ radius, Vector3Length(axis), radius }, qf, start);

	rlBegin(RL_LINES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	float delta = (endSegments - startSegments) / nSegments;

	float theta = startSegments;
	Vector3 tmpBottomLeft = Cylindrical{ 1, theta, 0 }.toCartesian();

	for (int i = 0; i < nSegments; i++) {
		float nextTheta = theta + delta;
		Vector3 bottomLeft = tmpBottomLeft;
		Vector3 topLeft = { bottomLeft.x, 1, bottomLeft.z };
		Vector3 bottomRight = Cylindrical{ 1, nextTheta, 0 }.toCartesian();
		Vector3 topRight = { bottomRight.x, 1, bottomRight.z };

		vertex(bottomLeft);
		vertex(topLeft);

		if (capsType == CYLINDER_CAPS_NONE) {
			vertex(bottomLeft);
			vertex(bottomRight);

			vertex(topLeft);
			vertex(topRight);
		}

		theta = nextTheta;
		tmpBottomLeft = bottomRight;
	}

	Vector3 bottomRight = tmpBottomLeft;
	Vector3 topRight = { bottomRight.x, 1, bottomRight.z };

	vertex(bottomRight);
	vertex(topRight);

	if (capsType == CYLINDER_CAPS_FLAT) {
		Quaternion aroundX = QuaternionFromAxisAngle({ 1, 0, 0 }, PI);
		Quaternion aroundY = QuaternionFromAxisAngle({ 0, 1, 0 }, startSegments);
		MyDrawDiskWiresPortion(QuaternionMultiply(aroundY, aroundX), { 0, 0, 0 }, 1, startSegments, endSegments, nSegments, color);
		MyDrawDiskWiresPortion(QuaternionIdentity(), { 0, 1, 0 }, 1, startSegments, endSegments, nSegments, color);
	}

	rlEnd();
	rlPopMatrix();

	if (capsType == CYLINDER_CAPS_ROUNDED) {
		MyDrawSphereWiresPortion(qf, start, radius, startSegments, endSegments, nSegments, PI / 2, PI, nSegments / 4, color);
		MyDrawSphereWiresPortion(qf, end, radius, startSegments, endSegments, nSegments, 0, PI / 2, nSegments / 4, color);
	}
}

void MyDrawDisk(Quaternion q, Vector3 center, float radius, int nSegments, Color color) {
	MyDrawDiskPortion(q, center, radius, 0, 2 * PI, nSegments, color);
}

void MyDrawDiskPortion(Quaternion q, Vector3 center, float radius, float startSegments, float endSegments, int nSegments, Color color) {
	if (nSegments < 3)
		return;

	int numVertex = nSegments * 3;
	if (rlCheckBufferLimit(numVertex))
		rlglDraw();

	rlPushMatrix();
	prepareTransformationMatrix({ radius, 0, radius }, q, center);

	rlBegin(RL_TRIANGLES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	float delta = (endSegments - startSegments) / nSegments;

	float theta = startSegments;
	Vector3 tmpLeft = Cylindrical{ 1, theta, 0 }.toCartesian();

	for (int i = 0; i < nSegments; i++) {
		float nextTheta = theta + delta;
		Vector3 left = tmpLeft;
		Vector3 right = Cylindrical{ 1, nextTheta, 0 }.toCartesian();

		vertex(0, 0, 0);
		vertex(left);
		vertex(right);

		theta = nextTheta;
		tmpLeft = right;
	}
	rlEnd();
	rlPopMatrix();
}

void MyDrawDiskWires(Quaternion q, Vector3 center, float radius, int nSegments, Color color) {
	MyDrawDiskWiresPortion(q, center, radius, 0, 2 * PI, nSegments, color);
}

void MyDrawDiskWiresPortion(Quaternion q, Vector3 center, float radius, float startSegments, float endSegments, int nSegments, Color color) {
	if (nSegments < 3)
		return;

	int numVertex = nSegments * 4 + 2;
	if (rlCheckBufferLimit(numVertex))
		rlglDraw();

	rlPushMatrix();
	prepareTransformationMatrix({ radius, 0, radius }, q, center);

	rlBegin(RL_LINES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	float delta = (endSegments - startSegments) / nSegments;

	float theta = startSegments;
	Vector3 tmpLeft = Cylindrical{ 1, theta, 0 }.toCartesian();

	for (int i = 0; i < nSegments; i++) {
		float nextTheta = theta + delta;
		Vector3 left = tmpLeft;
		Vector3 right = Cylindrical{ 1, nextTheta, 0 }.toCartesian();

		vertex(left);
		vertex(right);

		vertex(0, 0, 0);
		vertex(left);

		theta = nextTheta;
		tmpLeft = right;
	}

	Vector3 right = tmpLeft;

	vertex(0, 0, 0);
	vertex(right);

	rlEnd();
	rlPopMatrix();
}
