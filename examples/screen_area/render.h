#pragma once
#include <glm/glm.hpp>
#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>

bool intersectWithScreen(const glm::vec2 &p1, const glm::vec2 &p2, float screenBound, glm::vec2 &intersection, bool vertical);
void cropPoints(glm::vec2 &v1, glm::vec2 &v2, glm::vec2 &v3, float fbWidth, float fbHeight, glm::vec2 *clippedVertices, int &numClippedVertices);
float polygonArea(const glm::vec2 *vertices, int vertexCount);
float polygonArea(const glm::vec2 *vertices, int vertexCount);
float area(glm::vec2 &v1, glm::vec2 &v2, glm::vec2 &v3, int fbWidth, int fbHeight);

class Face
{
public:
    int *indices;
    float area;
    void create_data(int i, int j, int k);
    void delete_data();

public:
    glm::vec3 normal;
    Face(int v1 = 0, int v2 = 0, int v3 = 0) { create_data(v1, v2, v3); }
    ~Face() { delete_data(); }
};

class ConvexHull
{
public:
    glm::vec3 *vertices;
    Face *faces;

    glm::vec2 *newVertices;
    int nv;
    int nf;
    void allocate(int nvertices, int nfaces);
    void deallocate();
    void read(const char *path);

public:
    ConvexHull() {}
    ConvexHull(const char *path) { read(path); }
    void init(const char *path)
    {
        read(path);
    }
    ~ConvexHull() { deallocate(); }
    float Projection_Area(glm::mat4 &MVP, glm::vec3 cam_position, int fbWidth, int fbHeight);
};

// Function to compute the intersection of a line segment with the screen boundary
bool intersectWithScreen(const glm::vec2 &p1, const glm::vec2 &p2, float screenBound, glm::vec2 &intersection, bool vertical)
{
    if (vertical)
    {
        if (p1.x == p2.x)
            return false; // Avoid division by zero for vertical lines
        float t = (screenBound - p1.x) / (p2.x - p1.x);
        if (t >= 0 && t <= 1)
        {
            intersection = glm::vec2(screenBound, p1.y + t * (p2.y - p1.y));
            return true;
        }
    }
    else
    {
        if (p1.y == p2.y)
            return false; // Avoid division by zero for horizontal lines
        float t = (screenBound - p1.y) / (p2.y - p1.y);
        if (t >= 0 && t <= 1)
        {
            intersection = glm::vec2(p1.x + t * (p2.x - p1.x), screenBound);
            return true;
        }
    }
    return false;
}

// Crop points within screen bounds by calculating intersection points
void cropPoints(glm::vec2 &v1, glm::vec2 &v2, glm::vec2 &v3, float fbWidth, float fbHeight, glm::vec2 *clippedVertices, int &numClippedVertices)
{
    glm::vec2 vertices[3] = {v1, v2, v3};
    glm::vec2 newVertices[9];
    int newVertexCount = 0;

    for (int i = 0; i < 3; ++i)
    {
        glm::vec2 p1 = vertices[i];
        glm::vec2 p2 = vertices[(i + 1) % 3];

        if (p1.x >= 0 && p1.x <= fbWidth && p1.y >= 0 && p1.y <= fbHeight)
        {
            newVertices[newVertexCount++] = p1;
        }

        glm::vec2 intersection;
        if (intersectWithScreen(p1, p2, 0.0f, intersection, true) && intersection.y >= 0 && intersection.y <= fbHeight)
            newVertices[newVertexCount++] = intersection;
        if (intersectWithScreen(p1, p2, fbWidth, intersection, true) && intersection.y >= 0 && intersection.y <= fbHeight)
            newVertices[newVertexCount++] = intersection;
        if (intersectWithScreen(p1, p2, 0.0f, intersection, false) && intersection.x >= 0 && intersection.x <= fbWidth)
            newVertices[newVertexCount++] = intersection;
        if (intersectWithScreen(p1, p2, fbHeight, intersection, false) && intersection.x >= 0 && intersection.x <= fbWidth)
            newVertices[newVertexCount++] = intersection;
    }

    numClippedVertices = newVertexCount;
    for (int i = 0; i < newVertexCount; ++i)
    {
        clippedVertices[i] = newVertices[i];
    }
}

