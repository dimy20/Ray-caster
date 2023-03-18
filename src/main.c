#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include "ppm.h"
#include "wiz.h"

#define W 1024
#define H 728

int main(){
	Canvas * canvas = create_basic_canvas(W, H);
	basic_gradient(canvas);
	save_ppm("test.ppm", canvas);
	free_basic_canvas(canvas);
}


