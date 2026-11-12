// Right clicking item slots (on press, like dragging)
//  - mouse empty, slot has a stack: picks up half of it (rounded down)
//  - mouse holds a stack, slot is empty: places one item
//  - mouse holds a stack, slot has the same item: stacks one more onto it
//  - only plain item slots (not equipment, body parts or the action bar)
extern byte get_item_quantity(ecs*, entity);
extern entity split_item_stack(ecs*, entity);
extern entity take_one_item(ecs*, entity);
extern byte add_one_item(ecs*, entity, entity);

// sets an icon's texture to its data's, like data_icon_update_system (the mouse icon isn't DataUI)
static inline void set_data_icon_texture(
    ecs* world,
    entity icon,
    entity data)
{
    entity texture = zox_valid(data) ? zox_get_link(world, data, TextureLink) : 0;
    if (!texture) {
        texture = string_hashmap_get(files_hashmap_textures, new_string_data(zox_valid(data) ? "blank" : "empty"));
    }
    clone_texture_data(world, icon, texture);
}

static inline void set_mouse_data(
    ecs* world,
    entity mouse_ui,
    entity data)
{
    zox_set_link(world, mouse_ui, DataLink, data);
    zox_setv(mouse_ui, RenderDisabled, !zox_valid(data));
    if (zox_valid(data)) {
        set_data_icon_texture(world, mouse_ui, data);
    }
}

void data_ui_right_click_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity slot = zox_get_link(world, e, SlotLink);
        const entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        const entity mouse_ui = zox_valid(canvas) ? zox_get_child_by_id(world, canvas, zox_id(DataMouse)) : 0;
        if (!zox_valid(slot) || !zox_valid(mouse_ui)) {
            continue;
        }
        const entity frame = zox_get_parent(world, e);
        if (is_action_frame(world, frame) ||
            is_frame_equip_item(world, frame) ||
            is_frame_body_part(world, frame))
        {
            continue;
        }
        const entity data = zox_get_link(world, e, DataLink);
        const entity mouse_data = zox_get_link(world, mouse_ui, DataLink);
        if (!zox_valid(mouse_data)) {
            // picks up half the stack
            const entity half = zox_valid(data) ? split_item_stack(world, data) : 0;
            if (!half) {
                continue;
            }
            set_mouse_data(world, mouse_ui, half);
        } else {
            if (!can_place_icon_in_item_frame(world, frame, mouse_data)) {
                continue;
            }
            if (!zox_valid(data)) {
                // places one in the empty slot
                const entity one = take_one_item(world, mouse_data);
                if (!one) {
                    continue;
                }
                zox_set_link(world, slot, DataLink, one);
                zox_set_link(world, e, DataLink, one);
            } else if (!add_one_item(world, data, mouse_data)) {
                continue;
            }
            // the mouse's stack deletes itself once it's empty
            if (!get_item_quantity(world, mouse_data)) {
                set_mouse_data(world, mouse_ui, 0);
            }
        }
        // slot dirty, so its icon and quantity label update
        zox_add(slot, DataDirty);
        zox_add(e, DataDirty);
    }
    zox_sys_on_end();
} zoxd_system(data_ui_right_click_system);
