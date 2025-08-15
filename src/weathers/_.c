#ifndef zoxm_weathers
#define zoxm_weathers

entity prefab_skybox;
entity shader_skybox; // shaders global
entity skybox; // remove this, link to realm/game
#include "dat/settings.c"
zox_tag(Weather);
zox_tag(Skybox);
#include "shd/skybox.c"
#include "pre/prefabs.c"
#include "fun/_.c"
#include "sys/_.c"

void spawn_shaders_weather(ecs *world) {
    shader_skybox = spawn_shader_skybox(world);
}

void on_boot_weathers(ecs* world, entity app) {
    (void) app;
    spawn_weather(world);
}

zox_begin_module(Weathers)
    zox_define_tag(Weather);
    zox_define_tag(Skybox);
    if (!headless) {
        zox_gpu_restore_system(SkyboxRestoreSystem,
            [in] rendering.MaterialGPULink,
            [in] colorz.ColorRGB,
            [in] colorz.SecondaryColorRGB,
            [none] Skybox);
        zox_system_1(SkyboxSetTimeSystem, EcsOnUpdate,
            [in] rendering.MaterialGPULink,
            [none] Skybox);
    }
    // hooks
    add_hook_load_shader(&spawn_shaders_weather);
    add_hook_on_boot(on_boot_weathers);
    add_to_event_game_state((zox_game_event) { &game_state_weather });
    // prefabs
    spawn_prefabs_weather(world);
zox_end_module(Weathers)

#endif
