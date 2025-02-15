#include <argp.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pcprep/pointcloud.h>
#include "pcp.h"

int pcp_parsefunction(char             *arg, 
                      unsigned char    *func_id,
                      char             ***func_arg, 
                      size_t           *func_arg_size)
{
    int count_arg = 0;
    for(char *p = &arg[0]; *p != '\0'; p++)
    {
        if(*p == ',')
            count_arg++;
    }
    *func_arg_size = count_arg;
    *func_arg = malloc(count_arg * sizeof(char *));
    count_arg = 0;

    char *first_token = strtok(arg, ",");
    *func_id = atoi(first_token);

    for(char *token = strtok(NULL, ","); 
        token != NULL; 
        token = strtok(NULL, ","))
    {
        (*func_arg)[count_arg] = malloc(strlen(token) * sizeof(char));
        strcpy((*func_arg)[count_arg], token);
        count_arg++;
    }
    return 1;
}

int pcp_prepare(struct arguments *arg)
{

    pointcloud_t        input           = {NULL, NULL, 0};
    pointcloud_t        *outputs        = NULL;
    char                *input_path     = NULL;
    char                *output_path    = NULL;
    char 		        *tile_path      = NULL;
    int                 max_path_size   = 0;
    int                 binary          = 0;
    int                 out_count       = 0;
    
    max_path_size       = SIZE_PATH;
    input_path          = arg->input;
    output_path         = arg->output;
    binary              = arg->binary;
    tile_path           = malloc(max_path_size * sizeof(char));

    pointcloud_load(&input, input_path);

    // this needs ERROR control
    if (arg->flags & SET_OPT_TILE) 
    {
    // Handle the SET_OPT_TILE flag
        int 		    nx 	= 0;
        int 		    ny 	= 0;
        int 		    nz 	= 0;
        nx 			    = arg->tile.nx;
        ny 			    = arg->tile.ny;
        nz 			    = arg->tile.nz;
        out_count = pointcloud_tile(&input, nx, ny, nz, &outputs);
    }
    else
        out_count = pointcloud_tile(&input, 1, 1, 1, &outputs); 

    if (arg->flags & SET_OPT_PROCESS) 
    {
    // Handle the SET_OPT_PROCESS flag
        for (int i = 0; i < arg->procs_size; i++)
        {
            process_t *curr = &(arg->procs[i]);
            switch (curr->process)
            {
            case PCP_PROC_SAMPLE:
            {
                pcp_sample_p_arg_t param = {1.0f, 0};
                float percent = atof(curr->process_arg[0]);

                if(percent > 0.0f && percent < 1.0f)
                    param.ratio = percent;

                param.strategy = atoi(curr->process_arg[1]);
            
                printf("[PROCESS] Sample: {ratio:%f,strategy:%d}\n",
                    param.ratio,
                    param.strategy);
        
                for(int t = 0; t < out_count; t++)
                {
                    printf("-------Tile %d-------\n", t);
                    pcp_sample_p(&outputs[t], (void *)&param);
                }
                break;
            }
            case PCP_PROC_VOXEL:
            {
                float step_size = atof(curr->process_arg[0]);
                printf("[PROCESS] Voxel: {step_size:%f}\n", step_size);
                for(int t = 0; t < out_count; t++)
                {
                    printf("-------Tile %d-------\n", t);
                    pcp_voxel_p(&outputs[t], (void *)&step_size);
                }
                break;
            }
            case PCP_PROC_OCTREE:
            {
                break;
            }
            default:
            {
                break;
            }
            }           
        }
    }
    if (arg->flags & SET_OPT_STATUS) 
    {
    // Handle the SET_OPT_STATUS flag
        for (int i = 0; i < arg->stats_size; i++)
        {
            status_t *curr = &(arg->stats[i]);
            switch (curr->status)
            {
            case PCP_STAT_AABB:
            {
                pcp_aabb_s_arg_t param = {0, 0, NULL};
                param.output = atoi(curr->status_arg[0]);
                param.binary = atoi(curr->status_arg[1]);
                param.output_path = curr->status_arg[2];
                
                printf("[STATUS] AABB: {output:%d,binary:%d,output_path:%s}\n",
                    param.output,
                    param.binary,
                    param.output_path);
                    
                for(int t = 0; t < out_count; t++)
                {
                    printf("-------Tile %d-------\n", t);
                    param.tile_id = t;
                    pcp_aabb_s(&outputs[t], (void *)&param);
                }
                break;
            }
            default:
            {
                break;
            }
            }           
        }
    }

    if(out_count == 0)
        return 0;
    for (int t = 0; t < out_count; t++)
    {
        if (outputs[t].size == 0)
        {
            printf("Tile %d have no points, skip writing...\n", t);
        }
    snprintf(tile_path, max_path_size, output_path, t);
    pointcloud_write(&outputs[t], tile_path, binary);
    }
    return out_count;
}

