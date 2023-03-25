#pragma once

#define M_PI 3.14159265358979323846
#define TO_RAD(d) ((d) * M_PI) / 180.0

typedef struct{
	double x;
	double y;
}vec2f;

typedef struct{
	int x;
	int y;
}vec2i;
