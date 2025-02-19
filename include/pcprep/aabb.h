#ifndef AABB_H
#define AABB_H

#ifdef __cplusplus
extern "C"
{
    
#endif

#include <pcprep/vec3f.h>
#include <pcprep/mesh.h>

typedef struct aabb_t
{
    vec3f_t min;
    vec3f_t max;
} aabb_t;

int aabb_to_mesh(aabb_t aabb, mesh_t *mesh);

#ifdef __cplusplus
}
#endif
#endif