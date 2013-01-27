#include <stdint.h>
#include <stdbool.h>
#include "util.h"

void seq(ATYPE *x, uint n, uint *opsp);
void reduction(ATYPE *x, uint n, uint *opsp);
void inplace(ATYPE *x, uint n, uint *opsp);
void auxarr(ATYPE *x, uint n, uint *opsp);
void hillis(ATYPE *x, uint n, uint *opsp);
