#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include "engine.h"
#include "map.h"

#define W 1024
#define H 728

int main(){
	engine_init(W, H);
	engine_run();
	engine_quit();
}
