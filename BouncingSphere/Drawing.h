#pragma once
#include "raylib.h"

// PLAIN OBJECTS

void MyDrawQuad(Quaternion q, Vector3 center, Vector2 size, Color color);
void MyDrawQuadWires(Quaternion q, Vector3 center, Vector2 size, Color color);

// TODO Simplifier en appelant la version partielle de 0 à 2π
void MyDrawSphere(Quaternion q, Vector3 centerPos, float radius, int nSegmentsTheta, int nSegmentsPhi, Color color);
void MyDrawSphereWires(Quaternion q, Vector3 centerPos, float radius, int nSegmentsTheta, int nSegmentsPhi, Color color);

// TODO Simplifier en appelant la version partielle de 0 à 2π
void MyDrawCylinder(Quaternion q, Vector3 start, Vector3 end, float radius, int nSegments, bool drawCaps, Color color);
void MyDrawCylinderWires(Quaternion q, Vector3 start, Vector3 end, float radius, int nSegments, bool drawCaps, Color color);

// TODO Extraire depuis le tracé de cylindre
void MyDrawDisk(Quaternion q, Vector3 center, float radius, int nSegmentsTheta, Color color);
void MyDrawDiskWires(Quaternion q, Vector3 center, float radius, int nSegmentsTheta, Color color);

// PARTIAL OBJECTS

// TODO Extraire depuis la sphère complète
void MyDrawSpherePortion(Quaternion q, Vector3 centerPos, float radius, float startTheta, float endTheta, int nSegmentsTheta, float startPhi, float endPhi, int nSegmentsPhi, Color color);
void MyDrawSphereWiresPortion(Quaternion q, Vector3 centerPos, float radius, float startTheta, float endTheta, int nSegmentsTheta, float startPhi, float endPhi, int nSegmentsPhi, Color color);

// TODO Extraire depuis le cylindre complet
void MyDrawCylinderPortion(Quaternion q, Vector3 start, Vector3 end, float radius, float startSegments, float endSegments, int nSegments, bool drawCaps, Color color);
void MyDrawCylinderWiresPortion(Quaternion q, Vector3 start, Vector3 end, float radius, float startSegments, float endSegments, int nSegments, bool drawCaps, Color color);

// TODO Extraire depuis le disque complet
void MyDrawDiskPortion(Quaternion q, Vector3 center, float radius, float startSegments, float endSegments, int nSegments, Color color);
void MyDrawDiskWiresPortion(Quaternion q, Vector3 center, float radius, float startSegments, float endSegments, int nSegments, Color color);
