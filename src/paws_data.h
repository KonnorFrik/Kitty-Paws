/** @file
 * @brief Data (types, struct, etc) for the app is declared here */

#ifndef __KITTY_PAWS_DATA_H__
#define __KITTY_PAWS_DATA_H__

#include <stddef.h>
#include "cvector/cvector.h"

/**
 * @brief Hold indeces for vertex, texture, normal 
 */
typedef struct _paws_face {
    size_t vertex_index, texture_index, normal_index;
} paws_face_indices;

/**
 * @brief Hold mesh loaded from file
 */
typedef struct _paws_mesh {
    cvector* vertices; ///< Contains pointers to Vector3 
    cvector* textures; ///< Contains pointers to Vector2
    cvector* normals;  ///< Contains pointers to Vector3
    cvector* faces;    ///< Contains pointers to cvector with pointers to paws_face_indeces
} paws_mesh;

bool parse_format_obj(const char* filepath, paws_mesh* mesh);

#endif /* __KITTY_PAWS_DATA_H__ */
