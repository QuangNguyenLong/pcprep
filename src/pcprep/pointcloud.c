#include "pcprep/pointcloud.h"
#include "pcprep/wrapper.h"
#include "pcprep/vec3f.h"
#include "pcprep/vec3uc.h"
#include "pcprep/core.h"
#include <time.h>
#include <stdio.h>

#define MSH_PLY_INCLUDE_LIBC_HEADERS
#define MSH_PLY_IMPLEMENTATION
#include <msh_ply.h>

int pointcloud_init(pointcloud_t *pc, size_t size)
{
    if (size < 0)
        return -1;
    pc->size = size;
    pc->pos = (float *)malloc(sizeof(float) * 3 * pc->size);
    pc->rgb = (uint8_t *)malloc(sizeof(uint8_t) * 3 * pc->size);
    return pc->size;
}
int pointcloud_free(pointcloud_t *pc)
{
    if (pc == NULL)
        return 1;
    if (pc->pos)
    {
        free(pc->pos);
        pc->pos = NULL;
    }
    if (pc->rgb)
    {
        free(pc->rgb);
        pc->rgb = NULL;
    }
    return 1;
}
int pointcloud_load(pointcloud_t *pc, const char *filename)
{
    pointcloud_init(pc, ply_count_element(filename));
    return ply_loader(filename, pc->pos, pc->rgb);
}
int pointcloud_write(pointcloud_t *pc, const char *filename, int binary)
{
    const char *vertex_attributes[] = {"x", "y", "z",
                                       "red", "green", "blue"};
    int32_t vertex_count = pc->size;
    if (vertex_count == 0)
        return -1;

    msh_ply_desc_t positions_desc = {
        (char *)"vertex",
        &vertex_attributes[0],
        3,
        MSH_PLY_FLOAT,
        MSH_PLY_INVALID,
        &pc->pos,
        NULL,
        &vertex_count,
        0};

    msh_ply_desc_t colors_desc = {
        (char *)"vertex",
        &vertex_attributes[3],
        3,
        MSH_PLY_UINT8,
        MSH_PLY_INVALID,
        &pc->rgb,
        NULL,
        &vertex_count,
        0};

    const char *write_format = binary ? "wb" : "w";
    msh_ply_t *pf = msh_ply_open(filename, write_format);
    if (pf)
    {
        msh_ply_add_descriptor(pf, &positions_desc);
        msh_ply_add_descriptor(pf, &colors_desc);
        msh_ply_write(pf);
    }
    msh_ply_close(pf);
    return vertex_count;
}

int pointcloud_min(pointcloud_t *pc, vec3f_t *min)
{
    if (!pc)
        return -1;
    vec3f_t *pos_lst = (vec3f_t *)(pc->pos);
    *min = pos_lst[0];
    for (int i = 0; i < pc->size; i++)
    {
        if (pos_lst[i].x < min->x)
            min->x = pos_lst[i].x;
        if (pos_lst[i].y < min->y)
            min->y = pos_lst[i].y;
        if (pos_lst[i].z < min->z)
            min->z = pos_lst[i].z;
    }
    return 0;
}
int pointcloud_max(pointcloud_t *pc, vec3f_t *max)
{
    if (!pc)
        return -1;
    vec3f_t *pos_lst = (vec3f_t *)(pc->pos);
    *max = pos_lst[0];
    for (int i = 0; i < pc->size; i++)
    {
        if (pos_lst[i].x > max->x)
            max->x = pos_lst[i].x;
        if (pos_lst[i].y > max->y)
            max->y = pos_lst[i].y;
        if (pos_lst[i].z > max->z)
            max->z = pos_lst[i].z;
    }
    return 0;
}

