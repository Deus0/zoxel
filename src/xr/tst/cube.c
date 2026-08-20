// --------------------------------------------------
// Includes
// --------------------------------------------------

#include <jni.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_opengles2.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <android/log.h>
#include <stdarg.h>

// Zoxel Types / Functions
typedef unsigned char byte;
typedef struct { int x, y; } int2;
typedef struct { float x, y; } float2;
typedef struct { float x, y, z; } float3;
typedef struct { float x, y, z, w; } float4;
typedef struct { float4 x, y, z, w; } float4x4;

#include "math.c"
#include "opengl.c"

static void zox_log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    __android_log_vprint(ANDROID_LOG_INFO, "Zoxel", format, args);
    va_end(args);
}

static void zox_loge(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    __android_log_vprint(ANDROID_LOG_ERROR, "Zoxel", format, args);
    va_end(args);
}

// --------------------------------------------------
// XR
// --------------------------------------------------

#include "../fun/helpers.c"
#include "../fun/egl.c"
#include "../fun/init.c"
#include "../fun/frames.c"
#include "../fun/swapchains.c"
#include "../fun/eyes.c"
#include "../fun/input.c"
#include "../fun/time.c"

// --------------------------------------------------
// Globals
// --------------------------------------------------

static int running;
static int session_running;

// camera
static byte head_position_set = 0;
static float3 head_position;
static float3 eye_right_position;
static float4 eye_right_rotation;
static float4 eye_fov_right;
GLuint eye_right_fbo;
GLuint eye_right_rbo;
static float3 eye_left_position;
static float4 eye_left_rotation;
static float4 eye_fov_left;
GLuint eye_left_fbo;
GLuint eye_left_rbo;

// player
static float3 camera_position = { 0, 0, 0 };
static float3 camera_euler = { 0, 0, 0 };
static float4 camera_rotation = { 0, 0, 0, 1 };
static const float player_move_speed = 3.0f;
static const float player_turn_speed = 2.0f;

// rendering cubes
GLuint program;
GLuint vbo;
#define CUBE_COUNT 1024
static float cube_scale_min = 0.1f;
static float cube_scale_max = 0.369f;
static float3 cube_min = (float3) { -64, -4, -64 };
static float3 cube_max = (float3) { 64, 32, 64 };
static float3 cube_positions[CUBE_COUNT];
static float3 cube_rotations[CUBE_COUNT];
static float3 cube_scales[CUBE_COUNT];

static const float cube[] = {
    -1,-1,-1, 1,0,0, 1,-1,-1, 1,0,0, 1,1,-1, 1,0,0,
    -1,-1,-1, 1,0,0, 1,1,-1, 1,0,0, -1,1,-1, 1,0,0,
    -1,-1,1, 0,1,0, 1,1,1, 0,1,0, 1,-1,1, 0,1,0,
    -1,-1,1, 0,1,0, -1,1,1, 0,1,0, 1,1,1, 0,1,0,
    -1,-1,-1, 0,0,1, -1,1,-1, 0,0,1, -1,1,1, 0,0,1,
    -1,-1,-1, 0,0,1, -1,1,1, 0,0,1, -1,-1,1, 0,0,1,
    1,-1,-1, 1,1,0, 1,-1,1, 1,1,0, 1,1,1, 1,1,0,
    1,-1,-1, 1,1,0, 1,1,1, 1,1,0, 1,1,-1, 1,1,0,
    -1,1,-1, 0,1,1, 1,1,-1, 0,1,1, 1,1,1, 0,1,1,
    -1,1,-1, 0,1,1, 1,1,1, 0,1,1, -1,1,1, 0,1,1,
    -1,-1,-1, 1,0,1, -1,-1,1, 1,0,1, 1,-1,1, 1,0,1,
    -1,-1,-1, 1,0,1, 1,-1,1, 1,0,1, 1,-1,-1, 1,0,1
};

// --------------------------------------------------
// Cubes
// --------------------------------------------------

static void spawn_cubes(void) {
    srand((unsigned int)time(NULL));
    for (int i = 0; i < CUBE_COUNT; i++) {
        cube_positions[i] = (float3) {
            random_float(cube_min.x, cube_max.x),
            random_float(cube_min.y, cube_max.y),
            random_float(cube_min.z, cube_max.z)
        };
        cube_rotations[i] = (float3) {
            random_float(0, 6.28f),
            random_float(0, 6.28f),
            random_float(0, 6.28f)
        };
        float scale = random_float(cube_scale_min, cube_scale_max);
        cube_scales[i] = (float3) { scale, scale, scale };
    }
}

// --------------------------------------------------
// Rendering (TODO: Replace with zoxel render systems)
// --------------------------------------------------

static float3 sky = (float3) { 0.2f, 0.4f, 0.8f };

