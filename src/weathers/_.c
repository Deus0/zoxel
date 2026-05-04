#ifndef zoxm_weathers
#define zoxm_weathers

entity prefab_skybox;
entity shader_skybox; // shaders global
entity skybox; // remove this, link to realm/game
#include "dat/settings.c"
zox_tag(Weather);
zox_tag(Skybox);
#include "shd/skybox.c"
#include "pre/_.c"
#include "fun/_.c"
#include "sys/_.c"

void spawn_shaders_weather(ecs *world) {
    shader_skybox = spawn_shader_skybox(world);
}

zox_begin_module(Weathers)
    zoxd_tag(Weather);
    zoxd_tag(Skybox);
    define_systems_weather(world);
    // hooks
    add_hook_load_shader(&spawn_shaders_weather);
    // add_hook_on_boot(spawn_weather);
    add_to_event_game_state((zox_game_event) { &game_state_weather });
    // prefabs
    spawn_prefabs_weather(world);
zox_end_module(Weathers)

#endif
