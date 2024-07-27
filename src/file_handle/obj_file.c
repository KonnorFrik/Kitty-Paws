#include "../paws_data.h"

#include <raylib.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PARSER_DEBUG 0
#define PRINT_ERROR 1

enum obj_file_codes {
    V_CODE = ('v' << 8) | ' ',
    VT_CODE = ('v' << 8) | 't',
    VN_CODE = ('v' << 8) | 'n',
    F_CODE = ('f' << 8) | ' ',
    G_CODE = ('g' << 8) | ' ',
    O_CODE = ('o' << 8) | ' ',
    S_CODE = ('s' << 8) | ' ',
};

inline static bool format_obj_parse_v(const char* line, paws_mesh* mesh);
inline static bool format_obj_parse_vn(const char* line, paws_mesh* mesh);
inline static bool format_obj_parse_vt(const char* line, paws_mesh* mesh);
inline static bool format_obj_parse_f(const char* line, paws_mesh* mesh);
inline static bool format_obj_parse_o(const char* line, paws_mesh* mesh);
inline static bool format_obj_parse_s(const char* line, paws_mesh* mesh);

/**
 * @brief Read and parse file with '.obj' format. Write data to preallocated object
 * @param[in]  filepath - Path to '.obj' file
 * @param[out] mesh     - Mesh object with preinited cvector objects for write data in it
 * @return status - false:OK; true:ERROR
 * @version 0.1.0
 */
bool parse_format_obj(const char* filepath, paws_mesh* mesh) {
    if ( mesh->is_loaded ) {
        return true;
    }

    FILE* file = fopen(filepath, "rot");

    if ( !file ) {
        #if PRINT_ERROR == 1
        fprintf(stderr, "[PARSE ERROR] Can't open file: %s\n", filepath);
        #endif
        return true;
    }

    bool status = false;
    char* line = NULL;
    size_t line_size = 0;

    while ( !status && getline(&line, &line_size, file) != -1 ) {
        unsigned int data = (line[0] << 8) | line[1];
        char* line_copy = line + 2; // skip prefix

        switch ( data ) {
            case V_CODE:
                status = format_obj_parse_v(line_copy, mesh);
                break;

            case F_CODE: // parse lines f [v1 ...] | [v1/vt1 ...] | [v1/vt1/vn1 ...] | [v1//vn1 ...]
                status = format_obj_parse_f(line_copy, mesh);
                break;

            case VT_CODE:
                status = format_obj_parse_vt(line_copy, mesh);
                break;

            // case G_CODE:;
            //     char* group_name = line + 2;
            //     printf("Group: %s\n", group_name);
            //     break;

            case VN_CODE:
                status = format_obj_parse_vn(line_copy, mesh);
                break;

            case O_CODE:
                status = format_obj_parse_o(line_copy, mesh);
                break;

            case S_CODE:
                status = format_obj_parse_s(line_copy, mesh);
                break;

            default: break;

        }

    }

    if ( line ) {
        free(line);
    }

    fclose(file);

    if ( !status ) {
        mesh->is_loaded = true;
    }

    return status;
}

/**
 * @brief Save mesh to file 
 * @param[in] filepath Filepath for save
 * @param[in] mesh     paws_mesh object for save
 * @return status false:OK true:ERROR
 * @version 0.1.0
 */
