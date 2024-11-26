#include <argp.h>
#include <stdlib.h>
const char *argp_program_version = "program 1.0";
const char *argp_program_bug_address = "<bug-report@program.com>";

static char doc[] = "A program that handles many arguments easily.";
static char args_doc[] = "-i input -o output -t threads";

static struct argp_option options[] = {
    {"input", 'i', "FILE", 0, "Input file"},
    {"output", 'o', "FILE", 0, "Output file"},
    {"threads", 't', "NUM", 0, "Number of threads"},
    {0}
};

struct arguments {
    char *input;
    char *output;
    int threads;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *args = state->input;

    switch (key) {
        case 'i': args->input = arg; break;
        case 'o': args->output = arg; break;
        case 't': args->threads = atoi(arg); break;
        default: return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char *argv[]) {
    struct arguments args = {NULL, NULL, 0};

    argp_parse(&argp, argc, argv, 0, 0, &args);

    printf("Input: %s\n", args.input);
    printf("Output: %s\n", args.output);
    printf("Threads: %d\n", args.threads);

    return 0;
}