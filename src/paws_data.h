/** @file
 * @brief Data (types, struct, etc) for the app is declared here */

#ifndef __KITTY_PAWS_DATA_H__
#define __KITTY_PAWS_DATA_H__

#include <stddef.h>
#include <raylib.h>
#include "cvector/cvector.h"

/**
 * @brief Hold indeces for vertex, texture, normal 
 * @note 'long long' type for handle indices less than zero
 */
typedef struct _paws_face {
    long long vertex_index, texture_index, normal_index;
} paws_face_indices;

/**
 * @brief Hold mesh loaded from file
 * @version 0.3.0
 */
typedef struct _paws_mesh {
    cvector* vertices;     ///< Contains pointers to Vector3 
    cvector* textures;     ///< Contains pointers to Vector2
    cvector* normals;      ///< Contains pointers to Vector3
    cvector* faces;        ///< Contains pointers to cvector with pointers to paws_face_indeces
    bool is_loaded;        ///< Is mesh loaded from file and ready or not
    bool is_draw_normals;  ///< Is need to draw normals
    bool is_draw_textures; ///< Is need to draw textures
    bool is_draw_edges;    ///< Is need to draw edges

    struct settings {
        enum point_type { ///< How to draw a point
            NONE, SPHERE, CUBE, 
        } point_type;
        float point_radius; ///< How big will be a point
        Color color_point, color_edge, color_normal;
    } settings;
} paws_mesh;
// TODO: add settings to mesh or object struct
// - [ ]render-type - wireframe, model, texture, ...

// TODO: create struct with filepath, mesh, something more to hold object
// - Also write functions for manage this struct (load, save, unload, etc)

// Parser related functions
bool parse_format_obj(const char* filepath, paws_mesh* mesh);

// Mesh related functions
bool paws_mesh_ctor(paws_mesh* mesh);
void paws_mesh_dtor(paws_mesh* mesh);
void draw_mesh(paws_mesh* mesh);

#endif /* __KITTY_PAWS_DATA_H__ */