bool save_format_obj(const char* filepath, paws_mesh* mesh) {
    if ( !mesh->is_loaded ) {
        return true;
    }

    // TODO: use this func for save object (mesh + materials + textures)
    // change argument from 'paws_mesh' to 'paws_object'
    bool status = false;
    FILE* file = fopen(filepath, "wt");

    if ( !file ) {
        #if PRINT_ERROR == 1
        fprintf(stderr, "[PARSE ERROR] Can't open file for save: '%s'\n", filepath);
        #endif
        status = true;
    }

    // Write comments with program name and version
    if ( !status ) {
        fprintf(file, "#Kitty-Paws v%s\n", KITTY_PAWS_VERSION_STR);
    }

    // write mtllib file

    // write object name
    if ( !status ) {
        fprintf(file, "o %s\n", mesh->name);
    }

    // write all vertices
    for (size_t vi = 0; !status && vi < cvector_size(mesh->vertices); ++vi) {
        Vector3* vertex = cvector_at(mesh->vertices, vi);
        fprintf(file, "v %.6f %.6f %.6f\n", vertex->x, vertex->y, vertex->z);
    }

    // write all normals
    for (size_t vi = 0; !status && vi < cvector_size(mesh->normals); ++vi) {
        Vector3* normal = cvector_at(mesh->normals, vi);
        fprintf(file, "vn %.6f %.6f %.6f\n", normal->x, normal->y, normal->z);
    }

    // write all textures
    for (size_t vi = 0; !status && vi < cvector_size(mesh->textures); ++vi) {
        Vector2* texture = cvector_at(mesh->textures, vi);
        fprintf(file, "vt %.6f %.6f\n", texture->x, texture->y);
    }

    // write 's' param ( shader shooth)
    // write 'usemtl ...' TODO: find how to use materials

    // write smooth shading
    fprintf(file, "s %d", mesh->smooth_shade);

    // write all faces
    for (size_t fi = 0; fi < cvector_size(mesh->faces); ++fi) {
        cvector* faces = cvector_at(mesh->faces, fi);

        fprintf(file, "f");

        for (size_t vi = 0; vi < cvector_size(faces); ++vi) {
            paws_face_indices* one_face = cvector_at(faces, vi);

            fprintf(file, " %lld/%lld/%lld", one_face->vertex_index + 1, one_face->texture_index + 1, one_face->normal_index + 1);
        }

        fprintf(file, "\n");
    }

    return status;
}

/**
 * @brief Read and parse one line starts with 'vt' code from '.obj' file.
 * @param[in]  line - line from file
 * @param[out] mesh - Mesh object with preinited cvector objects for write data in it
 * @return status - false:OK; true:ERROR
 * @version 0.1.0
 */
inline static bool format_obj_parse_vt(const char* line, paws_mesh* mesh) {
    bool status = false;
    Vector2* texture = calloc(1, sizeof(Vector2));

    if ( !texture ) {
        status = true;
    }

    if ( !status ) {
        if ( sscanf(line, "%f %f", &texture->x, &texture->y) != 2 ) {
            #if PRINT_ERROR == 1
            fprintf(stderr, "[PARSE ERROR] Read less than 2 texture coordinates\n");
            #endif

            status = true;

        } else {
            cvector_push_back(mesh->textures, texture);
        }
    }

    return status;
}

/**
 * @brief Read and parse one line starts with 'o' code from '.obj' file.
 * @param[in]  line - line from file
 * @param[out] mesh - Mesh object with preinited cvector objects for write data in it
 * @return status - false:OK; true:ERROR
 * @version 0.1.1
 */
inline static bool format_obj_parse_o(const char* line, paws_mesh* mesh) {
    bool status = false;

    size_t len = 0;

    while ( line[len] != '\n' && line[len] != '\0') {
        len++;
    }

    if ( len == 0 ) {
        status = true;
    }

    if ( !status && !mesh->name ) {
        mesh->name = calloc(len, sizeof(char));

        if ( !mesh->name ) { 
            status = true;
        }
    }

    if ( !status ) {
        strncpy(mesh->name, line, len);
    }

    return status;
}

/**
 * @brief Read and parse one line starts with 'v' code from '.obj' file.
 * @param[in]  line - line from file
 * @param[out] mesh - Mesh object with preinited cvector objects for write data in it
 * @return status - false:OK; true:ERROR
 * @version 0.1.1
 */
inline static bool format_obj_parse_v(const char* line, paws_mesh* mesh) {
    bool status = false;
    Vector3* vertex = calloc(1, sizeof(Vector3));

    if ( !vertex ) {
        #if PRINT_ERROR == 1
        fprintf(stderr, "[PARSE ERROR] Can't allocate memory for Vector3\n");
        #endif
        status = true;
    }

    if ( !status && sscanf(line, "%f %f %f", &vertex->x, &vertex->y, &vertex->z) != 3 ) {
        #if PRINT_ERROR == 1
        fprintf(stderr, "[PARSE ERROR] Read less than 3 vertices\n");
        #endif
        status = true;
    };

    if ( !status ) {
        status = cvector_push_back(mesh->vertices, vertex);
    }

    // #if PARSER_DEBUG == 1
    // printf("Pushed vertex: %f %f %f\n", vertex->x, vertex->y, vertex->z);
    // #endif

    return status;
}

/**
 * @brief Read and parse one line starts with 'f' code from '.obj' file.
 * @param[in]  line - line from file
 * @param[out] mesh - Mesh object with preinited cvector objects for write data in it
 * @return status - false:OK; true:ERROR
 * @version 0.1.1
 */
