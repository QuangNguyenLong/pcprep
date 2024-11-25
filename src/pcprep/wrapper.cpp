#include "miniply/miniply.h"
#include "pcprep/wrapper.h"
#include <cstring>

extern bool p_ply_loader(const char *filename, float *pos, unsigned char *rgb)
{
    miniply::PLYReader reader(filename);
    if (!reader.valid())
    {
        return 0;
    }

    uint32_t propIdxs[3];
    bool gotVerts = false;

    while (reader.has_element() && !gotVerts)
    {
        if (reader.element_is(miniply::kPLYVertexElement))
        {
            if (!reader.load_element() || !reader.find_pos(propIdxs))
            {
                break;
            }

            reader.extract_properties(propIdxs, 3, miniply::PLYPropertyType::Float, pos);
            if (reader.find_color(propIdxs))
            {
                reader.extract_properties(propIdxs, 3, miniply::PLYPropertyType::UChar, rgb);
            }
            gotVerts = true;
        }
        reader.next_element();
    }

    return 1;
}

extern "C"
{
    int ply_count_element(const char *filename)
    {
        miniply::PLYReader reader(filename);
        if (!reader.valid())
        {
            return -1;
        }
        uint32_t elemIndex = reader.find_element("vertex");
        if (elemIndex == miniply::kInvalidIndex)
            return 0;
        return reader.get_element(elemIndex)->count;
    }
    // Wrapper implementation
    int ply_loader(const char *filename, float *pos, unsigned char *rgb)
    {
        // char filename_copy[1024];
        // strncpy(filename_copy, filename, sizeof(filename_copy) - 1);
        // filename_copy[sizeof(filename_copy) - 1] = '\0';

        return p_ply_loader(filename, pos, rgb) ? 1 : 0;
    }
}
