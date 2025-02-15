#ifndef MESH_H
#define MESH_H

#ifdef __cplusplus
extern "C"
{
    
#endif

#include <stdlib.h>

typedef struct mesh_t
{
  float* pos; 
  float* uv; 
  size_t num_verts;
  int* indices;
} mesh_t;

// int mesh_init(mesh_t *m, size_t size);
// int mesh_free(mesh_t *m);
// int mesh_load(mesh_t *m, const char *filename);
// int mesh_write(mesh_t *m, const char *filename, int binary);

#ifdef __cplusplus
}
#endif
#endif