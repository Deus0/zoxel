
// NOTE: Confusing AF atm, nodegraphs use a set size, 32 atm, however the vox models spawn at any size, and fill gets scaled to those
//      - so we have two sizes created per model

zox_sys2(BodysRealmSpawnSystem) {
    byte nodegraph_vlength = powers_of_two[nodegraph_max_depth];
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

        byte mdepth = block_vox_depth + 2;

        if (mdepth >= model_lods_max_length) {
            zox_logw("[BodysRealmSpawnSystem] Does not support depth [%i] max is [%i]", mdepth, model_lods_max_length);
            continue;
        }

        zox_log("mdepth in body parts gen [%i]", mdepth);

        byte variants = 1;
        byte vlength = powers_of_two_byte[mdepth];
        byte2 tsize = byte2_single(vlength);

        // Chest
        {
            byte3 bsize = (byte3) {  nodegraph_vlength / 2, (4 * nodegraph_vlength) / 10, nodegraph_vlength / 2 };
            lint mseed = 888 * (i * models->length);
            byte3 msize = (byte3) { vlength / 2, (4 * vlength) / 10, vlength / 2 };

            entity mblueprint = spawn_blueprint_chest(world, bsize);
            entity2 spawn = spawn_body_model_item(world, variants, mdepth, msize, mblueprint, "chest", mseed, tsize, zox_slot_core);

            add_to_NodegraphLinks(graphs, mblueprint);
            add_to_ItemLinks(items, spawn.x);
            add_to_ModelLinks(models, spawn.y);
        }

        // Head
        {
            byte3 bsize = byte3_single(1 + nodegraph_vlength / 4);
            lint mseed = 888 * (i * models->length);
            byte3 msize = byte3_single(1 + vlength / 4);

            entity mblueprint = spawn_blueprint_head(world, bsize);
            entity2 spawn = spawn_body_model_item(world, variants, mdepth, msize, mblueprint, "head", mseed, tsize, zox_slot_head);

            add_to_NodegraphLinks(graphs, mblueprint);
            add_to_ItemLinks(items, spawn.x);
            add_to_ModelLinks(models, spawn.y);
        }

        // Hips
        {
            byte3 bsize = byte3_single(1 + nodegraph_vlength / 4);
            lint mseed = 888 * (i * models->length);
            byte3 msize = byte3_single(1 + vlength / 4);
            msize.y /= 2;

            entity mblueprint = spawn_blueprint_hips(world, bsize);
            entity2 spawn = spawn_body_model_item(world, variants, mdepth, msize, mblueprint, "hips", mseed, tsize, zox_slot_hips);

            add_to_NodegraphLinks(graphs, mblueprint);
            add_to_ItemLinks(items, spawn.x);
            add_to_ModelLinks(models, spawn.y);
        }

        zox_logv("At [%f] Realm [bodys] [%i] spawned.", zox_current_time, items->length);
    }
} zox_sys_end(BodysRealmSpawnSystem);
