#include "texture.c"
entity prefab_render_texture;
entity prefab_render_texture_screen;

void spawn_prefabs_render_textures(ecs *world) {
    prefab_render_texture = spawn_prefab_render_texture(world, prefab_element_shell);
    prefab_render_texture_screen = spawn_prefab_render_texture(world, prefab_element_shell);
    zox_add_tag(prefab_render_texture_screen, RenderTextureScreen);
    zox_prefab_set(prefab_render_texture_screen, AnchorSize, { float2_one });
}
