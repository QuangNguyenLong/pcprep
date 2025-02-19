#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <pcprep/pointcloud.h>
#include <pcprep/vec3f.h>

#if defined(WITH_GLEW) && defined(WITH_GL) && defined(WITH_PNG) && defined(WITH_GLFW)
#define WITH_GRAPHIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <png.h>

// #if defined(WITH_OSMESA) && defined(WITH_GL) && defined(WITH_PNG)
// #define WITH_GRAPHIC

// #include <GL/osmesa.h>
// #include <GL/gl.h>
// #include <png.h>

int flip_image(unsigned char **row_pointers, 
               unsigned char *pixels, 
               int width, 
               int height);

GLuint load_shader(char *vertex_file_path, 
                   char *fragment_file_path);

// I guess there should be a pointcloud_render too
// `pixels` should be passed as a reference to an array of unsigned char
int pointcloud_get_viewport(pointcloud_t pc,
                            float *mvp,
                            int width,
                            int height,
                            char *vertex_shader,
                            char *fragment_shader,
                            vec3f_t background_color,
                            unsigned char *pixels);

int save_viewport(unsigned char **row_pointers, 
                  int width, 
                  int height, 
                  const char* filename);
#endif
#endif