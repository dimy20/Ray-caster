#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include "wiz.h"

#define PPM_MAGIC "P6"
void save_ppm(const char * filename, Canvas * canvas);
