entity big_old_particle_zone = 0;

void zox_dbg_spawn_particle_emitter(ecs *world, int32_t keycode) {
    if (keycode != zox_key_l) {
        return;
    }

    // max should be around 10k
    if (big_old_particle_zone) {
        zox_log("- Deleting a big old particle zone");
        zox_delete(big_old_particle_zone);
        big_old_particle_zone = 0;
        return;
    }

    zox_geter_value(dbg_player, CameraLink, entity, camera);
    zox_geter_value(camera, Position3D, float3, cposition);

    int spawn_rate = 100;
    float3 bounds = float3_single(4);
    color pcolor = (color) { rand_range(0, 255), rand_range(0, 255), rand_range(0, 255), rand_range(0, 255) };
    zox_log("+ Spawning Particles [%ix%ix%ix%i", pcolor.r, pcolor.g, pcolor.b, pcolor.a);

    entity e = spawn_particle3D_emitter(world, 0, spawn_rate, float3_scale(bounds, 2), pcolor);
    zox_set(e, Position3D, { cposition });

    big_old_particle_zone = e;
}
