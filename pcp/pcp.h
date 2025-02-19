#ifndef PCP_H
#define PCP_H

// #define WITH_GLEW_GL_PNG
#include <pcprep/graphic.h>
#include <pcprep/pointcloud.h>
#include <pcprep/aabb.h>
#include <stdint.h>

#define MAX_PROCESS 256
#define MAX_STATUS 256

#define PCP_PROC_SAMPLE 0x00
#define PCP_PROC_VOXEL 0x01
#define PCP_PROC_REMOVE_DUPLICATES 0x02

#define PCP_STAT_AABB 0x00
#define PCP_STAT_SCREEN_AREA 0x01
#ifdef WITH_GRAPHIC
#define PCP_STAT_SAVE_VIEWPORT 0x02
#endif

#define PCP_PLAN_NONE_NONE 0x00
#define PCP_PLAN_NONE_TILE 0x01
#define PCP_PLAN_NONE_MERGE 0x02
#define PCP_PLAN_TILE_NONE 0x10
#define PCP_PLAN_TILE_MERGE 0x12
#define PCP_PLAN_MERGE_NONE 0x20
#define PCP_PLAN_MERGE_TILE 0x21

#define SIZE_PATH 0xffff
#define SET_OPT_PROCESS 0x0001
#define SET_OPT_STATUS 0x0002

typedef struct func_t
{
    unsigned char func_id;
    char **func_arg;
    size_t func_arg_size;
} func_t;

typedef struct func_info {
    const char *name;
    unsigned char func;   // Maps to func_id
    int min_args;
    int max_args;
} func_info_t;

const func_info_t processes_g[] = 
{
    {
        "sample",
        PCP_PROC_SAMPLE,
        2, 2
    },
    {
        "voxel",
        PCP_PROC_VOXEL,
        1, 1
    },
    {
        "remove-duplicates",
        PCP_PROC_REMOVE_DUPLICATES,
        0, 0
    },
    {NULL, 0, 0, 0} 
};

const func_info_t statuses_g[] = 
{
    {
        "aabb",
        PCP_STAT_AABB,
        3, 3,
    },
#ifdef PCP_STAT_SAVE_VIEWPORT
    {
        "save-viewport",
        PCP_STAT_SAVE_VIEWPORT,
        3, 3,
    },
#endif
    {NULL, 0, 0, 0} 
};


struct arguments
{
    uint32_t flags;
    char *input;
    char *output;
    int binary;
    int tiled_input;
    unsigned char plan;
    size_t procs_size;
    size_t stats_size;
    func_t procs[MAX_PROCESS];
    func_t stats[MAX_STATUS];
    struct
    {
        uint8_t nx, ny, nz;
    } tile;
};

int arguments_free(struct arguments *arg)
{
    for (size_t i = 0; i < arg->procs_size; i++)
    {
        for (size_t j = 0; j < arg->procs[i].func_arg_size; j++)
        {
            free(arg->procs[i].func_arg[j]);
        }
        free(arg->procs[i].func_arg);
    }
    for (size_t i = 0; i < arg->stats_size; i++)
    {
        for (size_t j = 0; j < arg->stats[i].func_arg_size; j++)
        {
            free(arg->stats[i].func_arg[j]);
        }
        free(arg->stats[i].func_arg);
    }
    return 1;
}


const func_info_t *find_func(const char *name,
                             const func_info_t *funcs) 
{
    for (const func_info_t *p = funcs; p->name != NULL; p++) 
        if (strcmp(p->name, name) == 0) 
            return p;
    return NULL;
}

typedef struct pcp_sample_p_arg_t
{
    float ratio;
    unsigned char strategy;
} pcp_sample_p_arg_t;

unsigned int pcp_sample_p(pointcloud_t *pc,
                          void *arg)
{
    pcp_sample_p_arg_t *param = (pcp_sample_p_arg_t *)arg;

    pointcloud_t out = {NULL, NULL, 0};
    pointcloud_sample(*pc, param->ratio, param->strategy, &out);
    pointcloud_free(pc);
    *pc = out;
    return 1;
}

unsigned int pcp_voxel_p(pointcloud_t *pc,
                         void *arg)
{
    float step_size = *(float *)arg;

    pointcloud_t out = {NULL, NULL, 0};
    pointcloud_voxel(*pc, step_size, &out);
    pointcloud_free(pc);
    *pc = out;
    return 1;
}

unsigned int pcp_remove_dupplicates_p(pointcloud_t *pc,
                                      void *arg)
{
    pointcloud_t out = {NULL, NULL, 0};
    pointcloud_remove_dupplicates(*pc, &out);
    pointcloud_free(pc);
    *pc = out;
    return 1;
}

typedef struct pcp_aabb_s_arg_t
{
    int output;
    int binary;
    char *output_path;
    int id;
} pcp_aabb_s_arg_t;

unsigned int pcp_aabb_s(pointcloud_t *pc,
                        void *arg)
{
    pcp_aabb_s_arg_t *param = (pcp_aabb_s_arg_t *)arg;

    vec3f_t min, max;
    pointcloud_min(*pc, &min);
    pointcloud_max(*pc, &max);
    if (param->output == 0 || param->output == 2)
    {
        printf("Min: %f %f %f\n", min.x, min.y, min.z);
        printf("Max: %f %f %f\n", max.x, max.y, max.z);
        if (param->output == 0)
        {
            return 1;
        }
    }
    aabb_t aabb = {min, max};
    mesh_t mesh = (mesh_t){NULL, 0, NULL, 0};
    aabb_to_mesh(aabb, &mesh);

    char tile_path[SIZE_PATH];
    snprintf(tile_path, SIZE_PATH, param->output_path, param->id);
    mesh_write(mesh, tile_path, param->binary);
    return 1;
}

#ifdef PCP_STAT_SAVE_VIEWPORT
typedef struct pcp_save_viewport_s_arg_t
{
    char *output_path;
    int tile_id;
    float *mvp; // 4x4 matrix
    int width;
    int height;
    vec3f_t background;
    int frame_id;
} pcp_save_viewport_s_arg_t;
unsigned int pcp_save_viewport_s(pointcloud_t *pc,
                                 void *arg)
{
    pcp_save_viewport_s_arg_t *param = (pcp_save_viewport_s_arg_t *)arg;

    unsigned char *pixels = (unsigned char *)malloc(param->height * param->width * 3 * sizeof(unsigned char));
    pointcloud_get_viewport(*pc, 
                            param->mvp, 
                            param->width, 
                            param->height, 
                            NULL,
                            NULL,
                            param->background,
                            pixels);

    unsigned char **row_pointers = malloc(param->height * sizeof(unsigned char *));
    for (int i = 0; i < param->height; i++) 
        row_pointers[i] = malloc(param->width * 3); 

    flip_image(row_pointers, pixels, param->width, param->height);
    free(pixels);

    char tile_path[SIZE_PATH];
    snprintf(tile_path, SIZE_PATH, param->output_path, param->tile_id, param->frame_id);

    save_viewport(row_pointers, param->width, param->height, tile_path);
    for (int i = 0; i < param->height; i++) 
        free(row_pointers[i]);
    free(row_pointers);
    return 1;
}

#endif
#endif