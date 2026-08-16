#include "texture.c"
entity prefab_render_texture;
entity prefab_render_texture_screen;

void spawn_prefabs_render_textures(ecs *world) {
    prefab_render_texture = spawn_prefab_render_texture(world, prefab_element);
    {
        prefab_render_texture_screen = spawn_prefab_render_texture(world, prefab_element);
        zox_add(prefab_render_texture_screen, RenderTextureScreen);
        zox_setv(prefab_render_texture_screen, AnchorSize, float2_one);
    }
}
