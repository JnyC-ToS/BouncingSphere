#pragma once
#include "raylib.h"

// PLAIN OBJECTS

void MyDrawQuad(Quaternion q, Vector3 center, Vector2 size, Color color);
void MyDrawQuadWires(Quaternion q, Vector3 center, Vector2 size, Color color);

void MyDrawSphere(Quaternion q, Vector3 center, float radius, int nSegmentsTheta, int nSegmentsPhi, Color color);
void MyDrawSphereWires(Quaternion q, Vector3 center, float radius, int nSegmentsTheta, int nSegmentsPhi, Color color);

void MyDrawCylinder(Quaternion q, Vector3 start, Vector3 end, float radius, int nSegments, bool drawCaps, Color color);
void MyDrawCylinderWires(Quaternion q, Vector3 start, Vector3 end, float radius, int nSegments, bool drawCaps, Color color);

void MyDrawDisk(Quaternion q, Vector3 center, float radius, int nSegments, Color color);
void MyDrawDiskWires(Quaternion q, Vector3 center, float radius, int nSegments, Color color);

// PARTIAL OBJECTS

void MyDrawSpherePortion(Quaternion q, Vector3 center, float radius, float startTheta, float endTheta, int nSegmentsTheta, float startPhi, float endPhi, int nSegmentsPhi, Color color);
void MyDrawSphereWiresPortion(Quaternion q, Vector3 center, float radius, float startTheta, float endTheta, int nSegmentsTheta, float startPhi, float endPhi, int nSegmentsPhi, Color color);

void MyDrawCylinderPortion(Quaternion q, Vector3 start, Vector3 end, float radius, float startSegments, float endSegments, int nSegments, bool drawCaps, Color color);
void MyDrawCylinderWiresPortion(Quaternion q, Vector3 start, Vector3 end, float radius, float startSegments, float endSegments, int nSegments, bool drawCaps, Color color);

void MyDrawDiskPortion(Quaternion q, Vector3 center, float radius, float startSegments, float endSegments, int nSegments, Color color);
void MyDrawDiskWiresPortion(Quaternion q, Vector3 center, float radius, float startSegments, float endSegments, int nSegments, Color color);

void MySuperTest();
