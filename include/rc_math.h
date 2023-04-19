#pragma once

#include <cmath>

namespace rc{
	constexpr double to_rad(double deg){ return (deg * M_PI) / 180.0; };
}

#define MAX(a,b) (a) > (b) ? (a) : (b)
#define MIN(a,b) (a) > (b) ? (b) : (a)
#define TO_RAD(d) ((d) * M_PI) / 180.0
