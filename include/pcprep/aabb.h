#ifndef AABB_H
#define AABB_H

#ifdef __cplusplus
extern "C"
{
    
#endif

#include <pcprep/vec3f.h>

typedef struct aabb_t
{
    vec3f_t min;
    vec3f_t max;
} aabb_t;


#ifdef __cplusplus
}
#endif
#endif