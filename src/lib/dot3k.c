#include "dot3k_private.h"
#include <stdlib.h>

DOT3K *dot3k_init() {
    return (DOT3K*)calloc(1, sizeof(DOT3K));
}
