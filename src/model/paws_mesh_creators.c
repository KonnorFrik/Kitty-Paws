#include "../paws_data.h"

#include <stdlib.h>

/**
 * @brief Allocate memory for each member of paws_mesh object
 * @param[out] mesh a paws_mesh object
 * @return status: false:OK true:ERROR
 * @version 0.2.0
 */
bool paws_mesh_ctor(paws_mesh* mesh) {
    bool status = false;

    mesh->vertices = cvector_new(1);
    mesh->textures = cvector_new(1);
    mesh->normals = cvector_new(1);
    mesh->faces = cvector_new(1);

    mesh->is_draw_normals = false;
    mesh->is_draw_edges = true;

    mesh->settings.color_point = BLACK;
    mesh->settings.color_line = BLACK;
    mesh->settings.color_normal = BLUE;
    mesh->settings.point_type = SPHERE;
    mesh->settings.point_radius = 0.1;

    if ( !mesh->vertices || 
         !mesh->textures || 
         !mesh->normals ||
         !mesh->faces
    ) {
        status = true;
    }

    return status;
}

/**
 * @brief Free allocated memory for each member of paws_mesh object
 * @param[out] mesh a paws_mesh object
 * @version 0.1.0
 */
void paws_mesh_dtor(paws_mesh* mesh) {
    for (size_t i = 0; i < cvector_size(mesh->faces); ++i) {
        cvector* face_indeces = cvector_at(mesh->faces, i);

        if ( !face_indeces ) {
            continue;
        }

        for (size_t j = 0; j < cvector_size(face_indeces); ++j) {
            paws_face_indices* obj = cvector_at(face_indeces, j);

            if ( obj ) {
                free(obj);
            }
        }

        cvector_delete(face_indeces);
    }

    cvector_delete(mesh->vertices);
    cvector_delete(mesh->textures);
    cvector_delete(mesh->normals);
    cvector_delete(mesh->faces);
}