const char *argp_program_version = "pcp 0.1";
const char *argp_program_bug_address = "quang.nglong@gmail.com";

static char doc[] = "A program that prepare point cloud.";
static char args_doc[] = "-i <point-cloud-source-file> -o <output-source-file(s)>";

static struct argp_option options[] = {
    {"input",           'i',        "FILE",                0, 
    "Input point cloud source file (current support Polygon File Format)."},
    {"output",          'o',        "FILE",                0, 
    "Output file(s)."},
    {"binary",          'b',        "0|1",                 0, 
    "output binary or not (0 for not, default is 1)."},
    {"tile",            't',        "nx,ny,nz",            0, 
    "set number of division per axis (default is 1,1,1)."},
    {"process",         'p',        "P_ID,ARG,...",        0,
    "Process which the point cloud undergo."},
    {"status",          's',        "S_ID,ARG,...",        0,
    "Status of the point cloud given factors."},
    {0}
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *args = state->input;

    switch (key) 
    {
        case 'i': { args->input = arg; break; }
        case 'o': { args->output = arg; break; }
        case 'b': { args->binary = atoi(arg); break; }
        case 't': 
        {
            if(sscanf(arg, "%hhu,%hhu,%hhu", 
                        &(args->tile.nx), 
                        &(args->tile.ny), 
                        &(args->tile.nz)) != 3)
            {
                argp_state_help(state, stderr, ARGP_HELP_USAGE);
                return ARGP_ERR_UNKNOWN;
            }
            args->flags |= SET_OPT_TILE;
            break;
        }
        case 'p': 
        {
            process_t *curr = &(args->procs[args->procs_size]);

            if(!pcp_parsefunction(arg, 
                                   &(curr->process), 
                                   &(curr->process_arg),
                                   &(curr->process_arg_size)))
            {
                argp_state_help(state, stderr, ARGP_HELP_USAGE);
                return ARGP_ERR_UNKNOWN;
            }
            args->flags |= SET_OPT_PROCESS;
            args->procs_size++;
            break;
        }
        case 's': 
        {
            status_t *curr = &(args->stats[args->stats_size]);

            if(!pcp_parsefunction(arg, 
                                   &(curr->status), 
                                   &(curr->status_arg),
                                   &(curr->status_arg_size)))
            {
                argp_state_help(state, stderr, ARGP_HELP_USAGE);
                return ARGP_ERR_UNKNOWN;
            }
            args->flags |= SET_OPT_STATUS;
            args->stats_size++;
            break;
        }
        default: { return ARGP_ERR_UNKNOWN; }
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char *argv[]) 
{
    struct arguments args = {0, NULL, NULL, 1, 0, 0};

    argp_parse(&argp, argc, argv, 0, 0, &args);

    printf("input: %s\n",       args.input);
    printf("output: %s\n",      args.output);
    printf("binary: %d\n",      args.binary);
    
    pcp_prepare(&args);

    return 0;
}
