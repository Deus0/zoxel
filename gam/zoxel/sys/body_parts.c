// NOTE: Confusing AF atm, nodegraphs use a set size, 32 atm, however the vox models spawn at any size, and fill gets scaled to those
//      - so we have two sizes created per model
// color skin_color = (color) { 200, 155, 133, 255 };
// color eye_color = (color) { 55, 200, 99, 255 };
// TODO: Spawn the slots onto the items themselves
zox_sys2(BodysRealmSpawnSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_out(ModelLinks);
    zox_sys_out(ItemLinks);
    zox_sys_out(NodegraphLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_o(ModelLinks, models);
        zox_sys_o(ItemLinks, items);
        zox_sys_o(NodegraphLinks, graphs);
        if (state->value != zox_generate_realm_items) {
            continue;
        }
        // byte variants = 1;
        byte max_part_depth = block_vox_depth + 2;
        byte blueprint_depth = nodegraph_max_depth;
        byte blueprint_length = powers_of_two[blueprint_depth];
        if (max_part_depth >= model_lods_max_length) {
            zox_logw("[BodysRealmSpawnSystem] Does not support depth [%i] max is [%i]", max_part_depth, model_lods_max_length);
            continue;
        }
        float3 skin_hsv = (float3) {
            frand_range(0, 360),
            frand_range(14, 34),
            frand_range(24, 44)
        };
        color skin_color = hsv_to_color(skin_hsv);
        float3 eye_hsv = (float3) {
            ((int)skin_hsv.x + 180) % 360, // frand_range(0, 360),
            frand_range(44, 64),
            frand_range(40, 72)
        };
        color eye_color = hsv_to_color(eye_hsv);
        // Chest
        {
            float3 blueprint_scale = (float3) { 0.5f, 0.54f, 0.56f };
            // Our Chest Blueprint involving Shapes + other nodes
            byte3 blueprint_size = byte3_scale3f(byte3_single(blueprint_length), blueprint_scale);
            entity blueprint = spawn_blueprint_chest(world, blueprint_size, blueprint_depth, skin_color);
            add_to_NodegraphLinks(graphs, blueprint);
            // Model Data
            lint seed = 888 * (i * models->length);
            byte model_depth = max_part_depth - 1 < 0 ? 0 : max_part_depth - 1;
            entity2 e2 = spawn_realm_body_part2(world, e, seed, model_depth, "Chest", zox_slot_core, blueprint, blueprint_scale, dbg_log);
            add_to_ItemLinks(items, e2.x);
            add_to_ModelLinks(models, e2.y);
        }
        // Head
        {
            float3 blueprint_scale = float3_single(0.88f);
            lint seed = 16663 * (i * models->length);
            byte model_depth = max_part_depth - 2 < 0 ? 0 : max_part_depth - 2;
            // Spawn Blueprint
            byte3 blueprint_size = byte3_scale3f(byte3_single(blueprint_length), blueprint_scale);
            entity blueprint = spawn_blueprint_head(world, blueprint_size, blueprint_depth, skin_color, eye_color);
            add_to_NodegraphLinks(graphs, blueprint);
            // Model Data
            entity2 e2 = spawn_realm_body_part2(world, e, seed, model_depth, "Head", zox_slot_head, blueprint, blueprint_scale, dbg_log);
            add_to_ItemLinks(items, e2.x);
            add_to_ModelLinks(models, e2.y);
            zox_add_tag(e2.x, Head);
        }
        // Hips
        {
            float3 blueprint_scale = (float3) { 0.92f, 0.4f, 0.6f };
            lint seed = 3363 * (i * models->length);
            byte model_depth = max_part_depth - 2 < 0 ? 0 : max_part_depth - 2;
            // Spawn Blueprint
            byte3 blueprint_size = byte3_scale3f(byte3_single(blueprint_length), blueprint_scale);
            entity blueprint = spawn_blueprint_hips(world, blueprint_size, blueprint_depth, skin_color);
            add_to_NodegraphLinks(graphs, blueprint);
            // Spawn Model Item
            entity2 e2 = spawn_realm_body_part2(world, e, seed, model_depth, "Hips", zox_slot_hips, blueprint, blueprint_scale, dbg_log);
            add_to_ItemLinks(items, e2.x);
            add_to_ModelLinks(models, e2.y);
        }
        // Thigh
        {
            float3 blueprint_scale = (float3) { 0.42f, 0.76f, 0.42f };
            lint seed = 3363 * (i * models->length);
            byte model_depth = max_part_depth - 2 < 0 ? 0 : max_part_depth - 2;
            // Spawn Blueprint
            byte3 blueprint_size = byte3_scale3f(byte3_single(blueprint_length), blueprint_scale);
            entity blueprint = spawn_blueprint_flesh_part(world, blueprint_size, blueprint_depth, skin_color);
            add_to_NodegraphLinks(graphs, blueprint);
            // Spawn Model Item
            entity2 e2 = spawn_realm_body_part2(world, e, seed, model_depth, "Thigh", zox_slot_lthigh, blueprint, blueprint_scale, dbg_log);
            add_to_ItemLinks(items, e2.x);
            add_to_ModelLinks(models, e2.y);
        }
        // Calf
        {
            float3 blueprint_scale = (float3) { 0.34f, 0.92f, 0.34f };
            lint seed = 3363 * (i * models->length);
            byte model_depth = max_part_depth - 2 < 0 ? 0 : max_part_depth - 2;
            // Spawn Blueprint
            byte3 blueprint_size = byte3_scale3f(byte3_single(blueprint_length), blueprint_scale);
            entity blueprint = spawn_blueprint_flesh_part(world, blueprint_size, blueprint_depth, skin_color);
            add_to_NodegraphLinks(graphs, blueprint);
            // Spawn Model Item
            entity2 e2 = spawn_realm_body_part2(world, e, seed, model_depth, "Calf", zox_slot_lcalf, blueprint, blueprint_scale, dbg_log);
            add_to_ItemLinks(items, e2.x);
            add_to_ModelLinks(models, e2.y);
        }
        // Feet
        {
            float3 blueprint_scale = (float3) { 0.36f, 0.42f, 0.86f };
            lint seed = 3363 * (i * models->length);
            byte model_depth = max_part_depth - 2 < 0 ? 0 : max_part_depth - 2;
            // Spawn Blueprint
            byte3 blueprint_size = byte3_scale3f(byte3_single(blueprint_length), blueprint_scale);
            entity blueprint = spawn_blueprint_flesh_part(world, blueprint_size, blueprint_depth, skin_color);
            add_to_NodegraphLinks(graphs, blueprint);
            // Spawn Model Item
            entity2 e2 = spawn_realm_body_part2(world, e, seed, model_depth, "Foot", zox_slot_lfoot, blueprint, blueprint_scale, dbg_log);
            add_to_ItemLinks(items, e2.x);
            add_to_ModelLinks(models, e2.y);
        }
        // Shoulder
        {
            float3 blueprint_scale = (float3) { 0.6f, 0.4f, 0.4f };
            lint seed = 3363 * (i * models->length);
            byte model_depth = max_part_depth - 2 < 0 ? 0 : max_part_depth - 2;
            // Spawn Blueprint
            byte3 blueprint_size = byte3_scale3f(byte3_single(blueprint_length), blueprint_scale);
            entity blueprint = spawn_blueprint_flesh_part(world, blueprint_size, blueprint_depth, skin_color);
            add_to_NodegraphLinks(graphs, blueprint);
            // Spawn Model Item
            entity2 e2 = spawn_realm_body_part2(world, e, seed, model_depth, "Shoulder", zox_slot_lshoulder, blueprint, blueprint_scale, dbg_log);
            add_to_ItemLinks(items, e2.x);
            add_to_ModelLinks(models, e2.y);
        }
        // Bicep
        {
            float3 blueprint_scale = (float3) { 0.36f, 0.52f, 0.36f };
            lint seed = 3363 * (i * models->length);
            byte model_depth = max_part_depth - 2 < 0 ? 0 : max_part_depth - 2;
            // Spawn Blueprint
            byte3 blueprint_size = byte3_scale3f(byte3_single(blueprint_length), blueprint_scale);
            entity blueprint = spawn_blueprint_flesh_part(world, blueprint_size, blueprint_depth, skin_color);
            add_to_NodegraphLinks(graphs, blueprint);
            // Spawn Model Item
            entity2 e2 = spawn_realm_body_part2(world, e, seed, model_depth, "Bicep", zox_slot_lbicep, blueprint, blueprint_scale, dbg_log);
            add_to_ItemLinks(items, e2.x);
            add_to_ModelLinks(models, e2.y);
        }
        // Forearms
        {
            float3 blueprint_scale = (float3) { 0.36f, 0.62f, 0.36f };
            lint seed = 3363 * (i * models->length);
            byte model_depth = max_part_depth - 2 < 0 ? 0 : max_part_depth - 2;
            // Spawn Blueprint
            byte3 blueprint_size = byte3_scale3f(byte3_single(blueprint_length), blueprint_scale);
            entity blueprint = spawn_blueprint_flesh_part(world, blueprint_size, blueprint_depth, skin_color);
            add_to_NodegraphLinks(graphs, blueprint);
            // Spawn Model Item
            entity2 e2 = spawn_realm_body_part2(world, e, seed, model_depth, "Forearm", zox_slot_lforearm, blueprint, blueprint_scale, dbg_log);
            add_to_ItemLinks(items, e2.x);
            add_to_ModelLinks(models, e2.y);
        }
        // Hands
        {
            float3 blueprint_scale = (float3) { 0.32f, 0.42f, 0.18f };
            lint seed = 3363 * (i * models->length);
            byte model_depth = max_part_depth - 2 < 0 ? 0 : max_part_depth - 2;
            // Spawn Blueprint
            byte3 blueprint_size = byte3_scale3f(byte3_single(blueprint_length), blueprint_scale);
            entity blueprint = spawn_blueprint_flesh_part(world, blueprint_size, blueprint_depth, skin_color);
            add_to_NodegraphLinks(graphs, blueprint);
            // Spawn Model Item
            entity2 e2 = spawn_realm_body_part2(world, e, seed, model_depth, "Hand", zox_slot_lhand, blueprint, blueprint_scale, dbg_log);
            add_to_ItemLinks(items, e2.x);
            add_to_ModelLinks(models, e2.y);
        }
        float3 skin_hsv2 = (float3) {
            frand_range(0, 360),
            frand_range(5, 75),
            frand_range(5, 55)
        };
        color skin_color2 = hsv_to_color(skin_hsv2);
        float3 eye_hsv2 = (float3) {
            frand_range(0, 360),
            frand_range(44, 64),
            frand_range(40, 72)
        };
        color eye_color2 = hsv_to_color(eye_hsv2);
        // Chest2
        {
            float3 blueprint_scale = (float3) { 0.54f, 0.46f, 0.46f };
            // Our Chest Blueprint involving Shapes + other nodes
            byte3 blueprint_size = byte3_scale3f(byte3_single(blueprint_length), blueprint_scale);
            entity blueprint = spawn_blueprint_chest(world, blueprint_size, blueprint_depth, skin_color2);
            add_to_NodegraphLinks(graphs, blueprint);
            // Model Data
            lint seed = 888 * (i * models->length);
            byte model_depth = max_part_depth - 1 < 0 ? 0 : max_part_depth - 1;
            entity2 e2 = spawn_realm_body_part2(world, e, seed, model_depth, "Chest2", zox_slot_core, blueprint, blueprint_scale, dbg_log);
            add_to_ItemLinks(items, e2.x);
            add_to_ModelLinks(models, e2.y);
        }
        // Head_2
        {
            float3 blueprint_scale = float3_single(0.76f);
            lint seed = 16663 * (i * models->length);
            byte model_depth = max_part_depth - 2 < 0 ? 0 : max_part_depth - 2;
            // Spawn Blueprint
            byte3 blueprint_size = byte3_scale3f(byte3_single(blueprint_length), blueprint_scale);
            entity blueprint = spawn_blueprint_head(world, blueprint_size, blueprint_depth, skin_color2, eye_color2);
            add_to_NodegraphLinks(graphs, blueprint);
            // Model Data
            entity2 e2 = spawn_realm_body_part2(world, e, seed, model_depth, "Head2", zox_slot_head, blueprint, blueprint_scale, dbg_log);
            add_to_ItemLinks(items, e2.x);
            add_to_ModelLinks(models, e2.y);
            zox_add_tag(e2.x, Head);
        }
        zox_logv("At [%f] Realm [bodys] [%i] spawned.", zox_current_time, items->length);
    }
} zox_sys_end(BodysRealmSpawnSystem);
