extern entity get_linked_character(ecs*, entity);
entity zox_tst_character3_particles;

void tst_spawn_character3_particles(ecs *world, ClickEventData data) {

    if (zox_valid(zox_tst_character3_particles)) {
        zox_log("Cleaning Test [zox_tst_character3_particles]");
        zox_delete(zox_tst_character3_particles);
        zox_tst_character3_particles = 0;
        return;
    }

    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }

    entity character = get_linked_character(world, player);
    if (!zox_valid(character)) {
        return;
    }

    float prate = rand_range(5, 100); // 4
    color pcolor = (color) { rand_range(0, 255), rand_range(0, 255), rand_range(0, 255), rand_range(0, 255) };
    // float test_scale = 4; // 2
    zox_geter_value(character, Bounds3D, float3, bounds);
    bounds = float3_mulf(bounds, 2.0f);

    zox_log("+ Testing Character Particles [%s]", zox_get_name(character));
    // zox_geter_value(characterLink->value, Bounds3D, float3, bounds)
    entity e = spawn_particle3D_emitter(world, character, prate, bounds, pcolor);

    zox_set_parent(world, e, character);
    zox_tst_character3_particles = e;
}
