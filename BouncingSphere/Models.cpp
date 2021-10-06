#include "Models.h"
#include "Utils.h"
#include "raylib.h"
#include "raymath.h"

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
	float a = Vector3DotProduct(ab, ab);
	float b = 2 * Vector3DotProduct(ab, ca);
	float c = Vector3DotProduct(ca, ca) - sphere.r * sphere.r;
	float delta = b * b - 4 * a * c;
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

bool IntersectSegmentCylinderInfinite(Segment segment, Cylinder cylinder, Vector3& interPt, Vector3& interNormal) {
	Vector3 ab = Vector3Subtract(segment.pt2, segment.pt1);
	Vector3 pq = Vector3Subtract(cylinder.pt2, cylinder.pt1);
	Vector3 pa = Vector3Subtract(segment.pt1, cylinder.pt1);
	float pq2 = Vector3DotProduct(pq,pq);
	Vector3 i = Vector3Subtract(ab, Vector3Scale(pq, Vector3DotProduct(ab, pq) / pq2));
	Vector3 j = Vector3Subtract(pa, Vector3Scale(pq, Vector3DotProduct(pa, pq) / pq2));

	float a = Vector3DotProduct(i, i);
	float b = 2 * Vector3DotProduct(i, j);
	float c = Vector3DotProduct(j, j) - cylinder.r * cylinder.r;

	float delta = b * b - 4 * a * c;
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
	Vector3 pm = Vector3Subtract(interPt, cylinder.pt1);
	Vector3 u = Vector3Normalize(pq);
	float pm_u = Vector3DotProduct(pm, u);
	Vector3 ph = Vector3Scale(u, pm_u);
	interNormal = Vector3Normalize(Vector3Subtract(interPt, Vector3Add(cylinder.pt1, ph)));
	return true;
}

// TODO Traiter les zones d'intersection avec les disques du cylindre (plans)
bool IntersectSegmentCylinderFinite(Segment segment, Cylinder cylinder, Vector3& interPt, Vector3& interNormal) {
	Vector3 ab = Vector3Subtract(segment.pt2, segment.pt1);
	Vector3 pq = Vector3Subtract(cylinder.pt2, cylinder.pt1);
	Vector3 pa = Vector3Subtract(segment.pt1, cylinder.pt1);
	float pq2 = Vector3DotProduct(pq,pq);
	Vector3 i = Vector3Subtract(ab, Vector3Scale(pq, Vector3DotProduct(ab, pq) / pq2));
	Vector3 j = Vector3Subtract(pa, Vector3Scale(pq, Vector3DotProduct(pa, pq) / pq2));

	float a = Vector3DotProduct(i, i);
	float b = 2 * Vector3DotProduct(i, j);
	float c = Vector3DotProduct(j, j) - cylinder.r * cylinder.r;

	float delta = b * b - 4 * a * c;
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
	Vector3 pm = Vector3Subtract(interPt, cylinder.pt1);
	float pm_pq = Vector3DotProduct(pm, pq);
	if (pm_pq < 0 || pm_pq > pq2)
		return false;

	Vector3 u = Vector3Normalize(pq);
	float pm_u = Vector3DotProduct(pm, u);
	Vector3 ph = Vector3Scale(u, pm_u);
	interNormal = Vector3Normalize(Vector3Subtract(interPt, Vector3Add(cylinder.pt1, ph)));
	return true;
}
