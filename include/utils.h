#pragma once

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
