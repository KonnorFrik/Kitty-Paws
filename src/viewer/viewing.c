// File with drawing functions 
#include "../paws_data.h"
#include <raylib.h>

/** 
 * @brief Draw each face of a mesh (points, edges)
 * @param[in] mesh - paws_mesh object
 * @version 0.1.0
 */
void draw_mesh(paws_mesh* mesh) {
    size_t faces_count = cvector_size(mesh->faces);
    Color color_point = mesh->settings.color_point;
    Color color_line = mesh->settings.color_line;
    int point_type = mesh->settings.point_type;
    float point_radius = mesh->settings.point_radius;

    for (size_t fi = 0; fi < faces_count; ++fi) {
        cvector* faces = cvector_at(mesh->faces, fi);

        size_t indices_count = cvector_size(faces) - 1;
        Vector3* start_vertex = cvector_at(mesh->vertices, ((paws_face_indices*)cvector_at(faces, 0))->vertex_index);
        Vector3* end_vertex = NULL;

        for (size_t vi = 0; vi < indices_count; ++vi) {
            paws_face_indices* start_face = cvector_at(faces, vi);
            paws_face_indices* end_face = cvector_at(faces, vi + 1);

            Vector3* start_vert = (Vector3*)cvector_at(mesh->vertices, start_face->vertex_index);
            end_vertex = (Vector3*)cvector_at(mesh->vertices, end_face->vertex_index);

            switch ( point_type ) {
                default:
                case NONE:
                    break;
                case SPHERE:
                    DrawSphere(*start_vert, point_radius, color_point);
                    break;
                case CUBE:
                    DrawCube(*start_vert, point_radius, point_radius, point_radius, color_point);
                    break;
            }

            DrawLine3D(*start_vert, *end_vertex, color_line);
        }

            DrawLine3D(*start_vertex, *end_vertex, color_line);
    }
}
