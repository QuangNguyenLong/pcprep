#include <argp.h>
#include <stdlib.h>
#include <stdint.h>
#include <pcprep/pointcloud.h>

struct arguments 
{
    char *input;
    char *output;
    int binary;
    struct
    {
        // uint8_t  
        uint8_t nx, ny, nz;
    } tile;
    float psample;
    size_t nsample;
};


int pcprep_prepare(struct arguments *arg)
{
    pointcloud_t        input           = {NULL, NULL, 0};
    pointcloud_t        output          = {NULL, NULL, 0};
    char                *input_path     = NULL;
    char                *output_path    = NULL;

    input_path      = arg->input;
    output_path     = arg->output;


    // pointcloud_load(&input, )
}

const char *argp_program_version = "pcprep 0.1";
const char *argp_program_bug_address = "<quang.nglong@gmail.com>";

static char doc[] = "A program that prepare point cloud.";
static char args_doc[] = "-i input -o output";

static struct argp_option options[] = {
    {"input",           'i',        "FILE",         0, 
    "Input file"},
    {"output",          'o',        "FILE",         0, 
    "Output file(s)"},
    {"binary",          'b',        "0|1",          0, 
    "output binary (default is 1)."},
    {"tile",            'T',        "nx,ny,nz",     0, 
    "set number of division per axis"},
    {"sample-rule",     'S',        "0|1|2",        0,
    "set sample rule (default is 0 for UNIFORM)"},
    {"psample",         'P',        "REAL-VALUE",   0, 
    "randomly sample points using rule specified by --sample-rule in the point cloud with fixed proportion F (0.0 to 1.0) of the original point cloud size"},
    {"nsample",         'N',        "INT-VALUE",    0,
    "same as psample but sample N points instead"},
    {0}
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *args = state->input;

    switch (key) 
    {
        case 'i': args->input = arg; break;
        case 'o': args->output = arg; break;
        case 'b': args->binary = atoi(arg); break;
        case 'T':
                  if(sscanf(arg, "%hhu,%hhu,%hhu", 
                              &(args->tile.nx), 
                              &(args->tile.ny), 
                              &(args->tile.nz)) != 3)
                  {
                      argp_state_help(state, stderr, ARGP_HELP_USAGE);
                      return ARGP_ERR_UNKNOWN;
                  }
                  break;
        case 'P': args->psample = atof(arg); break;
        case 'N': args->nsample = atoi(arg); break;
        default: return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char *argv[]) {
    struct arguments args = {NULL, NULL, 1};

    argp_parse(&argp, argc, argv, 0, 0, &args);

    printf("input: %s\n", args.input);
    printf("output: %s\n", args.output);
    printf("binary: %d\n", args.binary);
    printf("tiles: %dx%dx%d\n", args.tile.nx, args.tile.ny, args.tile.nz);
    printf("psample: %f\n", args.psample);
    printf("nsample: %lu\n", args.nsample);



    return 0;
}