static void render_cube(
    float4x4 vp,
    float3 position,
    float3 euler,
    float3 scale,
    GLuint program,
    GLuint vbo
) {
    float4 rotation = euler_to_quaternion(euler);
    float4x4 model = prs_to_transform_matrix(
        position,
        rotation,
        scale
    );
    float4x4 mvp = matrix_multiply(
        vp,
        model
    );
    glUseProgram(program);
    glUniformMatrix4fv(
        glGetUniformLocation(program, "vp"),
        1,
        GL_FALSE,
        (const float *)&mvp
    );
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

static void render_viewport(
    float4x4 vp_matrix,
    int2 image_size,
    GLuint program,
    GLuint vbo
) {
    // actual rendering
    glViewport(0, 0, image_size.x, image_size.y);
    glEnable(GL_DEPTH_TEST);
    glClearColor(sky.x, sky.y, sky.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < CUBE_COUNT; i++) {
        cube_rotations[i].x += 0.003f;
        cube_rotations[i].y += 0.004f;
        render_cube(
            vp_matrix,
            cube_positions[i],
            cube_rotations[i],
            cube_scales[i],
            program,
            vbo
        );
    }
}

// --------------------------------------------------
// Free Roam Camera
// --------------------------------------------------


static void update_camera_rotation(float3 rotation) {
    camera_euler.x += rotation.x * player_turn_speed * delta_time;
    camera_euler.y += rotation.y * player_turn_speed * delta_time;
    camera_euler.z += rotation.z * player_turn_speed * delta_time;
    camera_rotation = euler_to_quaternion(camera_euler);
}

static void update_camera_position(float3 movement, float4 rotation) {
    float3 local_movement = movement;
    float3 forward = quaternion_rotate_vector(
        rotation,
        (float3){ 0, 0, -1 }
    );
    float3 right = quaternion_rotate_vector(
        rotation,
        (float3){ 1, 0, 0 }
    );
    float3 up = quaternion_rotate_vector(
        rotation,
        (float3){ 0, 1, 0 }
    );
    float3 world_movement = {
        right.x * local_movement.x +
        forward.x * local_movement.z +
        up.x * local_movement.y,
        right.y * local_movement.x +
        forward.y * local_movement.z +
        up.y * local_movement.y,
        right.z * local_movement.x +
        forward.z * local_movement.z +
        up.z * local_movement.y
    };
    camera_position.x += world_movement.x * player_move_speed * delta_time;
    camera_position.y += world_movement.y * player_move_speed * delta_time;
    camera_position.z += world_movement.z * player_move_speed * delta_time;
}

// --------------------------------------------------
// XR Input
// --------------------------------------------------

// TODO: Seperate input fetching from player updates for main loop
static void update_camera() {
    float2 left_stick = xr_get_left_stick();
    float2 right_stick = xr_get_right_stick();
    update_camera_rotation((float3){
        right_stick.y,
        - right_stick.x,
        0
    });
    // NOTE: Now using camera with head rotation to aim movement
    float4 movement_rotation = quaternion_multiply(
        camera_rotation,
        eye_left_rotation
    );
    update_camera_position(
        (float3) {
            left_stick.x,
            0,
            left_stick.y
        },
        movement_rotation
    );
}

// --------------------------------------------------
// XR Cameras
// --------------------------------------------------

// NOTE: Gets Eye data and applies our camera transform to it
static int xr_get_eyes() {
    XrView views[2] = {
        { .type = XR_TYPE_VIEW },
        { .type = XR_TYPE_VIEW }
    };
    if (!xr_get_views(views)) {
        return 0;
    }
    if (!head_position_set) {
        head_position = xr_get_head_position(views);
        head_position_set = 1;
    }
    xr_get_eye_matrix(
        views[0],
        &eye_left_position,
        &eye_left_rotation,
        &eye_fov_left
    );
    xr_get_eye_matrix(
        views[1],
        &eye_right_position,
        &eye_right_rotation,
        &eye_fov_right
    );
    return 1;
}

// --------------------------------------------------
// XR Render Cameras
// --------------------------------------------------

static void xr_eye_render(
    XrSwapchain xr_swapchain,
    XrSwapchainImageOpenGLESKHR* eye_images,
    float3 position,
    float4 rotation,
    float4 fov,
    int2 image_size,
    GLuint fbo,
    GLuint rbo,
    GLuint program,
    GLuint vbo)
{
    xr_eye_render_begin(
        xr_swapchain,
        eye_images,
        position,
        rotation,
        fov,
        image_size,
        fbo,
        rbo
    );
    float4x4 eye_matrix = create_view_matrix(
        position,
        rotation
    );
    // Calculates Camera Matrix
    camera_rotation = euler_to_quaternion(camera_euler);
    float4x4 camera_matrix = create_view_matrix(
        camera_position,
        camera_rotation
    );
    float4x4 view_matrix = matrix_multiply(
        eye_matrix,
        camera_matrix
    );
    // float4x4 view_matrix = eye_matrix;
    float4x4 projection = xr_create_projection_matrix(
        fov,
        0.05f,
        100.0f
    );
    float4x4 vp_matrix = matrix_multiply(
        projection,
        view_matrix
    );
    // our actual render code here
    render_viewport(
        vp_matrix,
        image_size,
        program,
        vbo);
    xr_eye_render_end(xr_swapchain);
}

// render all X eyes

static void xr_render() {
    if (!frame_state.shouldRender) {
        return;
    }
    xr_eye_render(
        xr_swapchains[0],
        images[0],
        eye_left_position,
        eye_left_rotation,
        eye_fov_left,
        xr_config_to_image_size(views_cfg[0]),
        eye_left_fbo,
        eye_left_rbo,
        program,
        vbo
    );
    xr_eye_render(
        xr_swapchains[1],
        images[1],
        eye_right_position,
        eye_right_rotation,
        eye_fov_right,
        xr_config_to_image_size(views_cfg[1]),
        eye_right_fbo,
        eye_right_rbo,
        program,
        vbo
    );
}

// --------------------------------------------------
// Main
// --------------------------------------------------

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    // Setups
    zox_log("[XR] Starting cube test");
    SDL_SetHint(SDL_HINT_ANDROID_TRAP_BACK_BUTTON, "0");
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        zox_loge("[XR] SDL_Init failed: %s", SDL_GetError());
        return 1;
    }
    if (!egl_setup()) {
        zox_loge("[XR] EGL setup failed");
        SDL_Quit();
        return 1;
    }
    if (!xr_init()) {
        zox_loge("[XR] OpenXR initialization failed");
        egl_shutdown();
        SDL_Quit();
        return 1;
    }
    if (!xr_setup()) {
        zox_loge("[XR] OpenXR setup failed");
        xr_shutdown();
        egl_shutdown();
        SDL_Quit();
        return 1;
    }
    if (!xr_input_init()) {
        zox_loge("[XR] OpenXR Input initialization failed");
        xr_shutdown();
        egl_shutdown();
        SDL_Quit();
        return 1;
    }
    // Iniitalize OpenGL Data
    program = make_program();
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(cube),
        cube,
        GL_STATIC_DRAW
    );
    initialize_camera_buffers(&eye_left_fbo, &eye_left_rbo);
    initialize_camera_buffers(&eye_right_fbo, &eye_right_rbo);
    // Iniitalize Cube Data
    spawn_cubes();
    zox_log("[XR] Cube renderer ready");
    running = 1;
    while (running) {
        // XR Events
        XrEventDataBuffer event_buffer = {
            .type = XR_TYPE_EVENT_DATA_BUFFER
        };
        XrResult result = xrPollEvent(
            xr_instance,
            &event_buffer
        );
        if (result == XR_SUCCESS) {
            XrEventDataSessionStateChanged *state =
                (XrEventDataSessionStateChanged *)&event_buffer;
            if (state->type ==
                XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED) {
                zox_log("[XR] Session state: %d", state->state);
                if (state->state == XR_SESSION_STATE_READY) {
                    result = xrBeginSession(
                        session,
                        &(XrSessionBeginInfo){
                            .type = XR_TYPE_SESSION_BEGIN_INFO,
                            .primaryViewConfigurationType =
                                XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO
                        }
                    );
                    if (XR_SUCCEEDED(result)) {
                        session_running = 1;
                        zox_log("[XR] Session started");
                    } else {
                        zox_loge("[XR] xrBeginSession failed: %d", result);
                    }
                }
                if (state->state == XR_SESSION_STATE_STOPPING) {
                    xrEndSession(session);
                    session_running = 0;
                    zox_log("[XR] Session stopped");
                }
                if (state->state == XR_SESSION_STATE_EXITING ||
                    state->state == XR_SESSION_STATE_LOSS_PENDING) {
                    running = 0;
                }
            }
        } else if (result != XR_EVENT_UNAVAILABLE) {
            zox_loge("[XR] xrPollEvent failed: %d", result);
        }
        // NOTE: Our main loop
        if (!session_running) {
            continue;
        }
        if (!xr_begin_frame()) {
            continue;
        }
        update_time();
        xr_update_input();
        update_camera();
        xr_get_eyes();
        xr_render();
        // hmmm
        xr_end_frame(
            eye_left_position,
            eye_right_position,
            eye_left_rotation,
            eye_right_rotation,
            eye_fov_left,
            eye_fov_right);
    }
    zox_log("[XR] Shutting down");
    if (session_running) {
        xrEndSession(session);
        session_running = 0;
    }
    if (space != XR_NULL_HANDLE) {
        xrDestroySpace(space);
        space = XR_NULL_HANDLE;
    }
    for (int eye = 0; eye < 2; eye++) {
        free(images[eye]);
        images[eye] = NULL;
        if (xr_swapchains[eye] != XR_NULL_HANDLE) {
            xrDestroySwapchain(xr_swapchains[eye]);
            xr_swapchains[eye] = XR_NULL_HANDLE;
        }
    }
    if (session != XR_NULL_HANDLE) {
        xrDestroySession(session);
        session = XR_NULL_HANDLE;
    }
    dispose_camera_buffers(&eye_left_fbo, &eye_left_rbo);
    dispose_camera_buffers(&eye_right_fbo, &eye_right_rbo);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(program);
    xr_shutdown();
    egl_shutdown();
    SDL_Quit();
    return 0;
}