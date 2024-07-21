#include "../paws_data.h"
#include "../raygui_impl/raygui.h"
#include <raylib.h>
#include <stdio.h>

#define APP_TITLE "Kitty Paws"

int main() {
    // Initialize anything here
    int status = 0;

    int screen_width = 900;
    int screen_height = 900;
    int target_fps = 60;

    // hardcoded value for testing
    char* mesh_filepath = "/home/konnor/code/c/graphics/3d_objects/cube/cube.obj";
    paws_mesh mesh = {0};

    if ( paws_mesh_ctor(&mesh) ) {
        status = 1;
        fprintf(stderr, "[ERROR] Can't Allocate memory for mesh\n");
    }

    if ( parse_format_obj(mesh_filepath, &mesh) ) {
        // error placeholdor
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

    // Inside settings window

    // SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, APP_TITLE);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    SetTargetFPS(target_fps);

    while ( !status && !WindowShouldClose() ) {
        // Update anything here ------------------
        if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
             ( !is_show_settings_window || ( is_show_settings_window && !CheckCollisionPointRec(GetMousePosition(), gui_rect_settings_window) ) )
        ) {
            UpdateCamera(&camera, CAMERA_THIRD_PERSON);
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
            }
            EndMode3D();

            //Draw GUI here ------------------
            if ( !is_show_settings_window && GuiButton(gui_rect_settings_button, "Settings") ) {
                is_show_settings_window = !is_show_settings_window;
            }

            if ( is_show_settings_window ) {
                if ( GuiWindowBox(gui_rect_settings_window, "Settings") ) {
                    is_show_settings_window = false;
                }

                enum _settings_mode {
                    VIEW = 0,
                    // camera,
                    // render
                };

                const char* settings_window_mode_text[] = {
                    "View",
                    "Camera",
                };

                int settings_mode = VIEW;

                Rectangle gui_settings_window_view_btn = {
                    .x = gui_rect_settings_window.x + 5,
                    .y = gui_rect_settings_window.y + 30,
                    .width = 90,
                    .height = 30
                };
                Rectangle gui_settings_line = {
                    .x = gui_rect_settings_window.x,
                    .y = gui_settings_window_view_btn.y + 50,
                    .width = gui_rect_settings_window.width,
                    .height = 0
                };

                // Inside view settings
                // Point type area
                Rectangle gui_settings_view_group_box_point_type = {
                    .x = gui_rect_settings_window.x + 5,
                    .y = gui_settings_line.y + 25,
                    .width = 110,
                    .height = (3 * 35) + 15, // 3 buttons each with height 30
                };
                Rectangle gui_settings_view_btn_none = {
                    .x = gui_rect_settings_window.x + 15,
                    .y = gui_settings_view_group_box_point_type.y + 15,
                    .width = 90,
                    .height = 30
                };
                Rectangle gui_settings_view_btn_sphere = {
                    .x = gui_settings_view_btn_none.x,
                    .y = gui_settings_view_btn_none.y + 35,
                    .width = 90,
                    .height = 30
                };
                Rectangle gui_settings_view_btn_cube = {
                    .x = gui_settings_view_btn_sphere.x,
                    .y = gui_settings_view_btn_sphere.y + 35,
                    .width = 90,
                    .height = 30
                };

                // Point radius area
                Rectangle gui_settings_view_group_box_point_radius = {
                    .x = gui_rect_settings_window.x + 130,
                    .y = gui_settings_line.y + 25,
                    .width = 170,
                    .height = 45
                };
                Rectangle gui_settings_view_slider_point_radius = {
                    .x = gui_settings_view_group_box_point_radius.x + 40,
                    .y = gui_settings_view_group_box_point_radius.y + 10,
                    .width = 100,
                    .height = 30
                };

                if ( GuiButton(gui_settings_window_view_btn, "View") ) {
                    settings_mode = VIEW;
                }

                GuiLine(gui_settings_line, settings_window_mode_text[settings_mode]);

                switch ( settings_mode ) {
                    case VIEW:
                        {
                            GuiGroupBox(gui_settings_view_group_box_point_type, "Point type");

                            // Change point type
                            if ( GuiButton(gui_settings_view_btn_none, "None") ) {
                                mesh.settings.point_type = NONE;
                            }
                            if ( GuiButton(gui_settings_view_btn_sphere, "Sphere") ) {
                                mesh.settings.point_type = SPHERE;
                            }
                            if ( GuiButton(gui_settings_view_btn_cube, "Cube") ) {
                                mesh.settings.point_type = CUBE;
                            }

                            // Change point radius
                            float gui_settings_point_radius_min = 0.01f, gui_settings_point_radius_max = 0.2f;
                            GuiGroupBox(gui_settings_view_group_box_point_radius, "Point Radius");
                            GuiSlider(gui_settings_view_slider_point_radius, "0.01", "0.2", &mesh.settings.point_radius, gui_settings_point_radius_min, gui_settings_point_radius_max);
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

    return status;
}
