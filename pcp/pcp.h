#ifndef PCP_H
#define PCP_H

#include <pcprep/pointcloud.h>
#include <pcprep/aabb.h>
#include <stdint.h>

#define MAX_PROCESS 256
#define MAX_STATUS 256

#define PCP_PROC_SAMPLE 0x00
#define PCP_PROC_VOXEL 0x01
#define PCP_PROC_OCTREE 0x02

#define PCP_STAT_AABB 0x00
#define PCP_STAT_SCREEN_AREA 0x01

#define SIZE_PATH 0xffff
#define SET_OPT_TILE 0x0001
#define SET_OPT_PROCESS 0x0002
#define SET_OPT_STATUS 0x0004

typedef struct process_t
{
    unsigned char process;
    char **process_arg;
    size_t process_arg_size;
} process_t;

typedef struct status_t
{
    unsigned char status;
    char **status_arg;
    size_t status_arg_size;
} status_t;

struct arguments
{
    uint32_t flags;
    char *input;
    char *output;
    int binary;
    size_t procs_size;
    size_t stats_size;
    process_t procs[MAX_PROCESS];
    status_t stats[MAX_STATUS];
    struct
    {
        uint8_t nx, ny, nz;
    } tile;
};

typedef struct pcp_sample_p_arg_t
{
    float ratio;
    unsigned char strategy;
} pcp_sample_p_arg_t;

// typedef unsigned int (*pcp_process)(pointcloud_t    *pc,
//                                     void            *arg);

unsigned int pcp_sample_p(pointcloud_t *pc,
                          void *arg)
{
    pcp_sample_p_arg_t *param = (pcp_sample_p_arg_t *)arg;


    pointcloud_t *out;
    pointcloud_sample(pc, param->ratio, param->strategy, &out);
    pointcloud_free(pc);
    // pc = out is wrong, pc is the pass-by-value pointer
    // which point to the real point cloud,
    // *pc is the referenced point cloud
    *pc = *out;
}

unsigned int pcp_voxel_p(pointcloud_t *pc,
                         void *arg)
{
    float step_size = *(float *)arg;

    pointcloud_t *out;
    pointcloud_voxel(pc, step_size, &out);
    pointcloud_free(pc);
    *pc = *out;
}

unsigned int pcp_octree_p(pointcloud_t *pc,
                          void *arg)
{
}

// pcp_process legs_g[MAX_PROCESS] = {pcp_sample_p,
//                                    pcp_voxel_p,
//                                    pcp_octree_p};

typedef struct pcp_aabb_s_arg_t
{
    int output;
    int binary;
    char *output_path;
    int tile_id;
} pcp_aabb_s_arg_t;



unsigned int pcp_aabb_s(pointcloud_t *pc,
                        void *arg)
{
    pcp_aabb_s_arg_t *param = (pcp_aabb_s_arg_t *)arg;

    vec3f_t min, max;
    pointcloud_min(pc, &min);
    pointcloud_max(pc, &max);
    if (param->output == 0 || param->output == 2)
    {
        printf("Min: %f %f %f\n", min.x, min.y, min.z);
        printf("Max: %f %f %f\n", max.x, max.y, max.z);
        if(param->output == 0)
        {
            return 1;
        }
    }
    aabb_t aabb = {min, max};
    mesh_t mesh = (mesh_t){NULL, 0, NULL, 0};
    aabb_to_mesh(&aabb, &mesh);

    char tile_path[SIZE_PATH];
    snprintf(tile_path, SIZE_PATH, param->output_path, param->tile_id);
    mesh_write(&mesh, tile_path, param->binary);
}

#endif