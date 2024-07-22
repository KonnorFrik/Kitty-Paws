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
};

inline static bool format_obj_parse_v(char const* line, paws_mesh* mesh);
inline static bool format_obj_parse_vn(char const* line, paws_mesh* mesh);
inline static bool format_obj_parse_f(char const* line, paws_mesh* mesh);
inline static bool format_obj_parse_o(char const* line, paws_mesh* mesh);

/**
 * @brief Read and parse file with '.obj' format. Write data to preallocated object
 * @param[in]  filepath - Path to '.obj' file
 * @param[out] mesh     - Mesh object with preinited cvector objects for write data in it
 * @return status - false: File parsed; true: Some error occurred
 * @version 0.1.0
 */
bool parse_format_obj(const char* filepath, paws_mesh* mesh) {
    FILE* file = fopen(filepath, "rot");

    if ( !file ) {
        #if PRINT_ERROR == 1
        fprintf(stderr, "[ERROR] Can't open file: %s\n", filepath);
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

            // case G_CODE:;
            //     char* group_name = line + 2;
            //     printf("Group: %s\n", group_name);
            //     break;

            case VN_CODE:
                status = format_obj_parse_vn(line_copy, mesh);
                break;

            case O_CODE:
                status = format_obj_parse_o(line_copy, mesh);

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
 * @brief Read and parse one line starts with 'o' code from '.obj' file.
 * @param[in]  line - line from file
 * @param[out] mesh - Mesh object with preinited cvector objects for write data in it
 * @return status - false: File parsed; true: Some error occurred
 * @version 0.1.1
 */
inline static bool format_obj_parse_o(char const* line, paws_mesh* mesh) {
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
 * @return status - false: File parsed; true: Some error occurred
 * @version 0.1.1
 */
inline static bool format_obj_parse_v(char const* line, paws_mesh* mesh) {
    bool status = false;
    Vector3* vertex = calloc(1, sizeof(Vector3));

    if ( !vertex ) {
        #if PRINT_ERROR == 1
        fprintf(stderr, "[ERROR] Can't allocate memory for Vector3\n");
        #endif
        status = true;
    }

    if ( !status && sscanf(line, "%f %f %f", &vertex->x, &vertex->y, &vertex->z) != 3 ) {
        #if PRINT_ERROR == 1
        fprintf(stderr, "[ERROR] Read less than 3 vertices\n");
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
 * @return status - false: File parsed; true: Some error occurred
 * @version 0.1.1
 */
inline static bool format_obj_parse_f(char const* line, paws_mesh* mesh) {
    bool status = false;
    bool loop = true;
    char const* copy = line;
    cvector* line_faces = cvector_new(1);

    if ( !line_faces ) {
        #if PRINT_ERROR == 1
        fprintf(stderr, "[ERROR] Can't allocate memory for faces\n");
        #endif
        status = true;
    }
    
    while ( !status && loop ) { // parse each [...] in line
        paws_face_indices* indeces = calloc(1, sizeof(paws_face_indices));

        if ( !indeces ) {
            #if PRINT_ERROR == 1
            fprintf(stderr, "[ERROR] Can't allocate memory for face indeces\n");
            #endif
            status = true;
            continue;
        }

        if ( sscanf(copy, "%lld", &indeces->vertex_index) == 1 ) {
            indeces->vertex_index--;

        } else {
            #if PRINT_ERROR == 1
            fprintf(stderr, "[ERROR] Read less than 1 face index\n");
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
                        fprintf(stderr, "[ERROR] Can't read normal from face line\n");
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
 * @return status - false: File parsed; true: Some error occurred
 * @version 0.1.1
 */
inline static bool format_obj_parse_vn(char const* line, paws_mesh* mesh) {
    bool status = false;
    Vector3* normal = calloc(1, sizeof(Vector3));

    if ( !normal ) {
        #if PRINT_ERROR == 1
        fprintf(stderr, "[ERROR] Can't allocate memory for normal\n");
        #endif
        status = true;
    }

    if ( sscanf(line, "%f %f %f", &normal->x, &normal->y, &normal->z) != 3 ) {
        #if PRINT_ERROR == 1
        fprintf(stderr, "[ERROR] Read less than 3 normals\n");
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

