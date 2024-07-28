#include "../paws_data.h"
#include "../raygui_impl/raygui.h"
#include <raylib.h>
#include <stdlib.h>
#include <raymath.h>
#include <stdio.h>

#define APP_TITLE "Kitty Paws"
#define DEBUG_PRINT 0
#define FILEPATH_MAX_SIZE 1024

// TODO: write func for draw gui
// for mesh settings: args - ptr to mesh
// for camera: ptr to default camera, ptr to custom camera

void paws_mesh_clear(paws_mesh* mesh) {
    paws_mesh_dtor(mesh);
    paws_mesh_ctor(mesh);
}

int main() {
    // Initialize anything here
    int status = 0;

    int screen_width = 900;
    int screen_height = 900;
    int target_fps = 60;

    // hardcoded value for testing
    char* mesh_filepath = calloc(FILEPATH_MAX_SIZE, sizeof(char));
    // char* mesh_filepath = "/home/konnor/code/c/graphics/3d_objects/cube/cube.obj";
    // char* mesh_filepath = "test.obj";
    // char* mesh_filepath = "/home/konnor/code/c/graphics/3d_objects/notebook_1/Lowpoly_Notebook_2.obj";
    paws_mesh mesh = {0};

    if ( !mesh_filepath ) {
        fprintf(stderr, "[ERROR] Can't allocate memory for filepath\n");
        status = 1;
    }

    if ( paws_mesh_ctor(&mesh) ) {
        status = 1;
        fprintf(stderr, "[ERROR] Can't Allocate memory for mesh\n");
    }

    Color color_background = WHITE;

    Camera camera = {
        .position = {.x = 0, .y = 1, .z = 4},
        .target = {.x = 0, .y = 0, .z = 0},
        .up = {.x = 0, .y = 1, .z = 0},
        .fovy = 45,
        .projection = CAMERA_PERSPECTIVE,
    };

    // GUI variables
    bool is_show_settings_window = false;

    // Settings button
    Rectangle gui_rect_settings_button = {0, 0, 95, 35};
    gui_rect_settings_button.x = screen_width - gui_rect_settings_button.width;

    // Settings window
    Rectangle gui_rect_settings_window = {0, 0, 500, screen_height};
    gui_rect_settings_window.x = screen_width - gui_rect_settings_window.width;

    // Save/Load windows
    bool gui_show_window_load = false;
    bool gui_show_window_save = false;

    static Rectangle gui_textinput_load_save_file = {
        .x = 50,
        .y = 30,
        .width = 450,
        .height = 130,
    };

    // Points focus
    cvector* focus_points = cvector_new(1);
    bool append_focus_points = false;
    Color color_drag_axis_x = RED;
    Color color_drag_axis_y = GREEN;
    Color color_drag_axis_z = BLUE;
    BoundingBox drag_axis_x_focus_points = {0};
    BoundingBox drag_axis_y_focus_points = {0};
    BoundingBox drag_axis_z_focus_points = {0};
    float drag_axis_size = 1.0f;
    float drag_axis_sub_size = drag_axis_size / 40;
    bool drag_axis_collision = false;
    RayCollision ray_collision_drag_axis_x = {0};
    RayCollision ray_collision_drag_axis_y = {0};
    RayCollision ray_collision_drag_axis_z = {0};

    SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, APP_TITLE);
    // TODO: add in gui settings text size ( with GuiSpinner)
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    GuiSetIconScale(2);
    SetTargetFPS(target_fps);

    while ( !status && !WindowShouldClose() ) {
        // Update anything here ------------------

        Vector2 mouse_pos = GetMousePosition();

        if ( IsMouseButtonDown(MOUSE_BUTTON_RIGHT) &&
             ( !is_show_settings_window || ( is_show_settings_window && !CheckCollisionPointRec(mouse_pos, gui_rect_settings_window) ) )
        ) {
            UpdateCamera(&camera, CAMERA_THIRD_PERSON);
        }

        if ( IsKeyPressed(KEY_LEFT_CONTROL) ) {
            append_focus_points = true;

        } else if ( IsKeyReleased(KEY_LEFT_CONTROL) ) {
            append_focus_points = false;
        }

        if ( IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ) {
            ray_collision_drag_axis_x = GetRayCollisionBox(GetMouseRay(mouse_pos, camera), drag_axis_x_focus_points);
            ray_collision_drag_axis_y = GetRayCollisionBox(GetMouseRay(mouse_pos, camera), drag_axis_y_focus_points);
            ray_collision_drag_axis_z = GetRayCollisionBox(GetMouseRay(mouse_pos, camera), drag_axis_z_focus_points);

            if ( ray_collision_drag_axis_x.hit ||
                 ray_collision_drag_axis_y.hit ||
                 ray_collision_drag_axis_z.hit
            ) {
                drag_axis_collision = true;

            } else {
                drag_axis_collision = false;
                // ray_collision_drag_axis_x = (RayCollision){0}; 
                // ray_collision_drag_axis_y = (RayCollision){0};
                // ray_collision_drag_axis_z = (RayCollision){0};
            }

        } else if ( IsMouseButtonReleased(MOUSE_BUTTON_LEFT) ) {
            drag_axis_collision = false;
        }

        // Choose points for modify
        if ( IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mesh.is_loaded && 
             ( !is_show_settings_window || ( is_show_settings_window && !CheckCollisionPointRec(mouse_pos, gui_rect_settings_window) ) ) &&
             ( !gui_show_window_save ) &&
             ( !gui_show_window_load )
        ) {
            Ray ray_mouse_point_collision = GetMouseRay(mouse_pos, camera);
            float ray_collision_dist_min = INFINITY;

            for (size_t vi = 0; vi < cvector_size(mesh.vertices); ++vi) {
                Vector3* check_point = cvector_at(mesh.vertices, vi);
                RayCollision ray_collision = {0};

                switch ( mesh.settings.point_type ) {
                    case SPHERE:
                        {
                            ray_collision = GetRayCollisionSphere(ray_mouse_point_collision, *check_point, mesh.settings.point_radius);
                            if ( ray_collision.hit ) {
                                #if DEBUG_PRINT == 1
                                printf("[FOCUS]: Nearest hit point: X:%.2f Y:%.2f Z:%.2f\n", ray_collision.point.x, ray_collision.point.y, ray_collision.point.z);
                                printf("[FOCUS]: Nearest dist: %.3f\n", ray_collision.distance);
                                #endif
                            }
                        }
                        break;
                    case CUBE:
                        {
                            // TODO: rewrite for check box collision
                            ray_collision = GetRayCollisionSphere(ray_mouse_point_collision, *check_point, mesh.settings.point_radius);
                            if ( ray_collision.hit ) {
                                #if DEBUG_PRINT == 1
                                printf("[FOCUS]: Nearest hit point: X:%.2f Y:%.2f Z:%.2f\n", ray_collision.point.x, ray_collision.point.y, ray_collision.point.z);
                                printf("[FOCUS]: Nearest dist: %.3f\n", ray_collision.distance);
                                #endif
                            }
                        }
                        break;

                    default: break;
                }

                // Focus only hitted by ray nearest point
                if ( ray_collision.hit && ray_collision.distance < ray_collision_dist_min ) {
                    if ( !isinf(ray_collision_dist_min) ) { // not inf = collision was occurred before in same loop
                        cvector_pop_back(focus_points);
                    }

                    ray_collision_dist_min = ray_collision.distance;

                    if ( !append_focus_points ) {
                        cvector_clear(focus_points);
                    }

                    if ( !cvector_contain(focus_points, check_point) ) {
                        cvector_push_back(focus_points, check_point);
                    }

                }
            }

            // if don't hit anything interactive, clear points in focus
            if ( isinf(ray_collision_dist_min) &&
                 !ray_collision_drag_axis_x.hit &&
                 !ray_collision_drag_axis_y.hit &&
                 !ray_collision_drag_axis_z.hit &&
                 !append_focus_points
            ) {
                cvector_clear(focus_points);
            }

            #if DEBUG_PRINT == 1
            printf("[FOCUS]: Points in focus: %lu\n", cvector_size(focus_points));
            #endif
        }

        float tmp_size_focus_points = cvector_size(focus_points);
        Vector3 mid_focus_points = {0};

        // Calculate middle point of chosen vertices and xyz axis
        if ( tmp_size_focus_points ) {
            for (size_t i = 0; i < tmp_size_focus_points; ++i) {
                Vector3* point = cvector_at(focus_points, i);
                mid_focus_points.x += point->x;
                mid_focus_points.y += point->y;
                mid_focus_points.z += point->z;
            }

            mid_focus_points.x /= tmp_size_focus_points;
            mid_focus_points.y /= tmp_size_focus_points;
            mid_focus_points.z /= tmp_size_focus_points;

            // Calculate dragable axis
            drag_axis_x_focus_points.min = (Vector3){
                .x = mid_focus_points.x,
                .y = mid_focus_points.y - drag_axis_sub_size,
                .z = mid_focus_points.z - drag_axis_sub_size,
            };

            drag_axis_x_focus_points.max = (Vector3){
                .x = mid_focus_points.x + drag_axis_size,
                .y = mid_focus_points.y + drag_axis_sub_size,
                .z = mid_focus_points.z + drag_axis_sub_size,
            };

            drag_axis_y_focus_points.min = (Vector3){
                .x = mid_focus_points.x - drag_axis_sub_size,
                .y = mid_focus_points.y,
                .z = mid_focus_points.z - drag_axis_sub_size,
            };

            drag_axis_y_focus_points.max = (Vector3){
                .x = mid_focus_points.x + drag_axis_sub_size,
                .y = mid_focus_points.y + drag_axis_size,
                .z = mid_focus_points.z + drag_axis_sub_size,
            };

            drag_axis_z_focus_points.min = (Vector3){
                .x = mid_focus_points.x - drag_axis_sub_size,
                .y = mid_focus_points.y - drag_axis_sub_size,
                .z = mid_focus_points.z,
            };

            drag_axis_z_focus_points.max = (Vector3){
                .x = mid_focus_points.x + drag_axis_sub_size,
                .y = mid_focus_points.y + drag_axis_sub_size,
                .z = mid_focus_points.z + drag_axis_size,
            };
        }

        // Move focus points by dragable axis if hitted 
        if ( tmp_size_focus_points && drag_axis_collision &&
             ( !gui_show_window_save ) &&
             ( !gui_show_window_load )
        ) {
            Vector2 mouse_delta = GetMouseDelta();
            float move_point_speed_x = mouse_delta.x / 100;
            float move_point_speed_y = mouse_delta.y / 100;
            #if DEBUG_PRINT == 1
            printf("Delta for x: %f\n", mouse_delta.x);
            printf("Delta for y: %f\n", mouse_delta.y);
            printf("Speed for x: %f\n", move_point_speed_x);
            printf("Speed for y: %f\n", move_point_speed_y);
            printf("\n");
            #endif

            if ( ray_collision_drag_axis_x.hit ) {
                for (size_t i = 0; i < tmp_size_focus_points; ++i) {
                    Vector3* point = cvector_at(focus_points, i);
                    point->x += move_point_speed_x;
                }

            } else if ( ray_collision_drag_axis_y.hit ) {
                for (size_t i = 0; i < tmp_size_focus_points; ++i) {
                    Vector3* point = cvector_at(focus_points, i);
                    point->y -= move_point_speed_y;
                }

            } else if ( ray_collision_drag_axis_z.hit ) {
                for (size_t i = 0; i < tmp_size_focus_points; ++i) {
                    Vector3* point = cvector_at(focus_points, i);
                    point->z += (-move_point_speed_x + move_point_speed_y) / 1.5;
                }
            }
        }

        // Draw gui at position relative to window width && height
        int tmp_screen_width = GetScreenWidth();
        int tmp_screen_height = GetScreenHeight();
        int width_diff;
        int height_diff;

        if ( (width_diff = tmp_screen_width - screen_width) ) {
            screen_width = tmp_screen_width;

            // gui_settings_window_view_btn.x += width_diff;
            gui_rect_settings_button.x += width_diff;
            gui_rect_settings_window.x += width_diff;
        }

        if ( (height_diff = tmp_screen_height - screen_height) ) {
            screen_height = tmp_screen_height;
            gui_rect_settings_window.height = screen_height;
        }

        // Draw anything here ------------------
        BeginDrawing();
        {
            ClearBackground(color_background);
            BeginMode3D(camera);
            {
                // DrawGrid(15, 10);

                if ( mesh.is_loaded ) {
                    draw_mesh(&mesh);
                }

                // Draw chosen vertices
                if ( tmp_size_focus_points > 1 ) {
                    for (size_t i = 0; i < tmp_size_focus_points; ++i) {
                        DrawSphere(*(Vector3*)cvector_at(focus_points, i), mesh.settings.point_radius, GOLD);
                    }
                }

                if ( tmp_size_focus_points ) {
                    DrawSphere(mid_focus_points, mesh.settings.point_radius, GRAY);

                    // Draw dragable axis
                    DrawBoundingBox(drag_axis_x_focus_points, color_drag_axis_x);
                    DrawBoundingBox(drag_axis_y_focus_points, color_drag_axis_y);
                    DrawBoundingBox(drag_axis_z_focus_points, color_drag_axis_z);
                }

            }
            EndMode3D();

            if ( mesh.name ) {
                DrawText(mesh.name, 5, 5, 19, BLACK);
            }

            //Draw GUI here ------------------
            if ( !is_show_settings_window && GuiButton(gui_rect_settings_button, "Settings") ) {
                is_show_settings_window = !is_show_settings_window;
            }

            Rectangle gui_button_load_file = {
                .x = 5,
                .y = 30,
                .width = 35,
                .height = 35,
            };

            Rectangle gui_button_save_file = {
                .x = gui_button_load_file.x,
                .y = gui_button_load_file.y + gui_button_load_file.height + 5,
                .width = 35,
                .height = 35,
            };

            // Load button
            if ( GuiButton(gui_button_load_file, "#05#") ) {
                gui_show_window_load = !gui_show_window_load;
                gui_show_window_save = false;
            }

            // Save button
            if ( GuiButton(gui_button_save_file, "#06#") ) {
                gui_show_window_save = !gui_show_window_save;
                gui_show_window_load = false;
            }

            static Rectangle gui_label_help_text = {
                .x = 0,
                .y = 0,
                .width = 115,
                .height = 15,
            };

            // Draw help msg for load/save
            if ( CheckCollisionPointRec(mouse_pos, gui_button_load_file) ) {
                gui_label_help_text.x = mouse_pos.x + 15;
                gui_label_help_text.y = mouse_pos.y + 5;
                GuiLabel(gui_label_help_text, "Open File");
            }

            if ( CheckCollisionPointRec(mouse_pos, gui_button_save_file) ) {
                gui_label_help_text.x = mouse_pos.x + 15;
                gui_label_help_text.y = mouse_pos.y + 5;
                GuiLabel(gui_label_help_text, "Save File");
            }

            // Draw window for load files
            if ( gui_show_window_load ) {
                if ( GuiTextInputBox(gui_textinput_load_save_file, "Open file", "Filepath for open", "Open", mesh_filepath, FILEPATH_MAX_SIZE, false) == 1 ) {
                    paws_mesh_clear(&mesh);

                    if ( parse_format_obj(mesh_filepath, &mesh) ) {
                        fprintf(stderr, "[LOAD] Can't open file\n");
                    }

                    gui_show_window_load = false;
                }
            }

            // Draw window for save files
            if ( gui_show_window_save ) {
                if ( GuiTextInputBox(gui_textinput_load_save_file, "Save File", "Filepath for save", "Save", mesh_filepath, FILEPATH_MAX_SIZE, false) == 1 ) {
                    save_format_obj(mesh_filepath, &mesh);
                    gui_show_window_save = false;
                }
            }

            // Draw window with settings controls
            if ( is_show_settings_window ) {
                if ( GuiWindowBox(gui_rect_settings_window, "Settings") ) {
                    is_show_settings_window = false;
                }

                enum _settings_mode {
                    VIEW = 0, CAMERA, OBJECT,
                };

                static const char* settings_window_mode_text[] = {
                    "View", "Camera", "Object"
                };

                static const char* settings_window_camera_mode[] = {
                    "Default", "Custom"
                };

                static enum _settings_mode settings_mode = VIEW;

                // Settings view mode button
                Rectangle gui_settings_window_view_btn = {
                    .x = gui_rect_settings_window.x + 5,
                    .y = gui_rect_settings_window.y + 30,
                    .width = 90,
                    .height = 30
                };

                // Settings camera mode button
                Rectangle gui_settings_window_camera_btn = {
                    .x = gui_settings_window_view_btn.x + gui_settings_window_view_btn.width + 10,
                    .y = gui_settings_window_view_btn.y,
                    .width = 90,
                    .height = 30
                };

                // Settings object mode button
                Rectangle gui_settings_window_object_btn = {
                    .x = gui_settings_window_camera_btn.x + gui_settings_window_camera_btn.width + 10,
                    .y = gui_settings_window_camera_btn.y,
                    .width = 90,
                    .height = 30
                };

                // Current settings mode text
                // NOTE: draw elements from this anchor
                Rectangle gui_settings_line = {
                    .x = gui_rect_settings_window.x,
                    .y = gui_settings_window_view_btn.y + 50,
                    .width = gui_rect_settings_window.width,
                    .height = 0
                };

                if ( GuiButton(gui_settings_window_view_btn, "View") ) {
                    settings_mode = VIEW;
                }

                if ( GuiButton(gui_settings_window_camera_btn, "Camera") ) {
                    settings_mode = CAMERA;
                }

                if ( GuiButton(gui_settings_window_object_btn, "Object") ) {
                    settings_mode = OBJECT;
                }

                GuiLine(gui_settings_line, settings_window_mode_text[settings_mode]);

                switch ( settings_mode ) {
                    case VIEW:
                        {
                            Rectangle gui_settings_view_line_point_type = {
                                .x = gui_rect_settings_window.x + 5,
                                .y = gui_settings_line.y + 25,
                                .width = 130,
                                .height = 0,
                            };

                            Rectangle gui_settings_view_dropbox_point_type = {
                                .x = gui_rect_settings_window.x + 15,
                                .y = gui_settings_view_line_point_type.y + 15,
                                .width = 110,
                                .height = 30
                            }; 

                            // Point radius area
                            Rectangle gui_settings_view_line_point_radius = {
                                .x = gui_settings_view_line_point_type.x + gui_settings_view_line_point_type.width + 15,
                                .y = gui_settings_view_line_point_type.y,
                                .width = 170,
                                .height = 0
                            };

                            Rectangle gui_settings_view_slider_point_radius = {
                                .x = gui_settings_view_line_point_radius.x + 40,
                                .y = gui_settings_view_line_point_radius.y + 10,
                                .width = 100,
                                .height = 30
                            };

                            // Point ColorPicker area
                            Rectangle gui_settings_view_line_point_colorpick = {
                                .x = gui_settings_view_line_point_radius.x + gui_settings_view_line_point_radius.width + 10,
                                .y = gui_settings_view_line_point_radius.y,
                                .width = 160,
                                .height = 0
                            };

                            Rectangle gui_settings_view_colorpicker_point = {
                                .x = gui_settings_view_line_point_colorpick.x,
                                .y = gui_settings_view_line_point_colorpick.y + 10,
                                .width = 130,
                                .height = 130
                            };

                            // Draw edges switch area
                            Rectangle gui_settings_view_checkbox_edges = {
                                .x = gui_settings_view_dropbox_point_type.x,
                                .y = gui_settings_view_dropbox_point_type.y + 140,
                                .width = 30,
                                .height = 30
                            };

                            // Edge ColorPicker area
                            Rectangle gui_settings_view_line_edge_colorpick = {
                                .x = gui_settings_view_line_point_colorpick.x,
                                .y = gui_settings_view_checkbox_edges.y,
                                .width = 160,
                                .height = 0
                            };

                            Rectangle gui_settings_view_colorpicker_edge = {
                                .x = gui_settings_view_line_edge_colorpick.x,
                                .y = gui_settings_view_line_edge_colorpick.y + 10,
                                .width = 130,
                                .height = 130
                            };

                            // Draw normals switch area
                            Rectangle gui_settings_view_checkbox_normals = {
                                .x = gui_settings_view_checkbox_edges.x,
                                .y = gui_settings_view_checkbox_edges.y + 155,
                                .width = 30,
                                .height = 30
                            };

                            // Normal ColorPicker area
                            Rectangle gui_settings_view_line_normal_colorpick = {
                                .x = gui_settings_view_colorpicker_edge.x,
                                .y = gui_settings_view_checkbox_normals.y,
                                .width = 160,
                                .height = 0
                            };

                            Rectangle gui_settings_view_colorpicker_normal = {
                                .x = gui_settings_view_line_normal_colorpick.x,
                                .y = gui_settings_view_line_normal_colorpick.y + 10,
                                .width = 130,
                                .height = 130
                            };

                            // Background ColorPicker area
                            Rectangle gui_settings_view_line_background_colorpick = {
                                .x = gui_settings_view_colorpicker_normal.x - 25,
                                .y = gui_settings_view_colorpicker_normal.y + gui_settings_view_colorpicker_normal.height + 20,
                                .width = 160,
                                .height = 0
                            };

                            Rectangle gui_settings_view_colorpicker_background = {
                                .x = gui_settings_view_colorpicker_normal.x,
                                .y = gui_settings_view_line_background_colorpick.y + 15,
                                .width = 130,
                                .height = 130
                            };

                            GuiLine(gui_settings_view_line_point_type, "Point type");

                            // Change point type
                            static int dropbox_chosen = 1;
                            static bool dropbox_mode = false;
                            static Vector3 color_hsv_point = {0};
                            static Vector3 color_hsv_edge = {0};
                            static Vector3 color_hsv_normal = {0};
                            static Vector3 color_hsv_background = {0, 0, 1};

                            if ( GuiDropdownBox(gui_settings_view_dropbox_point_type, "None;Sphere;Cube", &dropbox_chosen, dropbox_mode) ) {
                                dropbox_mode = !dropbox_mode;
                            }

                            switch ( dropbox_chosen ) {
                                case 0:
                                    mesh.settings.point_type = NONE;
                                    break;
                                case 1:
                                    mesh.settings.point_type = SPHERE;
                                    break;
                                case 2:
                                    mesh.settings.point_type = CUBE;
                                    break;
                                default: break;
                            }

                            // Change point radius
                            float gui_settings_point_radius_min = 0.01f, gui_settings_point_radius_max = 0.2f;
                            GuiLine(gui_settings_view_line_point_radius, "Point Radius");
                            GuiSlider(gui_settings_view_slider_point_radius, "0.01", "0.2", &mesh.settings.point_radius, gui_settings_point_radius_min, gui_settings_point_radius_max);

                            // Draw edges switch
                            GuiCheckBox(gui_settings_view_checkbox_edges, "Draw Edges", &mesh.is_draw_edges);

                            // Point colorPicker
                            GuiLine(gui_settings_view_line_point_colorpick, "Point Color");
                            GuiColorPickerHSV(gui_settings_view_colorpicker_point, 0, &color_hsv_point);
                            mesh.settings.color_point = ColorFromHSV(color_hsv_point.x, color_hsv_point.y, color_hsv_point.z);

                            // Edge colorPicker
                            GuiLine(gui_settings_view_line_edge_colorpick, "Edge Color");
                            GuiColorPickerHSV(gui_settings_view_colorpicker_edge, 0, &color_hsv_edge);
                            mesh.settings.color_edge = ColorFromHSV(color_hsv_edge.x, color_hsv_edge.y, color_hsv_edge.z);

                            // Normal draw switch
                            GuiCheckBox(gui_settings_view_checkbox_normals, "Draw Normals", &mesh.is_draw_normals);

                            // Normal colorPicker
                            GuiLine(gui_settings_view_line_normal_colorpick, "Normal Color");
                            GuiColorPickerHSV(gui_settings_view_colorpicker_normal, 0, &color_hsv_normal);
                            mesh.settings.color_normal = ColorFromHSV(color_hsv_normal.x, color_hsv_normal.y, color_hsv_normal.z);

                            // Background ColorPicker
                            GuiLine(gui_settings_view_line_background_colorpick, "Background Color");
                            GuiColorPickerHSV(gui_settings_view_colorpicker_background, 0, &color_hsv_background);
                            color_background = ColorFromHSV(color_hsv_background.x, color_hsv_background.y, color_hsv_background.z);
                        }
                        break;

                    case CAMERA:
                        {
                            Rectangle gui_settings_camera_mode_switch = {
                                .x = gui_settings_line.x + 10,
                                .y = gui_settings_line.y + 20,
                                .width = 30,
                                .height = 30
                            };

                            // Default camera - fovy change
                            Rectangle gui_settings_camera_line_fovy = {
                                .x = gui_settings_camera_mode_switch.x,
                                .y = gui_settings_camera_mode_switch.y + gui_settings_camera_mode_switch.height + 20,
                                .width = 305,
                                .height = 0
                            };

                            Rectangle gui_settings_camera_spinner_fovy = {
                                .x = gui_settings_camera_line_fovy.x,
                                .y = gui_settings_camera_line_fovy.y + 15,
                                .width = 100,
                                .height = 30
                            };

                            // Default camera - fovy change manual input switch
                            Rectangle gui_settings_camera_spinner_fovy_switch = {
                                .x = gui_settings_camera_spinner_fovy.x + gui_settings_camera_spinner_fovy.width + 55,
                                .y = gui_settings_camera_spinner_fovy.y,
                                .width = 30,
                                .height = 30
                            };

                            // Default camera - change projection
                            Rectangle gui_settings_camera_line_projection = {
                                .x = gui_settings_camera_mode_switch.x,
                                .y = gui_settings_camera_spinner_fovy.y + gui_settings_camera_spinner_fovy.height + 30,
                                .width = 180,
                                .height = 0
                            };

                            Rectangle gui_settings_camera_dropbox_projection = {
                                .x = gui_settings_camera_line_projection.x,
                                .y = gui_settings_camera_line_projection.y + 15,
                                .width = 160,
                                .height = 30
                            };

                            static char camera_mode_buffer[32] = "Camera: Default";
                            static bool settings_camera_is_custom_mode = false;
                            static int camera_fovy_min = 20, camera_fovy_max = 120, camera_fovy_value = 45;
                            static bool camera_fovy_is_manual = false;

                            // Switch to default/custom camera
                            if ( GuiCheckBox(gui_settings_camera_mode_switch, camera_mode_buffer, &settings_camera_is_custom_mode) ) {
                                sprintf(camera_mode_buffer, "Camera: %s", settings_window_camera_mode[settings_camera_is_custom_mode]);
                            }

                            if ( settings_camera_is_custom_mode ) {
                                // Custom settings here

                            } else {
                                // Default settings here
                                GuiLine(gui_settings_camera_line_fovy, "fovY");

                                // Change camera fovy
                                GuiSpinner(gui_settings_camera_spinner_fovy, 0, &camera_fovy_value, camera_fovy_min, camera_fovy_max, camera_fovy_is_manual);
                                camera.fovy = camera_fovy_value;

                                // Switch to manual input camera fovy
                                GuiCheckBox(gui_settings_camera_spinner_fovy_switch, "Manual input", &camera_fovy_is_manual);

                                // Change camera projection type
                                static int camera_projection_type = 0; // perspective
                                static bool camera_projection_dropbox_mode = false;
                                GuiLine(gui_settings_camera_line_projection, "Projection Type");

                                if ( GuiDropdownBox(gui_settings_camera_dropbox_projection,
                                                    "Perspective;Orthographic",
                                                    &camera_projection_type, camera_projection_dropbox_mode)
                                ) {
                                    camera_projection_dropbox_mode = !camera_projection_dropbox_mode;
                                }

                                camera.projection = camera_projection_type;
                            }
                        }
                        break;
                    case OBJECT:
                        {
                            Rectangle gui_settings_object_line_scaling = {
                                .x = gui_settings_line.x + 5,
                                .y = gui_settings_line.y + 20,
                                .width = 150,
                                .height = 0,
                            };

                            Rectangle gui_settings_object_btn_minus_scale = {
                                .x = gui_settings_object_line_scaling.x + 10,
                                .y = gui_settings_object_line_scaling.y + 20,
                                .width = 40,
                                .height = 30,
                            };

                            Rectangle gui_settings_object_checkbox_manual_scale_value = {
                                .x = gui_settings_object_btn_minus_scale.x + gui_settings_object_btn_minus_scale.width + 15,
                                .y = gui_settings_object_btn_minus_scale.y,
                                .width = 30,
                                .height = 30,
                            };

                            Rectangle gui_settings_object_valbox_scale = {
                                .x = gui_settings_object_checkbox_manual_scale_value.x + gui_settings_object_checkbox_manual_scale_value.width + 10,
                                .y = gui_settings_object_checkbox_manual_scale_value.y,
                                .width = 90,
                                .height = 30,
                            };

                            Rectangle gui_settings_object_btn_plus_scale = {
                                .x = gui_settings_object_valbox_scale.x + gui_settings_object_valbox_scale.width + 10,
                                .y = gui_settings_object_valbox_scale.y,
                                .width = 40,
                                .height = 30,
                            };

                            Rectangle gui_settings_object_btn_make_scale_up = {
                                .x = gui_settings_object_btn_plus_scale.x + gui_settings_object_btn_plus_scale.width + 10,
                                .y = gui_settings_object_btn_plus_scale.y,
                                .width = 90,
                                .height = 30,
                            };

                            Rectangle gui_settings_object_btn_make_scale_down = {
                                .x = gui_settings_object_btn_make_scale_up.x + gui_settings_object_btn_make_scale_up.width + 10,
                                .y = gui_settings_object_btn_make_scale_up.y,
                                .width = 120,
                                .height = 30,
                            };

                            static bool gui_settings_object_ckeckbox_manscale_value = false;
                            static float gui_settings_object_valbox_scale_val = 1;

                            GuiLine(gui_settings_object_line_scaling, "Scaling");

                            if ( GuiButton(gui_settings_object_btn_minus_scale, "-") ) {
                                gui_settings_object_valbox_scale_val -= 1;
                            }

                            GuiCheckBox(gui_settings_object_checkbox_manual_scale_value, 0, &gui_settings_object_ckeckbox_manscale_value);

                            char scale_value_buffer[32] = {0};
                            sprintf(scale_value_buffer, "%.0f", gui_settings_object_valbox_scale_val);
                            GuiValueBoxFloat(gui_settings_object_valbox_scale,
                                             0,
                                             scale_value_buffer,
                                             &gui_settings_object_valbox_scale_val,
                                             gui_settings_object_ckeckbox_manscale_value);

                            if ( GuiButton(gui_settings_object_btn_plus_scale, "+") ) {
                                gui_settings_object_valbox_scale_val += 1;
                            }

                            if ( GuiButton(gui_settings_object_btn_make_scale_up, "Scale Up") && mesh.is_loaded ) {
                                size_t vert_count = cvector_size(mesh.vertices);

                                for (size_t i = 0; i < vert_count; ++i) {
                                    Vector3* vertex = cvector_at(mesh.vertices, i);
                                    vertex->x *= gui_settings_object_valbox_scale_val;
                                    vertex->y *= gui_settings_object_valbox_scale_val;
                                    vertex->z *= gui_settings_object_valbox_scale_val;
                                }
                            }

                            if ( GuiButton(gui_settings_object_btn_make_scale_down, "Scale Down") && mesh.is_loaded ) {
                                size_t vert_count = cvector_size(mesh.vertices);
                                float scale_factor = 1 / gui_settings_object_valbox_scale_val;

                                for (size_t i = 0; i < vert_count; ++i) {
                                    Vector3* vertex = cvector_at(mesh.vertices, i);
                                    vertex->x *= scale_factor;
                                    vertex->y *= scale_factor;
                                    vertex->z *= scale_factor;
                                }
                            }
                        }
                        break;
                    default: break;
                }
            }

        }
        EndDrawing();

    }

    // Deinitialize anything here
    CloseWindow();
    paws_mesh_dtor(&mesh);
    cvector_delete(focus_points);
    free(mesh_filepath);

    return status;
}
