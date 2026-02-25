entity item_get_max_depth_vox(ecs* world, entity part) {
    if (!zox_has(part, ModelLink)) {
        zox_log_error("Part has no Model Link [%s]", zox_get_name(part));
        return 0;
    }

    zox_geter_value_non_const(part, ModelLink, entity, vox);

    if (!zox_valid(vox)) {
        zox_log_error("[player body]: Invalid Part %s Model", zox_get_name(part));
        return 0;
    }

    if (!zox_has(vox, MaxRenderDepth)) {
        zox_log_error("Part Vox Invalid Components %s Model %s", zox_get_name(part), zox_get_name(vox));
        return 0;
    }

    zox_geter_value(vox, MaxRenderDepth, byte, max_render_depth);

    if (zox_valid(vox) && zox_has(vox, ModelLods)) {
        zox_geter(vox, ModelLods, mlods);

        entity vox_lod = mlods->value[max_render_depth]; // rdepth->value];

        if (zox_valid(vox_lod)) {
            vox = vox_lod;
        } else {
            zox_log_error("[player body]: Invalid Vox Model Lod [%s] Depth [%i]", zox_get_name(vox), max_render_depth);
        }
    }

    if (!zox_valid(vox)) {
        zox_logw("Part [%s] has invalid Vox", zox_get_name(part));
    }

    return vox;
}

// for now just set vox, later we spawn item and set it from BodyDirty
zox_sys2(BodyCombineSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BodyDirty);
    zox_sys_in(BodyLinks);
    zox_sys_out(CombineList);
    zox_sys_out(CombinePositions);
    zox_sys_out(CombineVox);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BodyDirty, state);
        zox_sys_i(BodyLinks, bodys);
        zox_sys_o(CombineList, voxes);
        zox_sys_o(CombinePositions, positions);
        zox_sys_o(CombineVox, output);

        if (state->value != zox_dirty_active) {
            continue;
        }

        if (!bodys->length) {
            // zox_logw("No Body to Generate");
            continue;
        }

        // Clear combine data
        resize_CombineList(voxes, 0);
        resize_CombinePositions(positions, 0);

        // TODO: Generate this from item slot data
        // head size 8^3
        // chest size x: 10, y: 25, z: 10
        byte mul = 1;
        if (block_vox_depth == 5) mul = 2;

        // atm its based on the realm body items
        byte3 vpositions[] = {
            (byte3) { 0, 0, 0 },
            (byte3) { 4 * mul, 21 * mul, 4 * mul },
        };

#ifndef zox_debug_head_only
        for (int j = 0; j < bodys->length; j++) {
#else
        for (int j = 1; j < bodys->length; j++) {
#endif
            entity item = bodys->value[j];
            entity vox = item_get_max_depth_vox(world, item);
#ifndef zox_debug_head_only
            byte3 vposition = vpositions[j];
#else
            byte3 vposition = byte3_zero;
#endif

            add_to_CombineList(voxes, vox);
            add_to_CombinePositions(positions, vposition);
        }

        output->value = zox_dirty_trigger;
    }
} zox_sys_end(BodyCombineSystem);

// zox_set(e, ModelLink, { vox });
// zox_geter_value(vox, MaxRenderDepth, byte, max_render_depth);
// zox_set(e, MaxRenderDepth, { max_render_depth });;

// Temporarily clone it there
/*byte body_index = zox_slot_core; // zox_slot_head | zox_slot_core;
entity part = body_index < bodys->length ? bodys->value[body_index] : 0;

if (!zox_valid(part)) {
    zox_log_error("Player has invalid body part");
    continue;
}

entity vox = item_get_max_depth_vox(world, part);

if (!zox_valid(vox)) {
    zox_logw("{Invalid Part Vox} [%s]", zox_get_name(e));
    continue;
}

if (!zox_has(vox, MaxRenderDepth)) {
    zox_logw("Entity [%s]'s Vox [%s] has no Max Render Depth", zox_get_name(e), zox_get_name(vox));
    continue;
}

// zox_set(e, CloneVox, { 1 });
// zox_set(e, CloneVoxLink, { vox });

zox_log("[player body]: Valid Vox Model Lod [%s] Depth [%i]", zox_get_name(vox), rdepth->value);*/
