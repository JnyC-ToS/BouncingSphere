#include "Models.h"
#include "Utils.h"
#include "raylib.h"

bool IntersectSegmentPlane(Segment segment, Plane plane, Vector3& interPt, Vector3& interNormal) {
	Vector3 ab = segment.asVector();
	float ab_n = ab * plane.n;
	if (approxZero(ab_n))
		return false;
	float t = (plane.d - segment.pt1 * plane.n) / ab_n;
	if (t < -EPSILON || t > 1 + EPSILON)
		return false;
	interPt = segment.pt1 + ab * t;
	if (ab_n < 0)
		interNormal = plane.n;
	else
		interNormal = -plane.n;
	return true;
}

bool IntersectSegmentQuad(Segment segment, Quad quad, Vector3& interPt, Vector3& interNormal) {
	Vector3 interPtPlane;
	Vector3 interNormalPlane;
	if (!IntersectSegmentPlane(segment, quad.asPlane(), interPtPlane, interNormalPlane))
		return false;
	Vector3 interPtLocal = GlobalToLocalPos(interPtPlane, quad.ref);
	if (fabs(interPtLocal.x) > quad.ext.x + EPSILON || fabs(interPtLocal.z) > quad.ext.y + EPSILON)
		return false;
	interPt = interPtPlane;
	interNormal = interNormalPlane;
	return true;
}

bool IntersectSegmentDisk(Segment segment, Disk disk, Vector3& interPt, Vector3& interNormal) {
	Vector3 interPtPlane;
	Vector3 interNormalPlane;
	if (!IntersectSegmentPlane(segment, disk.asPlane(), interPtPlane, interNormalPlane)
			|| ~(interPtPlane - disk.ref.origin) > disk.r * disk.r + EPSILON)
		return false;
	interPt = interPtPlane;
	interNormal = interNormalPlane;
	return true;
}

bool IntersectSegmentSphere(Segment segment, Sphere sphere, Vector3& interPt, Vector3& interNormal) {
	Vector3 ab = segment.asVector();
	Vector3 ca = segment.pt1 - sphere.center;
	float a = ~ab;
	float b = 2 * (ab * ca);
	float c = ~ca - sphere.r * sphere.r;
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
	if (t < -EPSILON || t > 1 + EPSILON)
		return false;
	interPt = segment.pt1 + ab * t;
	interNormal = !(interPt - sphere.center);
	return true;
}

bool IntersectSegmentCylinderInfinite(Segment segment, Cylinder cylinder, Vector3& interPt, Vector3& interNormal) {
	Vector3 ab = segment.asVector();
	Vector3 pq = cylinder.axis();
	Vector3 pa = segment.pt1 - cylinder.pt1;
	float pq2 = ~pq;
	Vector3 i = ab - (pq * (ab * pq / pq2));
	Vector3 j = pa - (pq * (pa * pq / pq2));

	float a = ~i;
	float b = 2 * (i * j);
	float c = ~j - cylinder.r * cylinder.r;

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
	if (t < -EPSILON || t > 1 + EPSILON)
		return false;

	interPt = segment.pt1 + ab * t;
	Vector3 pm = interPt - cylinder.pt1;
	Vector3 u = !pq;
	float pm_u = pm * u;
	Vector3 ph = u * pm_u;
	interNormal = !(interPt - (cylinder.pt1 + ph));
	return true;
}

bool IntersectSegmentCylinderFinite(Segment segment, Cylinder cylinder, Vector3& interPt, Vector3& interNormal) {
	Vector3 ab = segment.asVector();
	Vector3 pq = cylinder.axis();
	Vector3 u = !pq;
	Vector3 pa = segment.pt1 - cylinder.pt1;
	float pq2 = ~pq;
	float r2 = cylinder.r * cylinder.r;

	Vector3 pm;
	if (~(pa ^ u) > r2 + EPSILON) { // If start outside of infinite cylinder
		Vector3 i = ab - (pq * (ab * pq / pq2));
		Vector3 j = pa - (pq * (pa * pq / pq2));

		float a = ~i;
		float b = 2 * (i * j);
		float c = ~j - r2;

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
		if (t < -EPSILON || t > 1 + EPSILON)
			return false;
		interPt = segment.pt1 + ab * t;
		pm = interPt - cylinder.pt1;
	} else { // Start cannot be inside the finite cylinder so only above or below
		interPt = segment.pt1;
		pm = pa;
	}

	float pm_pq = pm * pq;
	if (pm_pq < EPSILON) { // Below cylinder
		return IntersectSegmentDisk(segment, cylinder.bottom(), interPt, interNormal);
	} else if (pm_pq > pq2 - EPSILON) { // Above cylinder
		return IntersectSegmentDisk(segment, cylinder.top(), interPt, interNormal);
	} else { // In cylinder, only possible when start is outside
		float pm_u = pm * u;
		Vector3 ph = u * pm_u;
		interNormal = !(interPt - (cylinder.pt1 + ph));
		return true;
	}
}

