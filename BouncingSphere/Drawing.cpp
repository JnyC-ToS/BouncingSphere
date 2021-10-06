#include "Drawing.h"
#include "Models.h"
#include "rlgl.h"
#include "vector"

void MyDrawQuad(Quaternion q, Vector3 center, Vector2 size, Color color) {
	int numVertex = 6;
	if (rlCheckBufferLimit(numVertex))
		rlglDraw();

	rlPushMatrix();

	// NOTE: Transformation is applied in inverse order (scale -> translate)
	rlTranslatef(center.x, center.y, center.z);

	// ROTATION
	Vector3 vect;
	float angle;
	QuaternionToAxisAngle(q, &vect, &angle);
	rlRotatef(angle * RAD2DEG, vect.x, vect.y, vect.z);
	rlScalef(size.x / 2, 0, size.y / 2);

	rlBegin(RL_TRIANGLES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	rlVertex3f(-1, 0, -1);
	rlVertex3f(-1, 0, 1);
	rlVertex3f(1, 0, -1);

	rlVertex3f(1, 0, -1);
	rlVertex3f(-1, 0, 1);
	rlVertex3f(1, 0, 1);

	rlEnd();
	rlPopMatrix();
}

void MyDrawQuadWires(Quaternion q, Vector3 center, Vector2 size, Color color) {
	int numVertex = 12;
	if (rlCheckBufferLimit(numVertex))
		rlglDraw();

	rlPushMatrix();

	// NOTE: Transformation is applied in inverse order (scale -> translate)
	rlTranslatef(center.x, center.y, center.z);

	// ROTATION
	Vector3 vect;
	float angle;
	QuaternionToAxisAngle(q, &vect, &angle);
	rlRotatef(angle * RAD2DEG, vect.x, vect.y, vect.z);
	rlScalef(size.x / 2, 0, size.y / 2);

	rlBegin(RL_LINES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	// BORDERS
	rlVertex3f(-1, 0, -1);
	rlVertex3f(-1, 0, 1);

	rlVertex3f(-1, 0, 1);
	rlVertex3f(1, 0, 1);

	rlVertex3f(1, 0, 1);
	rlVertex3f(1, 0, -1);

	rlVertex3f(1, 0, -1);
	rlVertex3f(-1, 0, -1);

	// MIDDLE
	rlVertex3f(-1, 0, -1);
	rlVertex3f(1, 0, 1);

	rlVertex3f(-1, 0, 1);
	rlVertex3f(1, 0, -1);

	rlEnd();
	rlPopMatrix();
}

void MyDrawSphere(Quaternion q, Vector3 centerPos, float radius, int nSegmentsTheta, int nSegmentsPhi, Color color) {
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
			Vector3 topRight = vertexBufferTheta[j + 1];
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

void MyDrawSphereWires(Quaternion q, Vector3 centerPos, float radius, int nSegmentsTheta, int nSegmentsPhi, Color color) {
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

void MyDrawCylinder(Quaternion q, Vector3 start, Vector3 end, float radius, int nSegments, bool drawCaps, Color color) {
	if (nSegments < 3)
		return;

	int numVertex = nSegments * (6 + (drawCaps ? 6 : 0));
	if (rlCheckBufferLimit(numVertex))
		rlglDraw();

	rlPushMatrix();

	// NOTE: Transformation is applied in inverse order (scale -> translate)
	rlTranslatef(start.x, start.y, start.z);

	// ROTATION
	Vector3 axis = Vector3Subtract(end, start);
	Quaternion q1 = QuaternionFromVector3ToVector3({ 0, 1, 0 }, Vector3Normalize(axis));
	Vector3 vect;
	float angle;
	QuaternionToAxisAngle(q, &vect, &angle);
	rlRotatef(angle * RAD2DEG, vect.x, vect.y, vect.z);
	QuaternionToAxisAngle(q1, &vect, &angle);
	rlRotatef(angle * RAD2DEG, vect.x, vect.y, vect.z);
	rlScalef(radius, Vector3Length(axis), radius);

	rlBegin(RL_TRIANGLES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	float delta = 2 * PI / nSegments;

	float theta = 0;
	Vector3 bottomCenter = { 0, 0, 0 };
	Vector3 topCenter = { 0, 1, 0 };
	Vector3 tmpBottomLeft = Cylindrical{ 1, theta, 0 }.toCartesian();

	for (int i = 0; i < nSegments; i++) {
		Vector3 bottomLeft = tmpBottomLeft;
		Vector3 topLeft = { bottomLeft.x, 1, bottomLeft.z };
		Vector3 bottomRight = Cylindrical{ 1, theta + delta, 0 }.toCartesian();
		Vector3 topRight = { bottomRight.x, 1, bottomRight.z };

		rlVertex3f(bottomLeft.x, bottomLeft.y, bottomLeft.z);
		rlVertex3f(topRight.x, topRight.y, topRight.z);
		rlVertex3f(topLeft.x, topLeft.y, topLeft.z);

		rlVertex3f(bottomLeft.x, bottomLeft.y, bottomLeft.z);
		rlVertex3f(bottomRight.x, bottomRight.y, bottomRight.z);
		rlVertex3f(topRight.x, topRight.y, topRight.z);

		if (drawCaps) {
			rlVertex3f(bottomCenter.x, bottomCenter.y, bottomCenter.z);
			rlVertex3f(bottomRight.x, bottomRight.y, bottomRight.z);
			rlVertex3f(bottomLeft.x, bottomLeft.y, bottomLeft.z);

			rlVertex3f(topCenter.x, topCenter.y, topCenter.z);
			rlVertex3f(topLeft.x, topLeft.y, topLeft.z);
			rlVertex3f(topRight.x, topRight.y, topRight.z);
		}

		theta += delta;
		tmpBottomLeft = bottomRight;
	}
	rlEnd();
	rlPopMatrix();
}

void MyDrawCylinderWires(Quaternion q, Vector3 start, Vector3 end, float radius, int nSegments, bool drawCaps, Color color) {
	if (nSegments < 3)
		return;

	int numVertex = nSegments * (6 + (drawCaps ? 4 : 0));
	if (rlCheckBufferLimit(numVertex))
		rlglDraw();

	rlPushMatrix();

	// NOTE: Transformation is applied in inverse order (scale -> translate)
	rlTranslatef(start.x, start.y, start.z);

	// ROTATION
	Vector3 axis = Vector3Subtract(end, start);
	Quaternion q1 = QuaternionFromVector3ToVector3({ 0, 1, 0 }, Vector3Normalize(axis));
	Vector3 vect;
	float angle;
	QuaternionToAxisAngle(q, &vect, &angle);
	rlRotatef(angle * RAD2DEG, vect.x, vect.y, vect.z);
	QuaternionToAxisAngle(q1, &vect, &angle);
	rlRotatef(angle * RAD2DEG, vect.x, vect.y, vect.z);
	rlScalef(radius, Vector3Length(axis), radius);

	rlBegin(RL_LINES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	float delta = 2 * PI / nSegments;

	float theta = 0;
	Vector3 bottomCenter = { 0, 0, 0 };
	Vector3 topCenter = { 0, 1, 0 };
	Vector3 tmpBottomLeft = Cylindrical{ 1, theta, 0 }.toCartesian();

	for (int i = 0; i < nSegments; i++) {
		Vector3 bottomLeft = tmpBottomLeft;
		Vector3 topLeft = { bottomLeft.x, 1, bottomLeft.z };
		Vector3 bottomRight = Cylindrical{ 1, theta + delta, 0 }.toCartesian();
		Vector3 topRight = { bottomRight.x, 1, bottomRight.z };

		rlVertex3f(bottomLeft.x, bottomLeft.y, bottomLeft.z);
		rlVertex3f(bottomRight.x, bottomRight.y, bottomRight.z);

		rlVertex3f(topLeft.x, topLeft.y, topLeft.z);
		rlVertex3f(topRight.x, topRight.y, topRight.z);

		rlVertex3f(bottomLeft.x, bottomLeft.y, bottomLeft.z);
		rlVertex3f(topLeft.x, topLeft.y, topLeft.z);

		if (drawCaps) {
			rlVertex3f(bottomCenter.x, bottomCenter.y, bottomCenter.z);
			rlVertex3f(bottomLeft.x, bottomLeft.y, bottomLeft.z);

			rlVertex3f(topCenter.x, topCenter.y, topCenter.z);
			rlVertex3f(topLeft.x, topLeft.y, topLeft.z);
		}

		theta += delta;
		tmpBottomLeft = bottomRight;
	}
	rlEnd();
	rlPopMatrix();
}