// Calculate the area of a polygon
float polygonArea(const glm::vec2 *vertices, int vertexCount)
{
    float area = 0.0f;
    for (int i = 0; i < vertexCount; ++i)
    {
        glm::vec2 v1 = vertices[i];
        glm::vec2 v2 = vertices[(i + 1) % vertexCount];
        area += (v1.x * v2.y - v2.x * v1.y);
    }
    return std::abs(area) * 0.5f;
}

float area(glm::vec2 &v1, glm::vec2 &v2, glm::vec2 &v3, int fbWidth, int fbHeight)
{
    glm::vec2 clippedVertices[9]; // Maximum possible vertices after clipping
    int numClippedVertices = 0;
    cropPoints(v1, v2, v3, float(fbWidth), float(fbHeight), clippedVertices, numClippedVertices);
    float area = polygonArea(clippedVertices, numClippedVertices);
    return area / (fbWidth * fbHeight);
}

void Face::create_data(int i, int j, int k)
{
    indices = new int[3];
    indices[0] = i;
    indices[1] = j;
    indices[2] = k;
    return;
}
void Face::delete_data()
{
    if (indices)
        delete[] indices;
}

void ConvexHull::allocate(int nvertices, int nfaces)
{
    vertices = new glm::vec3[nvertices];
    faces = new Face[nfaces];
    newVertices = new glm::vec2[nvertices];
}
void ConvexHull::deallocate()
{
    if (vertices)
        delete[] vertices;
    if (faces)
        delete[] faces;
    if (newVertices)
        delete[] newVertices;
}
void ConvexHull::read(const char *path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "ERROR: Failed to open file " << path << '\n';
        return;
    }
    std::string line;

    nv = 0, nf = 0;
    while (std::getline(file, line))
    {
        if (line[0] == 'v')
        { // check if line starts with v
            nv++;
        }
        else if (line[0] == 'f')
        { // check if line starts with f
            nf++;
        }
    }

    file.clear();
    file.seekg(0, std::ios::beg);

    // std::cout << nv << " " << nf << std::endl;
    allocate(nv, nf);

    int i = 0,
        j = 0;
    char temp;

    while (std::getline(file, line))
    {
        if (line[0] == 'v')
        { // check if line starts with v
            std::stringstream s(line);
            s >> temp >> vertices[i].x >> vertices[i].y >> vertices[i].z;
            i++;
        }
        else if (line[0] == 'f')
        { // check if line starts with f
            std::stringstream s(line);
            s >> temp >> faces[j].indices[0] >> faces[j].indices[1] >> faces[j].indices[2];
            j++;
        }
    }

    file.close();
    return;
}

float ConvexHull::Projection_Area(glm::mat4 &MVP, glm::vec3 cam_position, int fbWidth, int fbHeight)
{

    glm::vec4 temp0;
    glm::vec3 *temp = new glm::vec3[nv];
    for (int i = 0; i < nv; i++)
    {
        temp0 = MVP * glm::vec4(vertices[i], 1.0); // clip space
        temp[i] = glm::vec3(temp0) / temp0.w;
        // NDC space
        newVertices[i] = glm::vec2(float(0.5 * (temp[i].x + 1.0) * fbWidth), float(0.5 * (1.0 - temp[i].y) * fbHeight)); // screen space
    }

    float ans = 0;
    for (int j = 0; j < nf; j++)
    {
        // Indices of the vertices for the current face
        int idx0 = faces[j].indices[0] - 1;
        int idx1 = faces[j].indices[1] - 1;
        int idx2 = faces[j].indices[2] - 1;

        // Check if the vertices are within the screen bounds
        if ((newVertices[idx0].x >= 0 && newVertices[idx0].x <= fbWidth && newVertices[idx0].y >= 0 && newVertices[idx0].y <= fbHeight) &&
            (newVertices[idx1].x >= 0 && newVertices[idx1].x <= fbWidth && newVertices[idx1].y >= 0 && newVertices[idx1].y <= fbHeight) &&
            (newVertices[idx2].x >= 0 && newVertices[idx2].x <= fbWidth && newVertices[idx2].y >= 0 && newVertices[idx2].y <= fbHeight))
        {
            ans += area(newVertices[idx0], newVertices[idx1], newVertices[idx2], fbWidth, fbHeight);
        }
    }
    return ans; // > 1.0f ? 1.0f : ans;
}