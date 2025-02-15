#ifndef WRAPPER_H
#define WRAPPER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
    // Declare the wrapper function
    int ply_count_vertex(const char *filename);
    int ply_count_face(const char *filename);
    int ply_pointcloud_loader(const char *filename, float *pos, unsigned char *rgb);
    int ply_mesh_loader(const char *filename, float *pos, int *indices);

#ifdef __cplusplus
}
#endif

#endif // WRAPPER_H
