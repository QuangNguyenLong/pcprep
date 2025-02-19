#include <pcprep/graphic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WITH_GRAPHIC

const char *vertexShaderCode = 
    "#version 330 core\n"
    "layout(location = 0) in vec3 vertexPosition_modelspace;\n"
    "layout(location = 1) in vec3 vertexCorlor;\n"
    "uniform mat4 MVP;\n"
    "out vec3 frag_color;\n"
    "void main(){\n"
    "    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);\n"
    "    gl_PointSize = 1.0;\n"
    "    frag_color = vertexCorlor;\n"
    "}";

const char *fragmentShaderCode = 
    "#version 330 core\n"
    "in vec3 frag_color;\n"
    "out vec3 color;\n"
    "void main(){\n"
    "    color = frag_color;\n"
    "}";


int flip_image(unsigned char **row_pointers, 
               unsigned char *pixels, 
               int width, 
               int height) 
{
    for (int y = 0; y < height; y++) {
        memcpy(row_pointers[y], &pixels[y * width * 3], width * 3);
    }
    for (int i = 0; i < height / 2; ++i) {
        unsigned char *temp = row_pointers[i];
        row_pointers[i] = row_pointers[height - 1 - i];
        row_pointers[height - 1 - i] = temp;
    }
}

GLuint load_shader(char *vertex_file_path, 
                   char *fragment_file_path)
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    char *VertexShaderCode = NULL;
    char *FragmentShaderCode = NULL;

    if(vertex_file_path == NULL)
    {
        VertexShaderCode = (char *)vertexShaderCode;
    }
    else
    {
        // Read the Vertex Shader code from the file
        FILE *VertexShaderStream = fopen(vertex_file_path, "r");
        if (VertexShaderStream != NULL) {
            fseek(VertexShaderStream, 0, SEEK_END);
            long length = ftell(VertexShaderStream);
            fseek(VertexShaderStream, 0, SEEK_SET);
            VertexShaderCode = (char *)malloc(length + 1);
            fread(VertexShaderCode, 1, length, VertexShaderStream);
            VertexShaderCode[length] = '\0';
            fclose(VertexShaderStream);
        } else {
            fprintf(stderr, "Impossible to open %s.\n", vertex_file_path);
            getchar();
            return 0;
        }
    }
    if(fragment_file_path == NULL)
    {
        FragmentShaderCode = (char *)fragmentShaderCode;
    }
    else 
    {
        // Read the Fragment Shader code from the file
        FILE *FragmentShaderStream = fopen(fragment_file_path, "r");
        if (FragmentShaderStream != NULL) {
            fseek(FragmentShaderStream, 0, SEEK_END);
            long length = ftell(FragmentShaderStream);
            fseek(FragmentShaderStream, 0, SEEK_SET);
            FragmentShaderCode = (char *)malloc(length + 1);
            fread(FragmentShaderCode, 1, length, FragmentShaderStream);
            FragmentShaderCode[length] = '\0';
            fclose(FragmentShaderStream);
        }
    }
    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    const char *VertexSourcePointer = VertexShaderCode;
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        char *VertexShaderErrorMessage = (char *)malloc(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, VertexShaderErrorMessage);
        fprintf(stderr, "%s\n", VertexShaderErrorMessage);
        free(VertexShaderErrorMessage);
    }

    // Compile Fragment Shader
    const char *FragmentSourcePointer = FragmentShaderCode;
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        char *FragmentShaderErrorMessage = (char *)malloc(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, FragmentShaderErrorMessage);
        fprintf(stderr, "%s\n", FragmentShaderErrorMessage);
        free(FragmentShaderErrorMessage);
    }

    // Link the program
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        char *ProgramErrorMessage = (char *)malloc(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, ProgramErrorMessage);
        fprintf(stderr, "%s\n", ProgramErrorMessage);
        free(ProgramErrorMessage);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    if(vertex_file_path != NULL)
        free(VertexShaderCode);
    if(fragment_file_path != NULL)
        free(FragmentShaderCode);

    return ProgramID;
}


