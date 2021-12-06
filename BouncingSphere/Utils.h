#ifndef __UTILS_H__
#define __UTILS_H__

#include "raylib.h"
#include "raymath.h"
#include <cstdlib>

#define EPSILON 1.e-6f
#define CYLINDER_CAPS_NONE 0
#define CYLINDER_CAPS_FLAT 1
#define CYLINDER_CAPS_ROUNDED 2

template <typename T> int sgn(T val);
bool approxZero(float val);
float min(float a, float b);

inline float random() {
	return rand() / (float) RAND_MAX;
}

inline float randPos() {
	return random() * 2 - 1;
}

// Add
inline Vector3 operator+(Vector3 v1, Vector3 v2) {
	return Vector3Add(v1, v2);
}

// Scale (right)
inline Vector3 operator*(Vector3 v, float f) {
	return Vector3Scale(v, f);
}

// Scale (left)
inline Vector3 operator*(float f, Vector3 v) {
	return Vector3Scale(v, f);
}

// Subtract
inline Vector3 operator-(Vector3 v1, Vector3 v2) {
	return Vector3Subtract(v1, v2);
}

// Negate
inline Vector3 operator-(Vector3 v) {
	return Vector3Negate(v);
}

// Dot Product
inline float operator*(Vector3 v1, Vector3 v2) {
	return Vector3DotProduct(v1, v2);
}

// Cross Product
inline Vector3 operator^(Vector3 v1, Vector3 v2) {
	return Vector3CrossProduct(v1, v2);
}

// Normalize
inline Vector3 operator!(Vector3 v) {
	return Vector3Normalize(v);
}

// Length Squared
inline float operator~(Vector3 v) {
	return Vector3LengthSqr(v);
}

// Reflect
inline Vector3 operator/(Vector3 v, Vector3 normal) {
	return Vector3Reflect(v, normal);
}

// Rotate (right)
inline Vector3 operator*(Vector3 v, Quaternion q) {
	return Vector3RotateByQuaternion(v, q);
}

// Rotate (left)
inline Vector3 operator*(Quaternion q, Vector3 v) {
	return Vector3RotateByQuaternion(v, q);
}

// Multiply
inline Quaternion operator*(Quaternion q1, Quaternion q2) {
	return QuaternionMultiply(q1, q2);
}

#endif
