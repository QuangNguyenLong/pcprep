#ifndef PCP_H
#define PCP_H

#include <pcprep/pointcloud.h>
#include <stdint.h>

#define MAX_PROCESS 256
#define MAX_STATUS 256

#define PCP_PROC_SAMPLE 0x00
#define PCP_PROC_OCTREE 0x01

#define PCP_STAT_SCREEN_AREA 0x00

#define SIZE_PATH               0xffff
#define SET_OPT_TILE 	        0x0001
#define SET_OPT_PROCESS 	    0x0002
#define SET_OPT_STATUS          0x0004

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

typedef struct pcp_sample_arg_t
{
    float ratio;
    unsigned char strategy;
} pcp_sample_arg_t;

typedef unsigned int (*pcp_process)(pointcloud_t    *pc,
                                    void            *arg);

unsigned int pcp_sample_p(pointcloud_t     *pc,
                          void             *arg)
{
    pcp_sample_arg_t *param = (pcp_sample_arg_t *)arg;

    pointcloud_t *out;
    pointcloud_sample(pc, param->ratio, param->strategy, &out);
    pointcloud_free(pc);
    // pc = out is wrong, pc is the pass-by-value pointer 
    // which point to the real point cloud,
    // *pc is the referenced point cloud 
    *pc = *out; 
}

unsigned int pcp_octree_p(pointcloud_t     *pc,
                          void             *arg)
{

}

pcp_process legs_g[MAX_PROCESS] = {pcp_sample_p,
                                   pcp_octree_p};


#endif