int get_tile_id(vec3f_t n, vec3f_t min, vec3f_t max, vec3f_t v)
{
    if (vec3f_g(v, max) || vec3f_l(v, min))
        return -1;
    vec3f_t ans;
    // ans = (v - min) / ((max - min) / n);
    // or ans = (v - min) * (invs(max - min) / n);
    // or ans = (v - min) * (invs(max - min) * invs(n));
    // or ans = (v - min) * invs(max - min) * n);
    ans = vec3f_mul(vec3f_mul(
                        vec3f_sub(v, min),
                        vec3f_inverse(vec3f_sub(max, min))),
                    n);
    ans = (vec3f_t){(int)(ans.x < n.x ? ans.x : n.x - 1),
                    (int)(ans.y < n.y ? ans.y : n.y - 1),
                    (int)(ans.z < n.z ? ans.z : n.z - 1)};
    for (int i = 0, a = 0, b = 0, c = 0; i < n.x * n.y * n.z; i++)
    {
        if (c == n.z)
        {
            c = 0;
            b++;
        }
        if (b == n.y)
        {
            b = 0;
            a++;
        }
        if (vec3f_eq(ans, vec3f_int((vec3f_t){a, b, c})))
            return i;
        c++;
    }
    return -1;
}

// TODO: this function is not safe
int pointcloud_tile(pointcloud_t *pc,
                    int n_x,
                    int n_y,
                    int n_z,
                    pointcloud_t **tiles)
{
    vec3f_t min, max;
    vec3f_t *pos_lst = (vec3f_t *)pc->pos;
    vec3f_t n = (vec3f_t){n_x, n_y, n_z};
    int size = n.x * n.y * n.z;

    int *numVerts = (int *)malloc(sizeof(int) * size);
    int *tmp = (int *)malloc(sizeof(int) * size);
    *tiles = (pointcloud_t *)malloc(sizeof(pointcloud_t) * size);

    pointcloud_min(pc, &min);
    pointcloud_max(pc, &max);

    for (int t = 0; t < size; t++)
    {
        numVerts[t] = 0;
        tmp[t] = 0;
    }
    for (int i = 0; i < pc->size; i++)
    {
        int t = get_tile_id(n, min, max, pos_lst[i]);
        numVerts[t]++;
    }

    for (int t = 0; t < size; t++)
    {
        pointcloud_init(&(*tiles)[t], numVerts[t]);
    }

    for (int i = 0; i < pc->size; i++)
    {
        int t = get_tile_id(n, min, max, pos_lst[i]);
        (*tiles)[t].pos[3 * tmp[t]] = pc->pos[3 * i];
        (*tiles)[t].pos[3 * tmp[t] + 1] = pc->pos[3 * i + 1];
        (*tiles)[t].pos[3 * tmp[t] + 2] = pc->pos[3 * i + 2];
        (*tiles)[t].rgb[3 * tmp[t]] = pc->rgb[3 * i];
        (*tiles)[t].rgb[3 * tmp[t] + 1] = pc->rgb[3 * i + 1];
        (*tiles)[t].rgb[3 * tmp[t] + 2] = pc->rgb[3 * i + 2];
        tmp[t]++;
    }

    free(numVerts);
    free(tmp);

    return size;
}

int pointcloud_sample(pointcloud_t *pc,
                      float ratio,
                      unsigned char strategy,
                      pointcloud_t **out)
{
    size_t num_points = (size_t)(pc->size * ratio);

    *out = (pointcloud_t *)malloc(sizeof(pointcloud_t));
    (*out)->pos = NULL;
    (*out)->rgb = NULL;
    (*out)->size = 0;

    pointcloud_init(*out, num_points);

    switch (strategy)
    {
    case PCP_SAMPLE_RULE_UNIFORM:
    {
        int *index_arr = (int *)malloc(sizeof(int) * pc->size);
        int *sample = (int *)malloc(num_points * sizeof(int));
        // Seed the random number generator
        srand((unsigned int)time(NULL));
        for (int i = 0; i < pc->size; i++)
            index_arr[i] = i;
        sample_union(index_arr, pc->size, sample, num_points);
        for (int i = 0; i < num_points; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                (*out)->pos[i * 3 + j] = pc->pos[sample[i] * 3 + j];
                (*out)->rgb[i * 3 + j] = pc->rgb[sample[i] * 3 + j];
            }
        }
        break;
    }
    default:
        break;
    }

    return 1;
}

