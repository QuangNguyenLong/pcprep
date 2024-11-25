#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <stdint.h>
#include "pcprep/vec3f.h"

typedef struct pointcloud_t
{
    float *pos;
    uint8_t *rgb;
    size_t size;
} pointcloud_t;

int pointcloud_init(pointcloud_t *pc, size_t size);
int pointcloud_load(pointcloud_t *pc, const char *filename);
int pointcloud_write(pointcloud_t *pc, const char *filename, int binary);
int pointcloud_min(pointcloud_t *pc, vec3f_t *min);
int pointcloud_max(pointcloud_t *pc, vec3f_t *max);
int get_tile_id(vec3f_t n, vec3f_t min, vec3f_t max, vec3f_t v);
int pointcloud_tiling(pointcloud_t *pc,
                      int n_x,
                      int n_y,
                      int n_z,
                      pointcloud_t **tiles);

#ifdef __cplusplus
}
#endif
#endif