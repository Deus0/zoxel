// The inspector lists an entity: its name, parent, children, then a row per id (component, tag, pair)
//  - every row targets the inspected entity (Target), and its id (ComponentTarget)
//  - inspector_label_system keeps the component rows' values up to date

typedef struct {
    char* text;         // freed after spawning
    entity component;   // 0 for the name, parent and children rows
    byte type;
} InspectorRow;

static inline color blend_color(color a, color b, float m) {
    return (color) {
        (byte) (a.r * m + (1.0f - m) * b.r),
        (byte) (a.g * m + (1.0f - m) * b.g),
        (byte) (a.b * m + (1.0f - m) * b.b),
        (byte) (a.a * m + (1.0f - m) * b.a) };
}

// a hint of hue on the theme's outline, low contrast
static inline color inspector_row_tint(color hue) {
    return blend_color(hue, editor_button_outline, 0.3f);
}

// row outlines are colour coded by type, subtly (the row's tooltip names its type)
color get_inspector_row_outline(byte type) {
    switch (type) {
        case zox_type_name:
        case zox_type_parent:
            return editor_button_outline;
        case zox_type_tag:
            return inspector_row_tint(color_teal);
        case zox_type_byte:
        case zox_type_byte2:
        case zox_type_byte3:
            return inspector_row_tint(color_red);
        case zox_type_int:
        case zox_type_int2:
        case zox_type_int3:
            return inspector_row_tint(color_blue);
        case zox_type_uint:
        case zox_type_uint2:
            return inspector_row_tint(color_green);
        case zox_type_float:
        case zox_type_float2:
        case zox_type_float3:
        case zox_type_float4:
        case zox_type_double:
        case zox_type_double2:
        case zox_type_double3:
        case zox_type_lint:
            return inspector_row_tint(color_white);
        case zox_type_color:
        case zox_type_color_rgb:
            return inspector_row_tint(color_yellow);
        case zox_type_entity:
        case zox_type_entities:
            return inspector_row_tint(color_purple);
        default:
            return editor_button_outline;
    }
}

// fills rows for target (3 + its ids count of them), returns how many
int get_inspector_rows(
    ecs* world,
    entity target,
    InspectorRow* rows,
    int max)
{
    int count = 0;
    if (!zox_valid(target) || max < 3) {
        return 0;
    }
    char* name = malloc(inspector_component_size_buffer);
    get_editor_entity_label(world, target, name, inspector_component_size_buffer);
    rows[count++] = (InspectorRow) { .text = name, .type = zox_type_name };
    const entity parent = zox_get_parent(world, target);
    char* parent_label = malloc(inspector_component_size_buffer);
    if (zox_valid(parent)) {
        get_editor_entity_label(world, parent, parent_label, inspector_component_size_buffer);
    } else {
        snprintf(parent_label, inspector_component_size_buffer, "None");
    }
    rows[count++] = (InspectorRow) { .text = parent_label, .type = zox_type_parent };
    char children_label[64];
    snprintf(children_label, sizeof(children_label), "children [%u]", zox_get_children_count(world, target));
    rows[count++] = (InspectorRow) { .text = zox_copy_string(children_label), .type = zox_type_children };
    const int ids = zox_get_ids_count(world, target);
    for (int i = 0; i < ids && count < max; i++) {
        const entity id = zox_get_id_at(world, target, i);
        rows[count++] = (InspectorRow) {
            .text = fetch_component_label(world, target, id),
            .component = id,
            .type = fetch_component_type(world, target, id)
        };
    }
    return count;
}

void button_event_clicked_inspector(ecs *world, ClickEventData event) {
    const entity clicked = event.clicked;
    if (!zox_has(clicked, ComponentTarget) || !zox_has(clicked, Target) || !zox_has(clicked, ComponentType)) {
        zox_loge("Clicked [%s] is not an inspector row", zox_get_name(clicked));
        return;
    }
    const byte type = zox_getv(clicked, ComponentType);
    const entity target = zox_getv(clicked, Target);
    const entity component = zox_getv(clicked, ComponentTarget);
    // an entity component (a link) inspects the entity it points at
    if (type != zox_type_entity || !zox_valid(component)) {
        return;
    }
    const Target* link = zox_get_id(target, component);
    zox_logv("Inspector following [%s] -> [%s]", zox_get_name(component), link->value ? zox_get_name(link->value) : "None");
    inspector_select_target(world, event.clicker, link->value);
}

// a search: the row's text (name), or a component row's component name (tag)
static byte inspector_search_match(ecs* world, InspectorRow row, const char* search, byte type) {
    if (type == window_search_tag) {
        return row.component && window_search_match(zox_get_name(row.component), search);
    }
    return window_search_match(row.text, search);
}

// its search changed (the core Dirty on its list, src/ui/windows/sys/search.c): its rows respawn
void inspector_search_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity window = zox_get_parent_by_id(world, it->entities[i], zox_id(InspectorUI));
        if (window) {
            zox_add(window, InspectorDirty);
        }
    }
}

// an observer on InspectorDirty (non fragmenting, removed after)
void inspector_spawn_event(iter* it) {
    const ClickEvent on_click = (ClickEvent) { &button_event_clicked_inspector };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Target);
    zox_sys_in(FontSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Target, target);
        zox_sys_i(FontSize, font_size);
        zox_remove(e, InspectorDirty);
        const entity list = get_editor_window_list(world, e);
        if (!list) {
            continue;
        }
        zox_setv(list, ListStart, 0);
        clear_editor_list(world, list);
        const int max = 3 + zox_get_ids_count(world, target->value);
        InspectorRow* rows = malloc(sizeof(InspectorRow) * max);
        const int count = get_inspector_rows(world, target->value, rows, max);
        char search[input_field_max_length + 2];
        byte search_type;
        const byte searching = window_search_get(world, e, search, sizeof(search), &search_type) && search[0];
        int shown = 0;
        for (int j = 0; j < count; j++) {
            const InspectorRow row = rows[j];
            if (searching && !inspector_search_match(world, row, search, search_type)) {
                free(row.text);
                continue;
            }
            const entity button = spawn_editor_list_row(
                world,
                list,
                prefab_button,
                row.text,
                font_size->value,
                get_inspector_row_outline(row.type),
                shown++, 0);
            set_TooltipText(world, button, row.text);
            zox_setv(button, TooltipEvent, &inspector_label_tooltip);
            zox_add(button, InspectorLabel);
            zox_setv(button, ComponentType, row.type);
            zox_setv(button, Target, target->value);
            zox_setv(button, ComponentTarget, row.component);
            zox_setv(button, ClickEvent, on_click.value);
            free(row.text);
        }
        free(rows);
        refresh_editor_list(world, list);
        zox_logv("Inspector [%s] refreshed with [%i] rows", zox_getn(target->value), count);
    }
}
