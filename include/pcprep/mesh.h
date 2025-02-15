#ifndef MESH_H
#define MESH_H

#ifdef __cplusplus
extern "C"
{
    
#endif

#include <stdint.h>
#include <stdlib.h>

typedef struct mesh_t
{
  float* pos; 
  uint32_t num_verts;
  uint32_t* indices;
  uint32_t num_indices;
} mesh_t;

int mesh_init(mesh_t* mesh, uint32_t num_verts, uint32_t num_indices);
int mesh_free(mesh_t* mesh);
int mesh_load(mesh_t* mesh, const char* filename);
int mesh_write(mesh_t* mesh, const char* filename, int binary);

#ifdef __cplusplus
}
#endif
#endif