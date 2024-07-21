// File with drawing functions 
#include "../paws_data.h"
#include <raylib.h>
#include <raymath.h>

#define DRAW_DEBUG 0

/** 
 * @brief Draw each face of a mesh (points, edges)
 * @param[in] mesh - paws_mesh object
 * @version 0.3.0
 */
void draw_mesh(paws_mesh* mesh) {
    size_t faces_count = cvector_size(mesh->faces);
    int point_type = mesh->settings.point_type;
    float point_radius = mesh->settings.point_radius;

    Color color_point = mesh->settings.color_point;
    Color color_line = mesh->settings.color_edge;
    Color color_normal = mesh->settings.color_normal;

    bool is_draw_normals = mesh->is_draw_normals;

    for (size_t fi = 0; fi < faces_count; ++fi) {
        cvector* faces = cvector_at(mesh->faces, fi);

        size_t indices_count = cvector_size(faces) - 1;

        Vector3* start_vertex = cvector_at(mesh->vertices, ((paws_face_indices*)cvector_at(faces, 0))->vertex_index);
        Vector3* end_vertex = NULL;

        Vector3 face_mid_point = *start_vertex;
        Vector3 face_normal = *(Vector3*)cvector_at(mesh->normals, ((paws_face_indices*)cvector_at(faces, 0))->normal_index);

        for (size_t vi = 0; vi < indices_count; ++vi) {
            paws_face_indices* start_face = cvector_at(faces, vi);
            paws_face_indices* end_face = cvector_at(faces, vi + 1);

            Vector3* start_vert = (Vector3*)cvector_at(mesh->vertices, start_face->vertex_index);
            end_vertex = (Vector3*)cvector_at(mesh->vertices, end_face->vertex_index);

            Vector3 normal = *(Vector3*)cvector_at(mesh->normals, end_face->normal_index);

            face_normal.x += normal.x;
            face_normal.y += normal.y;
            face_normal.z += normal.z;

            face_mid_point.x += end_vertex->x;
            face_mid_point.y += end_vertex->y;
            face_mid_point.z += end_vertex->z;

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

            if ( mesh->is_draw_edges ) {
                DrawLine3D(*start_vert, *end_vertex, color_line);
            }
        }

        if ( mesh->is_draw_edges ) {
            DrawLine3D(*start_vertex, *end_vertex, color_line);
        }

        if ( is_draw_normals ) {
            face_mid_point.x /= indices_count + 1;
            face_mid_point.y /= indices_count + 1;
            face_mid_point.z /= indices_count + 1;

            face_normal = Vector3Normalize(face_normal);
            face_normal = Vector3Add(face_mid_point, face_normal);

            DrawLine3D(face_mid_point, face_normal, color_normal);

            #if DRAW_DEBUG == 1
            DrawSphere(face_mid_point, 0.07, RED);
            #endif
        }

    }
}
