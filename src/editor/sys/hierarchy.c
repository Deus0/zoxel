// The hierarchy lists an entity tree as buttons, indented by depth, clicking one inspects it
const int hierarchy_max_line_characters = 64;
const ushort hierarchy_max_elements = 128;

typedef struct {
    entity target;
    byte depth;
    byte children;  // it has rows under it, shown or folded
} HierarchyRow;

static inline byte hierarchy_row_entity(ecs* world, entity e) {
    return zox_valid(e) && !zox_has(e, Glyph);
}

void button_event_clicked_hierarchy(
    ecs* world,
    ClickEventData event)
{
    const entity clicked = event.clicked;
    if (!zox_has(clicked, Target)) {
        zox_loge("Clicked [%s] is not a hierarchy row", zox_get_name(clicked));
        return;
    }
    inspector_select_target(world, event.clicker, zox_getv(clicked, Target));
    zox_setv(clicked, ActiveState, 1);
    zox_add(clicked, ActiveStateDirty);
}

// Adds a row and walks its unfolded children depth first, skipping glyphs.
void get_hierarchy_rows(
    ecs *world,
    entity e,
    byte depth,
    const Unfolded* unfolded,
    HierarchyRow* rows,
    int* count)
{
    if (*count >= hierarchy_max_elements || !hierarchy_row_entity(world, e)) {
        return;
    }
    HierarchyRow* row = &rows[(*count)++];
    *row = (HierarchyRow) { .target = e, .depth = depth };
    const byte open = depth == 0 || unfolded_has(unfolded, e);   // the root is always open
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            if (!hierarchy_row_entity(world, it2.entities[j])) {
                continue;
            }
            row->children = 1;
            if (open) {
                get_hierarchy_rows(world, it2.entities[j], depth + 1, unfolded, rows, count);
            }
        }
    }
}

// Rebuilds the dirty list after spawn or a fold change, keeping the scroll position.
void hierarchy_spawn_event(iter* it) {
    const ClickEvent on_click = (ClickEvent) { &button_event_clicked_hierarchy };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Unfolded);
    for (int i = 0; i < it->count; i++) {
        const entity list = it->entities[i];
        zox_sys_i(Unfolded, unfolded);
        const entity window = zox_get_parent_by_id(world, list, zox_id(HierarchyUI));
        if (!window) {
            continue;
        }
        const entity target = zox_getv(window, Target);
        const byte font_size = zox_getv(window, ElementFontSize);
        clear_editor_list(world, list);
        HierarchyRow rows[hierarchy_max_elements];
        int count = 0;
        get_hierarchy_rows(world, target, 0, unfolded, rows, &count);
        if (count >= hierarchy_max_elements) {
            zox_logw("Hierarchy of [%s] cut off at [%i] entities", zox_getn(target), hierarchy_max_elements);
        }
        const int visible = zox_getv(list, ListVisible);
        const int max_start = count > visible ? count - visible : 0;
        const int start = zox_getv(list, ListStart);
        if (start > max_start) {
            zox_setv(list, ListStart, max_start);
        }
        const entity canvas = zox_get_parent_by_id(world, window, zox_id(Canvas));
        const byte arrow_layer = zox_getv(list, Layer) + 2;
        char text[hierarchy_max_line_characters];
        for (int j = 0; j < count; j++) {
            get_editor_entity_label(world, rows[j].target, text, hierarchy_max_line_characters);
            const entity button = spawn_editor_list_row(
                world,
                list,
                prefab_button_hierarchy,
                text,
                font_size,
                editor_button_outline,
                j, font_size);
            zox_setv(button, ClickEvent, on_click.value);
            zox_setv(button, Target, rows[j].target);
            zox_setv(button, ListIndent, rows[j].depth);
            if (rows[j].children) {
                const byte open = rows[j].depth == 0 || unfolded_has(unfolded, rows[j].target);
                spawn_fold_arrow(world, canvas, button, list, rows[j].target, open, font_size, arrow_layer);
            }
        }
        refresh_editor_list(world, list);
        zox_logv("Hierarchy [%s] refreshed with [%i] rows", zox_getn(target), count);
    }
}
