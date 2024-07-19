/** @file
 * @brief Data (types, struct, etc) for the app is declared here */

#ifndef __KITTY_PAWS_DATA_H__
#define __KITTY_PAWS_DATA_H__

// TODO: move this file to model folder

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
 */
typedef struct _paws_mesh {
    cvector* vertices; ///< Contains pointers to Vector3 
    cvector* textures; ///< Contains pointers to Vector2
    cvector* normals;  ///< Contains pointers to Vector3
    cvector* faces;    ///< Contains pointers to cvector with pointers to paws_face_indeces
    bool is_loaded;    ///< Is mesh loaded from file and ready or not

    struct settings {
        enum point_type { ///< How to draw a point
            NONE, SPHERE, CUBE, 
        } point_type;
        // TODO: add radius for point

        Color color_point, color_line;
    } settings;
} paws_mesh;
// TODO: add settings to mesh or object struct
// - [ ]render-type - wireframe, model, texture, ...
// - [x]Draw points as: sphere, cube, ...
// - [x]Color for: points, lines
// - [x]Loaded and ready or not

// TODO: Add gui for change mesh settings
// [ ] Change points type
// [ ] Change points/line colors
// [ ] Render normals or not ( bool var in settings )
// [ ] Render materials or not ( bool var in settings )
// [ ] Render textures or not ( bool var in settings )

// Parser related functions
bool parse_format_obj(const char* filepath, paws_mesh* mesh);

// Mesh related functions
bool paws_mesh_ctor(paws_mesh* mesh);
void paws_mesh_dtor(paws_mesh* mesh);
void draw_mesh(paws_mesh* mesh);

#endif /* __KITTY_PAWS_DATA_H__ */
