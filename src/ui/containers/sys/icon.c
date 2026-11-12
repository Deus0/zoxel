// NOTE: When Data dirty, set the icon texture
// TODO: Use SlotLink instead of DataLink, more accurate
// NOTE: If slots data is dirty, we can also update texture

// NOTE: Slots whose data is deleted are refreshed by on_unlink_DataLink

// NOTE: Flags dirty if slot data is dirty
void data_icon_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    // zox_sys_in(SlotLink);
    // zox_sys_out(DataDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        // zox_sys_i(SlotLink, slot);
        // zox_sys_o(DataDirty, state);
        entity slot = zox_get_link(world, e, SlotLink);
        if (!zox_valid(slot)) {
            continue;
        }
        if (zox_has(slot, DataUpdate)) {
            zox_set_link(world, e, DataLink, zox_get_link(world, slot, DataLink));
            // state->value = zox_dirty_trigger;
            zox_add(e, DataDirty);
            // zox_log("Slot was dirty [%s]", zox_get_name(slot->value));
            if (dbg_log) {
                zox_log("Data UI - Slot UI was Dirty [%s]",
                    zox_sys_e_name);
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(data_icon_system);

// NOTE: Updates icon texture when dirty
// NOTE: Also updates the Tooltip to clear it
void data_icon_update_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    //zox_sys_in(DataDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        //zox_sys_i(DataDirty, state);
        entity data = zox_get_link(world, e, DataLink);
        /*if (state->value != zox_dirty_active) {
            continue;
        }*/
        // set texture of dirty
        entity texture = zox_valid(data) ?
            zox_get_link(world, data, TextureLink) :
            0;
            // zox_has(data, TextureLink) ? zox_getv(data, TextureLink) : 0;
        if (!texture) {
            if (zox_valid(data)) {
                texture = string_hashmap_get(files_hashmap_textures, new_string_data("blank"));
            } else {
                texture = string_hashmap_get(files_hashmap_textures, new_string_data("empty"));
            }
        }
        clone_texture_data(world, e, texture);
        // zox_log("Set Icon [%s] to texture [%s]", zox_get_name(e), zox_get_name(texture));if (!clicked_data_empty) {
        // For now lets do this
        /*entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (zox_valid(canvas)) {
            entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
            if (zox_valid(tooltip)) {
                set_entity_text(world, tooltip, "");
            }
        }*/
        if (dbg_log) {
            zox_log("Data Icon Updated [%s]",
                zox_sys_e_name);
        }
    }
    zox_sys_on_end();
} zoxd_system(data_icon_update_system);
