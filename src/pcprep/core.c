#include <pcprep/core.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
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
char* read_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file: %s\n", filename);
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate buffer
    char *buffer = (char *)malloc(length + 1);
    if (buffer == NULL) {
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

