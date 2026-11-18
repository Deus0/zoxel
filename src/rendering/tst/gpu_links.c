// tst: gpu_links, entities' gpu links give back what they hold (live counts, src/_/logs), no world
//  - deleting an entity lets go of its buffers and texture
//  - a gpu dispose lets go of a disabled entity's links too, a restore makes them again (a system skips disabled
//    entities: a hidden mesh kept a dead buffer through android's lost context), a prefab's stay empty
//  - run: bash bsh/tst.sh gpu_links
byte zox_tst_gpu_links_ready(ecs* world) {
    (void) world;
    zox_tst_waiting = "rendering";
    return rendering_initialized && render_backend == zox_render_backend_opengl;
}

// a mesh with uvs, colours and a texture, all on the gpu
entity zox_tst_gpu_links_spawn(ecs* world, byte disabled) {
    const entity e = zox_new();
    zox_setv(e, MeshGPULink, spawn_gpu_mesh_buffers());
    zox_setv(e, UvsGPULink, zox_gpu_create_buffer());
    zox_setv(e, ColorsGPULink, zox_gpu_create_buffer());
    zox_setv(e, TextureGPULink, spawn_gpu_texture_buffer());
    if (disabled) {
        zox_disable(e);
    }
    return e;
}

// all of an entity's links hold something (1), or all are empty (0)
byte zox_tst_gpu_links_held(ecs* world, entity e, byte held) {
    const guint2 mesh = zox_getv(e, MeshGPULink);
    const byte holds[4] = { mesh.x && mesh.y, zox_getv(e, UvsGPULink) != 0, zox_getv(e, ColorsGPULink) != 0,
        zox_getv(e, TextureGPULink) != 0 };
    for (int i = 0; i < 4; i++) {
        if (holds[i] != held) {
            return 0;
        }
    }
    return 1;
}

void zox_tst_gpu_links_check(ecs* world) {
    const int buffers = zox_gpu_buffers_live, textures = zox_gpu_textures_live;
    // deleting gives them back
    entity a = zox_tst_gpu_links_spawn(world, 0);
    zox_tst_check(zox_gpu_buffers_live == buffers + 4 && zox_gpu_textures_live == textures + 1,
        "spawned a mesh, [%i] buffers [%i] textures live, was [%i] [%i]", zox_gpu_buffers_live, zox_gpu_textures_live, buffers, textures);
    zox_delete(a);
    zox_tst_check(zox_gpu_buffers_live == buffers, "deleted, [%i] buffers live, [%i] before",
        zox_gpu_buffers_live, buffers);
    zox_tst_check(zox_gpu_textures_live == textures, "deleted, [%i] textures live, [%i] before", zox_gpu_textures_live, textures);
    // a dispose and restore, disabled ones too, not prefabs
    a = zox_tst_gpu_links_spawn(world, 0);
    const entity b = zox_tst_gpu_links_spawn(world, 1);
    entity prefab;
    {
        zox_prefab();
        zox_setv(e, MeshGPULink, guint2_zero);
        zox_setv(e, TextureGPULink, 0);
        prefab = e;
    }
    const int buffers_spawned = zox_gpu_buffers_live, textures_spawned = zox_gpu_textures_live;
    opengl_dispose_resources(world);
    zox_tst_check(zox_tst_gpu_links_held(world, a, 0), "disposed, a mesh still holds gpu links");
    zox_tst_check(zox_tst_gpu_links_held(world, b, 0), "disposed, a disabled mesh still holds gpu links");
    zox_tst_check(zox_gpu_buffers_live == 0 && zox_gpu_textures_live == 0, "disposed, [%i] buffers [%i] textures still live",
        zox_gpu_buffers_live, zox_gpu_textures_live);
    opengl_restore_resources(world);
    zox_tst_check(zox_tst_gpu_links_held(world, a, 1), "restored, a mesh's gpu links are empty");
    zox_tst_check(zox_tst_gpu_links_held(world, b, 1), "restored, a disabled mesh's gpu links are empty");
    zox_tst_check(!zox_getv(prefab, MeshGPULink).x && !zox_getv(prefab, TextureGPULink), "restored, a prefab got gpu links");
    zox_tst_check(zox_gpu_buffers_live == buffers_spawned && zox_gpu_textures_live == textures_spawned,
        "restored, [%i] buffers [%i] textures live, [%i] [%i] before", zox_gpu_buffers_live, zox_gpu_textures_live,
        buffers_spawned, textures_spawned);
    zox_enable(b);  // a disabled one's leak on delete: doc/todo.md Bugs
    zox_delete(a);
    zox_delete(b);
    zox_delete(prefab);
    zox_tst_check(zox_gpu_buffers_live == buffers && zox_gpu_textures_live == textures,
        "deleted after a restore, [%i] buffers [%i] textures live, [%i] [%i] at the start", zox_gpu_buffers_live,
        zox_gpu_textures_live, buffers, textures);
}
