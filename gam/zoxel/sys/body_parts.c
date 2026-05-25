// NOTE: Confusing AF atm, nodegraphs use a set size, 32 atm, however the vox models spawn at any size, and fill gets scaled to those
//      - so we have two sizes created per model
zox_sys2(BodysRealmSpawnSystem) {
    byte dbg_log = 1;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_out(ModelLinks);
    zox_sys_out(ItemLinks);
    zox_sys_out(NodegraphLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GenerateRealm, state);
        zox_sys_o(ModelLinks, models);
        zox_sys_o(ItemLinks, items);
        zox_sys_o(NodegraphLinks, graphs);
        if (state->value != zox_generate_realm_items) {
            continue;
        }
        byte variants = 1;
        byte max_part_depth = block_vox_depth + 2;
        byte blueprint_depth = nodegraph_max_depth;
        byte blueprint_length = powers_of_two[blueprint_depth];
        if (max_part_depth >= model_lods_max_length) {
            zox_logw("[BodysRealmSpawnSystem] Does not support depth [%i] max is [%i]", max_part_depth, model_lods_max_length);
            continue;
        }
        // Chest
        {
            float3 blueprint_scale = (float3) { 0.5f, 0.66f, 0.56f };
            byte3 blueprint_size = byte3_scale3f(byte3_single(blueprint_length), blueprint_scale);
            // Our Chest Blueprint involving Shapes + other nodes
            entity blueprint = spawn_blueprint_chest(world, blueprint_size, blueprint_depth);
            add_to_NodegraphLinks(graphs, blueprint);
            // Model Data
            lint seed = 888 * (i * models->length);
            byte model_decrease = 1;
            byte model_depth = max_part_depth - model_decrease < 0 ? 0 : max_part_depth - model_decrease;
            byte model_length = powers_of_two_byte[model_depth];
            byte3 model_size = byte3_scale3f(byte3_single(model_length), blueprint_scale);
            byte2 texture_size = byte2_single(model_length);
            // Spawn Model Item
            entity2 spawn = spawn_realm_body_part(world, variants, model_depth, model_size, blueprint, "chest", seed, texture_size, zox_slot_core);
            add_to_ItemLinks(items, spawn.x);
            add_to_ModelLinks(models, spawn.y);
            if (dbg_log) {
                zox_log("Model [Chest]:");
                zox_log("   Depth [%i] Length [%i]", model_depth, model_length);
                zox_log("   Size [%ix%ix%i]", model_size.x, model_size.y, model_size.z);
            }
        }
        // Head
        {
            float3 blueprint_scale = float3_single(0.84f);
            byte3 blueprint_size = byte3_scale3f(byte3_single(blueprint_length), blueprint_scale);
            // Spawn Blueprint
            entity blueprint = spawn_blueprint_head(world, blueprint_size, blueprint_depth);
            add_to_NodegraphLinks(graphs, blueprint);
            // Model Data
            lint seed = 16663 * (i * models->length);
            byte model_decrease = 2;
            byte model_depth = max_part_depth - model_decrease < 0 ? 0 : max_part_depth - model_decrease;
            byte model_length = powers_of_two_byte[model_depth];
            byte3 model_size = byte3_scale3f(byte3_single(model_length), blueprint_scale);
            byte2 texture_size = byte2_single(model_length);
            // Spawn Item Model
            entity2 spawn = spawn_realm_body_part(world, variants, model_depth, model_size, blueprint, "head", seed, texture_size, zox_slot_head);
            add_to_ItemLinks(items, spawn.x);
            add_to_ModelLinks(models, spawn.y);
            if (dbg_log) {
                zox_log("Model [Head]:");
                zox_log("   Depth [%i] Length [%i]", model_depth, model_length);
                zox_log("   Size [%ix%ix%i]", model_size.x, model_size.y, model_size.z);
            }
        }
        // Hips
        {
            float3 blueprint_scale = (float3) { 1, 0.46f, 0.6f };
            byte3 blueprint_size = byte3_scale3f(byte3_single(blueprint_length), blueprint_scale);
            // Spawn Blueprint
            entity blueprint = spawn_blueprint_hips(world, blueprint_size, blueprint_depth);
            add_to_NodegraphLinks(graphs, blueprint);
            // Model Data
            lint seed = 3363 * (i * models->length);
            byte model_decrease = 2;
            byte model_depth = max_part_depth - model_decrease < 0 ? 0 : max_part_depth - model_decrease;
            byte model_length = powers_of_two_byte[model_depth];
            byte3 model_size = byte3_scale3f(byte3_single(model_length), blueprint_scale);
            byte2 texture_size = byte2_single(model_length);
            // Spawn Item Model
            entity2 spawn = spawn_realm_body_part(world, variants, model_depth, model_size, blueprint, "hips", seed, texture_size, zox_slot_hips);
            add_to_ItemLinks(items, spawn.x);
            add_to_ModelLinks(models, spawn.y);
            if (dbg_log) {
                zox_log("Model [Hips]:");
                zox_log("   Depth [%i] Length [%i]", model_depth, model_length);
                zox_log("   Size [%ix%ix%i]", model_size.x, model_size.y, model_size.z);
            }
        }
        // Shoulder
        {
            float3 blueprint_scale = (float3) { 0.6f, 0.6f, 0.6f };
            byte3 blueprint_size = byte3_scale3f(byte3_single(blueprint_length), blueprint_scale);
            // Spawn Blueprint
            entity blueprint = spawn_blueprint_shoulder(world, blueprint_size, blueprint_depth);
            add_to_NodegraphLinks(graphs, blueprint);
            // Model Data
            lint seed = 3363 * (i * models->length);
            byte model_decrease = 2;
            byte model_depth = max_part_depth - model_decrease < 0 ? 0 : max_part_depth - model_decrease;
            byte model_length = powers_of_two_byte[model_depth];
            byte3 model_size = byte3_scale3f(byte3_single(model_length), blueprint_scale);
            byte2 texture_size = byte2_single(model_length);
            // Spawn Item Model
            entity2 spawn = spawn_realm_body_part(world, variants, model_depth, model_size, blueprint, "shoulder", seed, texture_size, zox_slot_lshoulder);
            add_to_ItemLinks(items, spawn.x);
            add_to_ModelLinks(models, spawn.y);
            if (dbg_log) {
                zox_log("Model [Left Shoulder]:");
                zox_log("   Depth [%i] Length [%i]", model_depth, model_length);
                zox_log("   Size [%ix%ix%i]", model_size.x, model_size.y, model_size.z);
            }
        }
        // Bicep
        {
            float3 blueprint_scale = (float3) { 0.36f, 0.6f, 0.36f };
            byte3 blueprint_size = byte3_scale3f(byte3_single(blueprint_length), blueprint_scale);
            // Spawn Blueprint
            entity blueprint = spawn_blueprint_shoulder(world, blueprint_size, blueprint_depth);
            add_to_NodegraphLinks(graphs, blueprint);
            // Model Data
            lint seed = 3363 * (i * models->length);
            byte model_decrease = 2;
            byte model_depth = max_part_depth - model_decrease < 0 ? 0 : max_part_depth - model_decrease;
            byte model_length = powers_of_two_byte[model_depth];
            byte3 model_size = byte3_scale3f(byte3_single(model_length), blueprint_scale);
            byte2 texture_size = byte2_single(model_length);
            // Spawn Item Model
            entity2 spawn = spawn_realm_body_part(world, variants, model_depth, model_size, blueprint, "bicep", seed, texture_size, zox_slot_lbicep);
            add_to_ItemLinks(items, spawn.x);
            add_to_ModelLinks(models, spawn.y);
            if (dbg_log) {
                zox_log("Model [Bicep]:");
                zox_log("   Depth [%i] Length [%i]", model_depth, model_length);
                zox_log("   Size [%ix%ix%i]", model_size.x, model_size.y, model_size.z);
            }
        }
        zox_logv("At [%f] Realm [bodys] [%i] spawned.", zox_current_time, items->length);
    }
} zox_sys_end(BodysRealmSpawnSystem);