int pointcloud_get_viewport(pointcloud_t pc,
                            float *mvp,
                            int width,
                            int height,
                            char *vertex_shader,
                            char *fragment_shader,
                            vec3f_t background_color,
                            unsigned char *pixels)
{
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(1, 1, "Hidden", NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    glClearColor(background_color.x / 255.0f, background_color.y / 255.0f, background_color.z / 255.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_PROGRAM_POINT_SIZE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint programID = load_shader(vertex_shader, fragment_shader);
    glUseProgram(programID);
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, mvp);

    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    GLuint renderedTexture;
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);

    GLuint depthbuffer;
    glGenRenderbuffers(1, &depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "Framebuffer is not complete!\n");
        return -1;
    }

    GLuint vbuffer, rbuffer;
    glGenBuffers(1, &vbuffer);
    glGenBuffers(1, &rbuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glBufferData(GL_ARRAY_BUFFER, pc.size * 3 * sizeof(float), pc.pos, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, rbuffer);
    glBufferData(GL_ARRAY_BUFFER, pc.size * 3 * sizeof(unsigned char), pc.rgb, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, rbuffer);
    glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void *)0);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawArrays(GL_POINTS, 0, pc.size);
    printf("Drawed %lu points\n", pc.size);

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &renderedTexture);
    glDeleteRenderbuffers(1, &depthbuffer);
    glDeleteBuffers(1, &vbuffer);
    glDeleteBuffers(1, &rbuffer);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);

    glfwTerminate();

    return 1;
}


// int pointcloud_get_viewport(pointcloud_t pc,
//                             float *mvp,
//                             int width,
//                             int height,
//                             char *vertex_shader,
//                             char *fragment_shader,
//                             vec3f_t background_color,
//                             unsigned char *pixels)
// {
//     // Create an OSMesa context
//     OSMesaContext ctx = OSMesaCreateContext(GL_RGBA, NULL);
//     if (!ctx)
//     {
//         fprintf(stderr, "Failed to create OSMesa context\n");
//         return -1;
//     }

//     // Allocate buffer for rendering (RGBA)
//     unsigned char *buffer = (unsigned char *)malloc(width * height * 4);
//     if (!buffer)
//     {
//         fprintf(stderr, "Failed to allocate buffer\n");
//         OSMesaDestroyContext(ctx);
//         return -1;
//     }

//     // Bind buffer to OSMesa context
//     if (!OSMesaMakeCurrent(ctx, buffer, GL_UNSIGNED_BYTE, width, height))
//     {
//         fprintf(stderr, "OSMesaMakeCurrent failed\n");
//         free(buffer);
//         OSMesaDestroyContext(ctx);
//         return -1;
//     }

//     // Set up OpenGL
//     glViewport(0, 0, width, height);
//     glClearColor(background_color.x / 255.0f, background_color.y / 255.0f, background_color.z / 255.0f, 1.0f);
//     glEnable(GL_DEPTH_TEST);
//     glDepthFunc(GL_LESS);
//     glEnable(GL_PROGRAM_POINT_SIZE);

//     // Load shaders
//     GLuint programID = load_shader(vertex_shader, fragment_shader);
//     glUseProgram(programID);
//     GLuint MatrixID = glGetUniformLocation(programID, "MVP");
//     glUniformMatrix4fv(MatrixID, 1, GL_FALSE, mvp);

//     // Generate buffers for point cloud
//     GLuint vbuffer, rbuffer;
//     glGenBuffers(1, &vbuffer);
//     glGenBuffers(1, &rbuffer);

//     // Upload vertex data (positions)
//     glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
//     glBufferData(GL_ARRAY_BUFFER, pc.size * 3 * sizeof(float), pc.pos, GL_STATIC_DRAW);
//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

//     // Upload color data
//     glBindBuffer(GL_ARRAY_BUFFER, rbuffer);
//     glBufferData(GL_ARRAY_BUFFER, pc.size * 3 * sizeof(unsigned char), pc.rgb, GL_STATIC_DRAW);
//     glEnableVertexAttribArray(1);
//     glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void *)0);

//     // Render to the OSMesa buffer
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//     glDrawArrays(GL_POINTS, 0, pc.size);
//     printf("Drawed %lu points\n", pc.size);

//     // Copy rendered pixels to output buffer
//     memcpy(pixels, buffer, width * height * 3); // Copy only RGB channels

//     // Cleanup
//     glDeleteBuffers(1, &vbuffer);
//     glDeleteBuffers(1, &rbuffer);
//     glDeleteProgram(programID);
//     OSMesaDestroyContext(ctx);
//     free(buffer);

//     return 0;
// }

int save_viewport(unsigned char **row_pointers, 
                  int width, 
                  int height, 
                  const char *filename)
{

    // Open the PNG file for writing
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        fprintf(stderr, "Error: could not open PNG file for writing\n");
        exit(1);
    }

    // Initialize the PNG writer
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fprintf(stderr, "Error: could not initialize PNG write structure\n");
        exit(1);
    }

    // Initialize the PNG info structure
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        fprintf(stderr, "Error: could not initialize PNG info structure\n");
        exit(1);
    }

    // Set the error handling for the PNG writer
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        fprintf(stderr, "Error: PNG write failed\n");
        exit(1);
    }

    // Set the output file handle
    png_init_io(png_ptr, fp);

    // Set the image dimensions and format
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // Write the PNG header and image data
    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, row_pointers);
    png_write_end(png_ptr, NULL);

    // Clean up and close the file
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}

#endif