void pointcloud_element_merge(pointcloud_t *pc, int left, int mid, int right)
{
    vec3f_t *arr_pos = (vec3f_t *)pc->pos;
    vec3uc_t *arr_col = (vec3uc_t *)pc->rgb;

    int n1 = mid - left + 1;
    int n2 = right - mid;

    vec3f_t *L_pos = (vec3f_t *)malloc(n1 * sizeof(vec3f_t));
    vec3f_t *R_pos = (vec3f_t *)malloc(n2 * sizeof(vec3f_t));

    vec3uc_t *L_col = (vec3uc_t *)malloc(n1 * sizeof(vec3uc_t));
    vec3uc_t *R_col = (vec3uc_t *)malloc(n2 * sizeof(vec3uc_t));

    for (int i = 0; i < n1; i++)
    {
        *(L_pos + i) = arr_pos[left + i];
        *(L_col + i) = arr_col[left + i];
    }
    for (int i = 0; i < n2; i++)
    {
        *(R_pos + i) = arr_pos[mid + 1 + i];
        *(R_col + i) = arr_col[mid + 1 + i];
    }
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2)
    {
        if (vec3f_leq(*(L_pos + i), *(R_pos + j)))
        {
            arr_pos[k] = *(L_pos + i);
            arr_col[k++] = *(L_col + i++);
        }
        else
        {
            arr_pos[k] = *(R_pos + j);
            arr_col[k++] = *(R_col + j++);
        }
    }
    while (i < n1)
    {
        arr_pos[k] = *(L_pos + i);
        arr_col[k++] = *(L_col + i++);
    }
    while (j < n2)
    {
        arr_pos[k] = *(R_pos + j);
        arr_col[k++] = *(R_col + j++);
    }

    free(L_pos);
    free(R_pos);
    free(L_col);
    free(R_col);
}
void pointcloud_element_merge_sort(pointcloud_t *pc, int left, int right)
{
    if (left >= right)
        return;
    int mid = left + (right - left) / 2;

    pointcloud_element_merge_sort(pc, left, mid);
    pointcloud_element_merge_sort(pc, mid + 1, right);

    pointcloud_element_merge(pc, left, mid, right);
}

int pointcloud_remove_dupplicates(pointcloud_t *pc,
                                  pointcloud_t **out)
{
    // use mergesort to sort points, then remove consecutives, O(Nlog(N))
    pointcloud_element_merge_sort(pc, 0, pc->size - 1);

    // count unique points to get output size
    size_t count_unique = 1;
    for (int i = 1; i < pc->size; i++)
    {
        vec3f_t pre = ((vec3f_t *)pc->pos)[i - 1];
        vec3f_t curr = ((vec3f_t *)pc->pos)[i];
        if (!vec3f_eq(pre, curr))
        {
            count_unique++;
        }
    }
    // init output pc, O(1)
    *out = (pointcloud_t *)malloc(sizeof(pointcloud_t));
    (*out)->pos = NULL;
    (*out)->rgb = NULL;
    (*out)->size = 0;
    pointcloud_init(*out, count_unique);

    // puts the unique points into the output, O(N)
    vec3f_t *uni_pos = (vec3f_t *)((*out)->pos);
    vec3uc_t *uni_col = (vec3uc_t *)((*out)->rgb);
    vec3f_t *pos = (vec3f_t *)(pc->pos);
    vec3uc_t *col = (vec3uc_t *)(pc->rgb);

    int index = 0;
    for (int i = 1; i < pc->size; i++)
    {
        if (!vec3f_eq(pos[i], pos[i - 1]))
        {
            uni_pos[index] = pos[i];
            uni_col[index++] = col[i]; // Start the color for the new point
        }
    }

    return (*out)->size;
}

int pointcloud_voxel(pointcloud_t *pc,
                     float voxel_size,
                     pointcloud_t **out)
{
    // quantize the points to the voxel grid
    // then remove the duplicates

    vec3f_t *pos = (vec3f_t *)pc->pos;
    for (int i = 0; i < pc->size; i++)
        pos[i] = vec3f_quantize(pos[i], voxel_size);
    pointcloud_remove_dupplicates(pc, out);
}