#ifndef CORE_H
#define CORE_H

#define PCP_SAMPLE_RULE_UNIFORM 0x00
#include <stdlib.h>

typedef struct {
    float x, y;
} vec2f_t;

// Function to get the current time in milliseconds
long long get_current_time_ms(); 

int sample_union(int *input, 
                 int input_size, 
                 int *output, 
                 int output_size);

float quantize(float x, float q);
char* read_file(const char *filename);

// `mvps` should be parse as an array of float mvp[4][4]
int json_parse_cam_matrix(char *filepath, 
                          float *mvps, 
                          size_t view_count, 
                          size_t *width, 
                          size_t *height);


void json_write_to_file(const char* filename, void* json);

int json_write_tiles_pixel(char *outpath,
                           int num_tile,
                           int num_view,
                           int **pixel_count_per_tile,
                           size_t total_pixel);

int json_write_screen_area_estimation(char *outpath,
                                      int num_view,
                                      size_t width,
                                      size_t height,
                                      float *screen_ratio);

float clipped_triangle_area(vec2f_t p1, vec2f_t p2, vec2f_t p3);

#endif