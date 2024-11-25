#ifndef WRAPPER_H
#define WRAPPER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
    // Declare the wrapper function
    int ply_count_element(const char *filename);
    int ply_loader(const char *filename, float *pos, unsigned char *rgb);

#ifdef __cplusplus
}
#endif

#endif // WRAPPER_H
