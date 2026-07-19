#include "material.c"
#include "renderer_instance.c"
entity prefab_renderer_instance;
entity prefab_static_instance_mesh;

void spawn_prefabs_rendering_core(ecs *world) {
    spawn_prefab_material(world);
    prefab_renderer_instance = spawn_prefab_renderer_instance(world);
    prefab_static_instance_mesh = spawn_prefab_static_instance_mesh(world);
}
