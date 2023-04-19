#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <vector>

#include "RC_Engine.h"
#include "RC_Core.h"

#define W 1280
#define H 768

int main(){
	rc::Engine engine(W, H);
	engine.run();
	return 0;
}
