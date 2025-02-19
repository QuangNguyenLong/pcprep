#include <pcprep/pointcloud.h>
#include <glm/gtc/matrix_transform.hpp>
#include "render.h"

ConvexHull createBoundingBoxHull(const vec3f_t min, const vec3f_t max)
{
    ConvexHull hull;
    hull.nv = 8;
    hull.nf = 12;
    hull.allocate(hull.nv, hull.nf);
    // Define the 8 vertices of the bounding box

    hull.vertices[0] = glm::vec3(min.x, min.y, min.z);
    hull.vertices[1] = glm::vec3(min.x, min.y, max.z);
    hull.vertices[2] = glm::vec3(min.x, max.y, min.z);
    hull.vertices[3] = glm::vec3(min.x, max.y, max.z);
    hull.vertices[4] = glm::vec3(max.x, min.y, min.z);
    hull.vertices[5] = glm::vec3(max.x, min.y, max.z);
    hull.vertices[6] = glm::vec3(max.x, max.y, min.z);
    hull.vertices[7] = glm::vec3(max.x, max.y, max.z);

    // Define the indices for each triangular face
    int triangleIndices[12][3] = {
        {1, 2, 4},
        {1, 4, 3},
        {5, 8, 6},
        {5, 7, 8},
        {1, 7, 5},
        {1, 3, 7},
        {2, 6, 8},
        {2, 8, 4},
        {1, 6, 2},
        {1, 5, 6},
        {3, 4, 8},
        {3, 8, 7}};

    // Allocate and assign indices for each face
    for (int i = 0; i < 12; ++i)
    {
        hull.faces[i].indices = (int *)malloc(3 * sizeof(int));
        for (int j = 0; j < 3; ++j)
        {
            hull.faces[i].indices[j] = triangleIndices[i][j];
        }
    }

    return hull;
}

void writeConvexHullToObj(const ConvexHull *hull, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("Failed to open file");
        return;
    }

    // Write vertices
    fprintf(file, "# Vertices\n");
    for (int i = 0; i < 8; ++i)
    {
        fprintf(file, "v %f %f %f\n", hull->vertices[i].x, hull->vertices[i].y, hull->vertices[i].z);
    }

    // Write faces
    fprintf(file, "\n# Faces\n");
    for (int i = 0; i < 12; ++i) // 12 triangular faces
    {
        fprintf(file, "f %d %d %d\n",
                hull->faces[i].indices[0], // OBJ format uses 1-based indexing
                hull->faces[i].indices[1],
                hull->faces[i].indices[2]);
    }

    fclose(file);
    // printf("ConvexHull written to '%s'\n", filename);
}

// M: assume 1.
// V: lookAt point, cam position, up vector
int main()
{
    // cam_pos_x, cam_pos_y, cam_pos_z, look_at_x, look_at_y, look_at_z, up_vector_x, up_vector_y, up_vector_z, FoVy, aspect_ratio  
    glm::vec3 cam_position(1000, 1000, 1000);
    glm::vec3 look_at_point(250, 250, 250);
    glm::vec3 up_vector(0, 1, 0);

    glm::mat4 ModelMatrix = glm::mat4(1.0f);
    glm::mat4 ViewMatrix = glm::lookAt(cam_position, look_at_point, up_vector);
    glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 1.0f, 10000.0f);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

    const char tile_path[][1000] = {"test/tile0000.ply",
                                    "test/tile0001.ply",
                                    "test/tile0002.ply",
                                    "test/tile0003.ply",
                                    "test/tile0004.ply",
                                    "test/tile0005.ply",
                                    "test/tile0006.ply",
                                    "test/tile0007.ply"};
    const char out_bbox_path[][1000] = {"test/tile0000.obj",
                                        "test/tile0001.obj",
                                        "test/tile0002.obj",
                                        "test/tile0003.obj",
                                        "test/tile0004.obj",
                                        "test/tile0005.obj",
                                        "test/tile0006.obj",
                                        "test/tile0007.obj"};
    pointcloud_t tile[8];

    for (int i = 0; i < 8; i++)
    {
        pointcloud_load(&tile[i], tile_path[i]);
        vec3f_t max, min;
        pointcloud_max(tile[i], &max);
        pointcloud_min(tile[i], &min);
        ConvexHull bbox = createBoundingBoxHull(min, max);
        writeConvexHullToObj(&bbox, out_bbox_path[i]);
        float screen_area = bbox.Projection_Area(MVP, cam_position, 1, 1);
        printf("%f \n", screen_area);
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                printf("%.04f ", MVP[i][j]);
            }
            printf("\n");
        }
    }
}