inline static bool format_obj_parse_f(const char* line, paws_mesh* mesh) {
    bool status = false;
    bool loop = true;
    char const* copy = line;
    cvector* line_faces = cvector_new(1);

    if ( !line_faces ) {
        #if PRINT_ERROR == 1
        fprintf(stderr, "[PARSE ERROR] Can't allocate memory for faces\n");
        #endif
        status = true;
    }
    
    while ( !status && loop ) { // parse each [...] in line
        paws_face_indices* indeces = calloc(1, sizeof(paws_face_indices));

        if ( !indeces ) {
            #if PRINT_ERROR == 1
            fprintf(stderr, "[PARSE ERROR] Can't allocate memory for face indeces\n");
            #endif
            status = true;
            continue;
        }

        if ( sscanf(copy, "%lld", &indeces->vertex_index) == 1 ) {
            indeces->vertex_index--;

        } else {
            #if PRINT_ERROR == 1
            fprintf(stderr, "[PARSE ERROR] Read less than 1 face index\n");
            #endif
            status = true;
            continue;
        }

        int which_ind = 1; // 1 - for texture, 2 - for normal

        while ( !status && (*copy != ' ' && *copy != '\n' && *copy != '\0') ) {
            if ( *copy == '/' ) {
                if ( which_ind == 1 ) {
                    // read vt
                    // do not set status to 1 (vt can be not defined)
                    which_ind++;
                    // indeces->texture_index--;

                } else if (which_ind == 2) {
                    if ( sscanf(++copy, "%lld", &indeces->normal_index) != 1 ) {
                        #if PRINT_ERROR == 1
                        fprintf(stderr, "[PARSE ERROR] Can't read normal from face line\n");
                        #endif
                        status = true;
                        continue;
                    }

                    which_ind++;
                    indeces->normal_index--;
                } // else unknown behaviour
            }

            copy++;
        }

        if ( !status ) {
            status = cvector_push_back(line_faces, indeces);
        }

        if ( *copy == '\0' || *copy == '\n' ) {
            loop = false;
            continue;
        }

        copy++; // skip space

        // #if PARSER_DEBUG == 1
        // printf("Pushed face ind: %d\n", face_ind);
        // #endif
    }

    if ( !status ) {
        status = cvector_push_back(mesh->faces, line_faces);
    }

    return status;
}

/**
 * @brief Read and parse one line starts with 'vn' code from '.obj' file.
 * @param[in]  line - line from file
 * @param[out] mesh - Mesh object with preinited cvector objects for write data in it
 * @return status - false:OK; true:ERROR
 * @version 0.1.1
 */
inline static bool format_obj_parse_vn(const char* line, paws_mesh* mesh) {
    bool status = false;
    Vector3* normal = calloc(1, sizeof(Vector3));

    if ( !normal ) {
        #if PRINT_ERROR == 1
        fprintf(stderr, "[PARSE ERROR] Can't allocate memory for normal\n");
        #endif
        status = true;
    }

    if ( sscanf(line, "%f %f %f", &normal->x, &normal->y, &normal->z) != 3 ) {
        #if PRINT_ERROR == 1
        fprintf(stderr, "[PARSE ERROR] Read less than 3 normals\n");
        #endif
        status = true;
    }

    if ( !status ) {
        status = cvector_push_back(mesh->normals, normal);
    }

    // #if PARSER_DEBUG == 1
    // printf("Pushed normal: %f %f %f\n", normal->x, normal->y, normal->z);
    // #endif

    return status;
}

/**
 * @brief Read and parse one line starts with 's' code from '.obj' file.
 * @param[in]  line - line from file
 * @param[out] mesh - Mesh object with preinited cvector objects for write data in it
 * @return status - false:OK; true:ERROR
 * @version 0.1.0
 */
inline static bool format_obj_parse_s(const char* line, paws_mesh* mesh) {
    bool status = false;
    int cant_read_count = 0;

    if ( sscanf(line, "%d", &mesh->smooth_shade) != 1 ) {
        cant_read_count++;
    }

    if ( cant_read_count == 1 ) {
        if ( line[0] == 'o' && line[1] == 'n' ) {
            mesh->smooth_shade = 1;

        } else if ( line[0] == 'o' && line[1] == 'o' && line[2] == 'f' ) {
            mesh->smooth_shade = 0;

        } else {
            cant_read_count++;
        }
    }

    if ( cant_read_count == 2 ) {
        #if PRINT_ERROR == 1
        fprintf(stderr, "[PARSE ERROR] Can't read 's' code\n");
        #endif
        status = true;
    }

    return status;
}
