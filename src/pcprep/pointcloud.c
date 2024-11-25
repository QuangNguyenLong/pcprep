#include "pcprep/pointcloud.h"
#include "pcprep/wrapper.h"
#include "pcprep/vec3f.h"
#include <stdio.h>

#define MSH_PLY_INCLUDE_LIBC_HEADERS
#define MSH_PLY_IMPLEMENTATION
#include <msh_ply.h>

int pointcloud_init(pointcloud_t *pc, size_t size)
{
    if (size < 0)
        return -1;
    pc->size = size;
    pc->pos = (float *)malloc(sizeof(float) * 3 * pc->size);
    pc->rgb = (uint8_t *)malloc(sizeof(uint8_t) * 3 * pc->size);
    return pc->size;
}
int pointcloud_load(pointcloud_t *pc, const char *filename)
{
    pointcloud_init(pc, ply_count_element(filename));
    return ply_loader(filename, pc->pos, pc->rgb);
}
int pointcloud_write(pointcloud_t *pc, const char *filename, int binary)
{
    const char *vertex_attributes[] = {"x", "y", "z",
                                       "red", "green", "blue"};
    int32_t vertex_count = pc->size;
    if (vertex_count == 0)
        return -1;

    msh_ply_desc_t positions_desc = {
        (char *)"vertex",
        &vertex_attributes[0],
        3,
        MSH_PLY_FLOAT,
        MSH_PLY_INVALID,
        &pc->pos,
        NULL,
        &vertex_count,
        0};

    msh_ply_desc_t colors_desc = {
        (char *)"vertex",
        &vertex_attributes[3],
        3,
        MSH_PLY_UINT8,
        MSH_PLY_INVALID,
        &pc->rgb,
        NULL,
        &vertex_count,
        0};

    const char *write_format = binary ? "wb" : "w";
    msh_ply_t *pf = msh_ply_open(filename, write_format);
    if (pf)
    {
        msh_ply_add_descriptor(pf, &positions_desc);
        msh_ply_add_descriptor(pf, &colors_desc);
        msh_ply_write(pf);
    }
    msh_ply_close(pf);
    return vertex_count;
}

int pointcloud_min(pointcloud_t *pc, vec3f_t *min)
{
    if (!pc)
        return -1;
    vec3f_t *pos_lst = (vec3f_t *)(pc->pos);
    *min = pos_lst[0];
    for (int i = 0; i < pc->size; i++)
    {
        if (pos_lst[i].x < min->x)
            min->x = pos_lst[i].x;
        if (pos_lst[i].y < min->y)
            min->y = pos_lst[i].y;
        if (pos_lst[i].z < min->z)
            min->z = pos_lst[i].z;
    }
    return 0;
}
int pointcloud_max(pointcloud_t *pc, vec3f_t *max)
{
    if (!pc)
        return -1;
    vec3f_t *pos_lst = (vec3f_t *)(pc->pos);
    *max = pos_lst[0];
    for (int i = 0; i < pc->size; i++)
    {
        if (pos_lst[i].x > max->x)
            max->x = pos_lst[i].x;
        if (pos_lst[i].y > max->y)
            max->y = pos_lst[i].y;
        if (pos_lst[i].z > max->z)
            max->z = pos_lst[i].z;
    }
    return 0;
}

int get_tile_id(vec3f_t n, vec3f_t min, vec3f_t max, vec3f_t v)
{
    if (vec3f_g(v, max) || vec3f_l(v, min))
        return -1;
    vec3f_t ans;
    // ans = (v - min) / ((max - min) / n);
    // or ans = (v - min) * (invs(max - min) / n);
    // or ans = (v - min) * (invs(max - min) * invs(n));
    // or ans = (v - min) * invs(max - min) * n);
    ans = vec3f_mul(vec3f_mul(
                        vec3f_sub(v, min),
                        vec3f_inverse(vec3f_sub(max, min))),
                    n);
    ans = (vec3f_t){(int)(ans.x < n.x ? ans.x : n.x - 1),
                    (int)(ans.y < n.y ? ans.y : n.y - 1),
                    (int)(ans.z < n.z ? ans.z : n.z - 1)};
    for (int i = 0, a = 0, b = 0, c = 0; i < n.x * n.y * n.z; i++)
    {
        if (c == n.z)
        {
            c = 0;
            b++;
        }
        if (b == n.y)
        {
            b = 0;
            a++;
        }
        if (vec3f_eq(ans, vec3f_int((vec3f_t){a, b, c})))
            return i;
        c++;
    }
    return -1;
}

// TODO: this function is not safe
int pointcloud_tiling(pointcloud_t *pc,
                      int n_x,
                      int n_y,
                      int n_z,
                      pointcloud_t **tiles)
{
    vec3f_t min, max;
    vec3f_t *pos_lst = (vec3f_t *)pc->pos;
    vec3f_t n = (vec3f_t){n_x, n_y, n_z};
    int size = n.x * n.y * n.z;

    int *numVerts = (int *)malloc(sizeof(int) * size);
    int *tmp = (int *)malloc(sizeof(int) * size);

    *tiles = (pointcloud_t *)malloc(sizeof(pointcloud_t) * size);

    pointcloud_min(pc, &min);
    pointcloud_max(pc, &max);

    for (int t = 0; t < size; t++)
    {
        numVerts[t] = 0;
        tmp[t] = 0;
    }
    for (int i = 0; i < pc->size; i++)
    {
        int t = get_tile_id(n, min, max, pos_lst[i]);
        numVerts[t]++;
    }

    for (int t = 0; t < size; t++)
    {
        pointcloud_init(&(*tiles)[t], numVerts[t]);
    }

    for (int i = 0; i < pc->size; i++)
    {
        int t = get_tile_id(n, min, max, pos_lst[i]);
        (*tiles)[t].pos[3 * tmp[t]] = pc->pos[3 * i];
        (*tiles)[t].pos[3 * tmp[t] + 1] = pc->pos[3 * i + 1];
        (*tiles)[t].pos[3 * tmp[t] + 2] = pc->pos[3 * i + 2];
        (*tiles)[t].rgb[3 * tmp[t]] = pc->rgb[3 * i];
        (*tiles)[t].rgb[3 * tmp[t] + 1] = pc->rgb[3 * i + 1];
        (*tiles)[t].rgb[3 * tmp[t] + 2] = pc->rgb[3 * i + 2];
        tmp[t]++;
    }

    free(numVerts);
    free(tmp);

    return size;
}
