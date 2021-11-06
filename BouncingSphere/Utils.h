#pragma once
#include "raylib.h"

#define EPSILON 1.e-6f

template <typename T> int sgn(T val);
bool approxZero(float val);
float min(float a, float b);
Quaternion QuaternionConjuguate(Quaternion q);
