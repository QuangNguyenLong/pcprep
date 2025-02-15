#ifndef CORE_H
#define CORE_H

#define PCP_SAMPLE_RULE_UNIFORM 0x00

int sample_union(int *input, 
                 int input_size, 
                 int *output, 
                 int output_size);

float quantize(float x, float q);

#endif