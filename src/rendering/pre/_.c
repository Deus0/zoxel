#include "material.c"
#include "renderer_instance.c"
#include "renderer_child.c"
entity prefab_renderer_instance;
entity prefab_renderer_child;
entity prefab_static_instance_mesh;

void spawn_prefabs_rendering(ecs *world) {
    spawn_prefab_material(world);
    prefab_renderer_instance = spawn_prefab_renderer_instance(world);
    prefab_renderer_child = spawn_prefab_renderer_child(world, prefab_renderer_instance);
    prefab_static_instance_mesh = spawn_prefab_static_instance_mesh(world);
}
