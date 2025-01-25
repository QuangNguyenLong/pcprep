#include <pcprep/sample.h>
#include <time.h>
#include <stdlib.h>

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

    srand((unsigned int)time(NULL)); // Seed the random number generator

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
