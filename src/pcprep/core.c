#include <pcprep/core.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <cJSON.h>
int sample_union(int *input,
                 int input_size,
                 int *output,
                 int output_size)
{
    if (output_size > input_size)
    {
        return -1;
    }

    // Create an array to keep track of selected indices
    int *selected = calloc(input_size, sizeof(int));
    if (!selected)
    {
        return -1;
    }
    // Seed the random number generator
    srand((unsigned int)time(NULL));

    int selected_count = 0;
    while (selected_count < output_size)
    {
        int index = rand() % input_size;

        // Ensure no duplicates
        if (!selected[index])
        {
            selected[index] = 1;
            output[selected_count++] = input[index];
        }
    }

    free(selected);
}
float quantize(float x, float q)
{
    return q * floor(x / q + 0.5f);
}
char *read_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not open file: %s\n", filename);
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate buffer
    char *buffer = (char *)malloc(length + 1);
    if (buffer == NULL)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(file);
        return NULL;
    }

    // Read file into buffer
    size_t read_length = fread(buffer, 1, length, file);
    buffer[read_length] = '\0'; // Null-terminate the string

    fclose(file);
    return buffer;
}

void json_write_to_file(const char *filename, void *json)
{
    cJSON *cjson = (cJSON *)json;
    char *json_string = cJSON_Print(cjson);
    FILE *file = fopen(filename, "w");
    if (file)
    {
        fprintf(file, "%s", json_string);
        fclose(file);
    }
    else
    {
        printf("Error writing to file: %s\n", filename);
    }
    free(json_string);
}

int json_parse_cam_matrix(char *filepath, float *mvps, size_t mvps_size, size_t *width, size_t *height)
{
    char *json_buff = read_file(filepath);
    cJSON *json = cJSON_Parse(json_buff);
    cJSON *camera = cJSON_GetObjectItem(json, "camera");
    cJSON *screen = cJSON_GetObjectItem(camera, "screen");
    *width = (size_t)cJSON_GetObjectItem(screen, "width")->valueint;
    *height = (size_t)cJSON_GetObjectItem(screen, "height")->valueint;
    cJSON *mvp_array = cJSON_GetObjectItem(camera, "mvp");
    int actual_mvp_count = cJSON_GetArraySize(mvp_array);
    // if the allocated is smaller than the actual count, then the allocated is used.
    // else if it is larger, the actual count is used, the parsed mvp count is returned.
    if (mvps_size < actual_mvp_count)
        actual_mvp_count = mvps_size;

    for (int f = 0; f < actual_mvp_count; f++)
    {
        cJSON *matrix = cJSON_GetArrayItem(mvp_array, f);
        for (int i = 0; i < 4; i++)
        {
            cJSON *row = cJSON_GetArrayItem(matrix, i);
            for (int j = 0; j < 4; j++)
            {
                mvps[f * 4 * 4 + i * 4 + j] = (float)cJSON_GetArrayItem(row, j)->valuedouble;
            }
        }
    }
    return actual_mvp_count;
}
int json_write_tiles_pixel(char *outpath,
                           int num_tile,
                           int num_view,
                           int **pixel_count_per_tile,
                           size_t total_pixel)
{
    cJSON *view = cJSON_CreateObject();
    cJSON *viewArray = cJSON_CreateArray();

    for (int v = 0; v < num_view; v++)
    {
        cJSON *view_item = cJSON_CreateObject();
        cJSON_AddNumberToObject(view_item, "id", v);
        cJSON *tile_visi_array = cJSON_CreateArray();
        for (int t = 0; t < num_tile; t++)
        {
            cJSON *tile_item = cJSON_CreateObject();
            cJSON_AddNumberToObject(tile_item, "id", t);
            cJSON_AddNumberToObject(tile_item, "pixel-count", pixel_count_per_tile[v][t]);
            cJSON_AddNumberToObject(tile_item,
                                    "screen-ratio",
                                    pixel_count_per_tile[v][t] * 1.0f / total_pixel);
            cJSON_AddItemToArray(tile_visi_array, tile_item);
        }
        cJSON_AddItemToObject(view_item, "tile-visibility", tile_visi_array);
        cJSON_AddItemToArray(viewArray, view_item);

    }
    cJSON_AddItemToObject(view, "view", viewArray);
    json_write_to_file(outpath, view);
}
