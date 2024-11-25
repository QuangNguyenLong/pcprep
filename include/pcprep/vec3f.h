#ifndef VEC3F_H
#define VEC3F_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <math.h>
typedef struct vec3f_t
{
    float x;
    float y;
    float z;
} vec3f_t;

static inline vec3f_t vec3f_set(float x, float y, float z)
{
    vec3f_t v = {x, y, z};
    return v;
}

static inline vec3f_t vec3f_int(vec3f_t v)
{
    return (vec3f_t){(float)(int)v.x, (float)(int)v.y, (float)(int)v.z};
}

static inline vec3f_t vec3f_mul_scalar(vec3f_t v, float scalar)
{
    return (vec3f_t){v.x * scalar, v.y * scalar, v.z * scalar};
}

static inline vec3f_t vec3f_inverse(vec3f_t v)
{
    return (vec3f_t){1.0f / v.x, 1.0f / v.y, 1.0f / v.z};
}

static inline float vec3f_magnitude(vec3f_t v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

static inline vec3f_t vec3f_normalize(vec3f_t v)
{
    float mag = vec3f_magnitude(v);
    if (mag > 0.0f)
    {
        return vec3f_mul_scalar(v, 1.0f / mag);
    }
    return vec3f_set(0.0f, 0.0f, 0.0f);
}

static inline vec3f_t vec3f_add(vec3f_t a, vec3f_t b)
{
    return (vec3f_t){a.x + b.x, a.y + b.y, a.z + b.z};
}

static inline vec3f_t vec3f_sub(vec3f_t a, vec3f_t b)
{
    return (vec3f_t){a.x - b.x, a.y - b.y, a.z - b.z};
}

static inline vec3f_t vec3f_mul(vec3f_t a, vec3f_t b)
{
    return (vec3f_t){a.x * b.x, a.y * b.y, a.z * b.z};
}

static inline float vec3f_dot(vec3f_t a, vec3f_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
static inline vec3f_t vec3f_cross(vec3f_t a, vec3f_t b)
{
    return (vec3f_t){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

static inline float vec3f_angle_between(vec3f_t a, vec3f_t b)
{
    float dot = vec3f_dot(a, b);
    float magA = vec3f_magnitude(a);
    float magB = vec3f_magnitude(b);
    return acos(dot / (magA * magB));
}
static inline int vec3f_eq(vec3f_t a, vec3f_t b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

static inline int vec3f_g(vec3f_t a, vec3f_t b)
{
    return a.x > b.x && a.y > b.y && a.z > b.z;
}

static inline int vec3f_l(vec3f_t a, vec3f_t b)
{
    return a.x < b.x && a.y < b.y && a.z < b.z;
}

static inline int vec3f_geq(vec3f_t a, vec3f_t b)
{
    return vec3f_g(a, b) || vec3f_eq(a, b);
}

static inline int vec3f_leq(vec3f_t a, vec3f_t b)
{
    return vec3f_l(a, b) || vec3f_eq(a, b);
}

static inline vec3f_t vec3f_reflect(vec3f_t v, vec3f_t n)
{
    float dot = vec3f_dot(v, n);
    return vec3f_sub(v, vec3f_mul_scalar(n, 2 * dot));
}

#ifdef __cplusplus
}
#endif

#endif
