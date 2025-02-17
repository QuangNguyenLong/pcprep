#include <argp.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pcprep/pointcloud.h>
#include "pcp.h"

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
            func_t *curr = &(arg->procs[i]);
            switch (curr->func_id)
            {
            case PCP_PROC_SAMPLE:
            {
                pcp_sample_p_arg_t param = {1.0f, 0};
                float percent = atof(curr->func_arg[0]);

                if(percent > 0.0f && percent < 1.0f)
                    param.ratio = percent;

                param.strategy = atoi(curr->func_arg[1]);
            
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
                float step_size = atof(curr->func_arg[0]);
                printf("[PROCESS] Voxel: {step_size:%f}\n", step_size);
                for(int t = 0; t < out_count; t++)
                {
                    printf("-------Tile %d-------\n", t);
                    pcp_voxel_p(&outputs[t], (void *)&step_size);
                }
                break;
            }
            case PCP_PROC_REMOVE_DUPLICATES:
            {
                printf("[PROCESS] Remove dupplicates: \n");
                for(int t = 0; t < out_count; t++)
                {
                    printf("-------Tile %d-------\n", t);
                    pcp_remove_dupplicates_p(&outputs[t], NULL);
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
    if (arg->flags & SET_OPT_STATUS) 
    {
    // Handle the SET_OPT_STATUS flag
        for (int i = 0; i < arg->stats_size; i++)
        {
            func_t *curr = &(arg->stats[i]);
            switch (curr->func_id)
            {
            case PCP_STAT_AABB:
            {
                pcp_aabb_s_arg_t param = {0, 0, NULL};
                param.output = atoi(curr->func_arg[0]);
                param.binary = atoi(curr->func_arg[1]);
                param.output_path = curr->func_arg[2];
                
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
static struct argp_option options[] = {
    {"input",           'i',        "FILE",                0, 
    "Input point cloud source file (current support Polygon File Format)."},
    {"output",          'o',        "FILE",                0, 
    "Output point cloud source file(s) (current support Polygon File Format)."},
    {"binary",          'b',        "0|1",                 0, 
    "output binary or not (0 for not, default is 1)."},
    {"tile",            't',        "nx,ny,nz",            0, 
    "set number of division per axis (default is 1,1,1)."},
    {"process",         'p',        "PROCESS",             0,
    "Process which the point cloud undergo, use '--process help' for more info."},
    {"status",          's',        "STATUS",              0,
    "Status of the point cloud given factors, use '--status help' for more info."},
    {0}
};

static char process_doc[] = 
"Defines a specific process to be applied to the point cloud.";
static struct argp_option process_options[] = {
    {"help",                    0, NULL, OPTION_DOC,
    "Give this help list"},
    {"sample",                  0, NULL, OPTION_DOC, 
    "<ratio> <binary>"},
    {"voxel",                   0, NULL, OPTION_DOC, 
    "<voxel-size>"},
    {"remove-dupplicates",      0, NULL, OPTION_DOC, 
    "No arguments"},
    {0}
};

static char status_doc[] = 
"Defines a specific status operation to be applied to the point cloud.";
static struct argp_option status_options[] = {
    {"help",                    0, NULL, OPTION_DOC,
    "Give this help list"},
    {"aabb",                    0, NULL, OPTION_DOC, 
    "<output> <binary> <output-path>"},
    {0}
};

static struct argp process_argp = {process_options,
                                   NULL, 
                                   "[<ARG>...]", 
                                   process_doc};
static struct argp status_argp = {status_options,
                                  NULL, 
                                  "[<ARG>...]", 
                                  status_doc};

static int parse_func_opt(char *arg, 
                          struct argp *argp,
                          struct argp_state *state, 
                          func_t *funcs, 
                          size_t *size, 
                          int max_size, 
                          const func_info_t *func_info_list,
                          char *name)
{
    if (*size >= max_size) {
        argp_error(state, "Too many options specified (max: %d)", max_size);
        return ARGP_ERR_UNKNOWN;
    }
    if (strcmp(arg, "help") == 0) {
        argp_help(argp, 
                  stdout, 
                  ARGP_HELP_STD_HELP, 
                  name);
        exit(0);
    }

    const func_info_t *info = find_func(arg, func_info_list);
    if (!info) {
        argp_error(state, "Unknown option: %s", arg);
        return ARGP_ERR_UNKNOWN;
    }

    func_t *curr = &funcs[*size];
    curr->func_id = info->func;

    // Collect arguments
    curr->func_arg = malloc(info->max_args * sizeof(char*));
    curr->func_arg_size = 0;

    // Collect the required number of arguments
    for (int i = 0; i < info->min_args; i++) {
        arg = state->argv[state->next++];
        if (!arg) {
            argp_error(state, "Option %s requires %d arguments", 
                      info->name, info->min_args);
            return ARGP_ERR_UNKNOWN;
        }
        curr->func_arg[i] = strdup(arg);
        curr->func_arg_size++;
    }
}

static error_t parse_opt(int key, char *arg, struct argp_state *state) 
{
    struct arguments *args = state->input;
    
    switch (key) {
        case 'i':
            args->input = arg;
            break;
            
        case 'o':
            args->output = arg;
            break;
            
        case 'b':
            args->binary = atoi(arg);
            break;
            
        case 't':
            if (sscanf(arg, "%hhu,%hhu,%hhu", 
                      &args->tile.nx, 
                      &args->tile.ny, 
                      &args->tile.nz) != 3) {
                argp_error(state, "Invalid tile format. Use: nx,ny,nz");
                return ARGP_ERR_UNKNOWN;
            }
            args->flags |= SET_OPT_TILE;
            break;
            
        case 'p': 
        {
            parse_func_opt(arg,
                           &process_argp,
                           state,
                           args->procs, 
                           &args->procs_size, 
                           MAX_PROCESS, 
                           processes_g,
                           "--process");
            args->flags |= SET_OPT_PROCESS;
            args->procs_size++;
            break;
        }
            
        case 's': 
        {
            parse_func_opt(arg,
                           &status_argp, 
                           state, 
                           args->stats, 
                           &args->stats_size, 
                           MAX_STATUS, 
                           statuses_g,
                           "--status");
            args->flags |= SET_OPT_STATUS;
            args->stats_size++;
            break;
        }   
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, 0, doc};

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
