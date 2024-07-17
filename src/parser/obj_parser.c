#include "../paws_data.h"
#include <raylib.h>

/**
 * @brief Read and parse file with '.obj' format. Write data to preallocated object
 * @param[in]  filepath - Path to '.obj' file
 * @param[out] mesh     - Mesh object with preinited cvector objects for write data in it
 * @return status - false: File parsed; true: Some error occurred
 */
bool parse_format_obj(const char* filepath, paws_mesh* mesh);