bool IntersectSegmentCylinderRounded(Segment segment, Cylinder cylinder, Vector3& interPt, Vector3& interNormal) {
	Vector3 ab = segment.asVector();
	Vector3 pq = cylinder.axis();
	Vector3 u = !pq;
	Vector3 pa = segment.pt1 - cylinder.pt1;
	float pq2 = ~pq;
	float r2 = cylinder.r * cylinder.r;

	Vector3 pm;
	if (~(pa ^ u) > r2 + EPSILON) { // If start outside of infinite cylinder
		Vector3 i = ab - (pq * (ab * pq / pq2));
		Vector3 j = pa - (pq * (pa * pq / pq2));

		float a = ~i;
		float b = 2 * (i * j);
		float c = ~j - r2;

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
		if (t < -EPSILON || t > 1 + EPSILON)
			return false;
		interPt = segment.pt1 + ab * t;
		pm = interPt - cylinder.pt1;
	} else { // Start cannot be inside the finite cylinder so only above or below
		interPt = segment.pt1;
		pm = pa;
	}

	float pm_pq = pm * pq;
	if (pm_pq < EPSILON) { // Below cylinder
		return IntersectSegmentSphere(segment, cylinder.bottomSphere(), interPt, interNormal);
	} else if (pm_pq > pq2 - EPSILON) { // Above cylinder
		return IntersectSegmentSphere(segment, cylinder.topSphere(), interPt, interNormal);
	} else { // In cylinder, only possible when start is outside
		float pm_u = pm * u;
		Vector3 ph = u * pm_u;
		interNormal = !(interPt - (cylinder.pt1 + ph));
		return true;
	}
}

bool IntersectSegmentBoxRounded(Segment segment, BoxRounded box, Vector3& interPt, Vector3& interNormal) {
	float distSqr = -1;
	Vector3 interPtClosest;
	Vector3 interNormalClosest;
	Vector3 interPtTest;
	Vector3 interNormalTest;
	std::vector<Quad> quads = box.listQuads();
	for (auto quad : quads) {
		if (IntersectSegmentQuad(segment, quad, interPtTest, interNormalTest)) {
			float distTest = ~(interPtTest - segment.pt1);
			if (distSqr < 0 || distTest < distSqr) {
				distSqr = distTest;
				interPtClosest = interPtTest;
				interNormalClosest = interNormalTest;
			}
		}
	}
	std::vector<Cylinder> cylinders = box.listCylinders();
	for (auto cylinder : cylinders) {
		if (IntersectSegmentCylinderRounded(segment, cylinder, interPtTest, interNormalTest)) {
			float distTest = ~(interPtTest - segment.pt1);
			if (distSqr < 0 || distTest < distSqr) {
				distSqr = distTest;
				interPtClosest = interPtTest;
				interNormalClosest = interNormalTest;
			}
		}
	}
	if (distSqr > -EPSILON) {
		interPt = interPtClosest;
		interNormal = interNormalClosest;
		return true;
	}
	return false;
}

Vector3 GlobalToLocalPos(Vector3 posGlobal, Referential localRef) {
	return GlobalToLocalVect(posGlobal - localRef.origin, localRef);
}

Vector3 GlobalToLocalVect(Vector3 vectGlobal, Referential localRef) {
	return { vectGlobal * localRef.i, vectGlobal * localRef.j, vectGlobal * localRef.k };
}

Vector3 LocalToGlobalPos(Vector3 posLocal, Referential localRef) {
	return LocalToGlobalVect(posLocal + localRef.origin, localRef);
}

Vector3 LocalToGlobalVect(Vector3 vectLocal, Referential localRef) {
	return localRef.i * vectLocal.x + localRef.j * vectLocal.y + localRef.k * vectLocal.z;
}

Referential localReferential(Vector3 origin, Quaternion q) {
	return (Referential{
		{ 0, 0, 0 },
		{ 1, 0, 0 },
		{ 0, 1, 0 },
		{ 0, 0, 1 }
	} + origin) * q;
}
