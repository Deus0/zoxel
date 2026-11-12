// The hierarchy lists an entity tree as buttons, indented by depth, clicking one inspects it
const int hierarchy_max_line_characters = 64;
const ushort hierarchy_max_elements = 128;

typedef struct {
    entity target;
    byte depth;
} HierarchyRow;

void button_event_clicked_hierarchy(
    ecs* world,
    ClickEventData event)
{
    const entity clicked = event.clicked;
    if (!zox_has(clicked, EntityTarget)) {
        zox_loge("Clicked [%s] is not a hierarchy row", zox_get_name(clicked));
        return;
    }
    inspector_select_target(world, event.clicker, zox_getv(clicked, EntityTarget));
    zox_setv(clicked, ActiveState, 1);
    zox_setv(clicked, ActiveStateDirty, zox_dirty_trigger);
}

// adds e and its descendants (depth first), skipping glyphs, up to hierarchy_max_elements
void get_hierarchy_rows(
    ecs *world,
    entity e,
    byte depth,
    HierarchyRow* rows,
    int* count)
{
    if (*count >= hierarchy_max_elements || !zox_valid(e) || zox_has(e, Glyph)) {
        return;
    }
    rows[(*count)++] = (HierarchyRow) { .target = e, .depth = depth };
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            get_hierarchy_rows(world, it2.entities[j], depth + 1, rows, count);
        }
    }
}

void hierarchy_spawn_system(iter* it) {
    zox_sys_on_begin();
    const ClickEvent on_click = (ClickEvent) { &button_event_clicked_hierarchy };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(HierarchyUIDirty);
    zox_sys_in(EntityTarget);
    zox_sys_in(ElementFontSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(HierarchyUIDirty, dirty);
        zox_sys_i(EntityTarget, target);
        zox_sys_i(ElementFontSize, font_size);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        const entity list = get_editor_window_list(world, e);
        if (!list) {
            continue;
        }
        clear_editor_list(world, list);
        HierarchyRow rows[hierarchy_max_elements];
        int count = 0;
        get_hierarchy_rows(world, target->value, 0, rows, &count);
        if (count >= hierarchy_max_elements) {
            zox_logw("Hierarchy of [%s] cut off at [%i] entities", zox_getn(target->value), hierarchy_max_elements);
        }
        char text[hierarchy_max_line_characters];
        for (int j = 0; j < count; j++) {
            get_editor_entity_label(world, rows[j].target, text, hierarchy_max_line_characters);
            const entity button = spawn_editor_list_row(
                world,
                list,
                prefab_button_hierarchy,
                text,
                font_size->value,
                editor_button_outline,
                j);
            zox_setv(button, ClickEvent, on_click.value);
            zox_setv(button, EntityTarget, rows[j].target);
            zox_setv(button, ListIndent, rows[j].depth);
        }
        refresh_editor_list(world, list);
        zox_logv("Hierarchy [%s] refreshed with [%i] rows", zox_getn(target->value), count);
    }
    zox_sys_on_end();
} zoxd_system(hierarchy_spawn_system);
