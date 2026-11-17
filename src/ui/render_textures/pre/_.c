#include "texture.c"
entity prefab_render_texture;
entity prefab_render_texture_screen;

void spawn_prefabs_render_textures(ecs *world) {
    prefab_render_texture =
        spawn_prefab_render_texture(
            world,
            prefab_element);
    prefab_render_texture_screen =
        spawn_prefab_render_texture(
            world,
            prefab_element);
    zox_add(prefab_render_texture_screen, RenderTextureScreen);
    // its 3D twin (a render frame in a 3D window: a mugshot, the mirror), the camera's texture on an element3
    zox_setv(prefab_render_texture, Prefab3, spawn_prefab_render_texture(world, prefab_element3_twin));
    zox_setv(prefab_render_texture_screen, AnchorSize, float2_one);
}
