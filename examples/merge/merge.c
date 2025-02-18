#include <pcprep/pointcloud.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if(argc < 5){
        printf("Usage: %s <input_pointcloud_1> <input_pointcloud_2> <input_pointcloud_3> <output_pointcloud>\n", argv[0]);
    }

    pointcloud_t pc1, pc2, pc3, out;
    pointcloud_load(&pc1, argv[1]);
    pointcloud_load(&pc2, argv[2]);
    pointcloud_load(&pc3, argv[3]);
    pointcloud_t pcs[3] = {pc1, pc2, pc3};

    pointcloud_merge(&pcs[0], 3, &out);

    pointcloud_write(out, argv[4], 1);
}