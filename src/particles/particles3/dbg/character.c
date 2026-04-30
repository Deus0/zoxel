extern entity get_linked_character(ecs*, entity);

void toggle_character_particles(ecs *world, int32_t keycode) {
    if (keycode != zox_key_r) {
        return;
    }

    const color test_color = (color) { 0, 255, 255, 200 };
    const float test_scale = 4; // 2
    const float test_rate = 32; // 4

    entity player = dbg_player;
    if (!zox_valid(player)) return;

    entity character = get_linked_character(world, player);
    if (!character) return;
    // our logic stuffee
    // spawn particle system
    zox_muter(character, Children, children);
    const entity previous_particles = children->length > 0 && zox_has(children->value[children->length - 1], Particle3DEmitter) ? children->value[children->length - 1] : 0;
    if (previous_particles) {
        zox_log(" + removing particles from character")
        zox_delete(previous_particles)
    } else {
        zox_log(" + adding particles to character")
        // zox_geter_value(characterLink->value, Bounds3D, float3, bounds)
        entity e = spawn_particle3D_emitter(world, character, test_rate, float3_single(test_scale), test_color);

        add_to_Children(children, e);
    }
    // not linked to skill
    // zox_set(particle3D_emitter, SkillLink, { character })
}
