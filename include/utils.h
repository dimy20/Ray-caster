#pragma once

#include <cmath>

#define UNIMPLEMENTED do{ \
	fprintf(stderr, "UNIMPLEMENTED %s:%d\n", __FILE__, __LINE__); \
	exit(1); \
}while(0);

#define RC_DIE(v, s) do{													\
	if(v){																	\
		fprintf(stderr, "Error : %s at %s:%d\n", s, __FILE__, __LINE__);    \
		exit(1);															\
	}																		\
}while(0);

namespace rc{
	constexpr double to_rad(double deg){ return (deg * M_PI) / 180.0; };
	constexpr double first_quadrant(double a) { return (a >= 0.0 && a <= 90.0); }
	constexpr double fourth_quadrant(double a) { return (a >= 270.0 && a <= 360.0); }
}
