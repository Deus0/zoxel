enum { hierarchy_max_line_characters = 64 };

typedef struct {
    entity list;
    entity root;
    entity anchor;
    char search[input_field_max_length + 2];
    byte search_type;
    int index;
    int anchor_index;
    int start;
    entity first;
    VirtualListPage* page;
} HierarchyWalk;

// a row: not the editor's own ui, nor a glyph; a prefab only while the list shows them (its header's p toggle)
static byte hierarchy_row_entity(ecs* world, entity list, entity e) {
    return zox_valid(e) && !zox_has(e, Glyph) && !zox_has(e, EditorElement) &&
        (!zox_is_prefab(e) || zox_has(list, HierarchyPrefabs));
}

// a root of the scene, or with prefabs shown, a prefab with no parent
static byte hierarchy_root_entity(ecs* world, entity list, entity e) {
    return zox_is_scene_root(world, e) ||
        (zox_has(list, HierarchyPrefabs) && zox_is_prefab(e) && !zox_get_parent(world, e));
}

static byte hierarchy_search_match(ecs* world, entity e, const char* search, byte type) {
    if (type == window_search_tag) {
        const int ids = zox_get_ids_count(world, e);
        for (int i = 0; i < ids; i++) {
            if (window_search_match(zox_get_name(zox_get_id_at(world, e, i)), search)) {
                return 1;
            }
        }
        return 0;
    }
    char label[hierarchy_max_line_characters];
    get_editor_entity_label(world, e, label, sizeof(label));
    return window_search_match(label, search);
}

// The same depth-first order supplies the count and the page. Search descends through folds.
static byte hierarchy_walk_tree(ecs* world, HierarchyWalk* walk, entity e, int depth) {
    if (!hierarchy_row_entity(world, walk->list, e)) {
        return 0;
    }
    const byte searching = walk->search[0] != 0;
    if (!searching || hierarchy_search_match(world, e, walk->search, walk->search_type)) {
        const int index = walk->index++;
        if (e == walk->anchor) {
            walk->anchor_index = index;
        }
        if (walk->page && index >= walk->start) {
            if (!walk->first) {
                walk->first = e;
            }
            const entity row = walk->page->slots[walk->page->filled];
            if (zox_has(row, ActiveState) && zox_getv(row, ActiveState) && e != zox_get_link(world, row, RowTargetLink)) {
                zox_setv(row, ActiveState, 0);
                zox_add(row, ActiveStateDirty);
            }
            zox_setv(row, ListIndent, searching ? 0 : int_min(depth, UINT8_MAX));
            virtual_list_bind(world, walk->page, e);
            if (walk->page->filled == walk->page->capacity) {
                return 1;
            }
        }
    }
    if (searching || zox_is_linked(world, walk->list, Unfolded, e)) {
        iter children = zox_children(world, e);
        while (zox_children_next(children)) {
            for (int i = 0; i < children.count; i++) {
                if (hierarchy_walk_tree(world, walk, children.entities[i], depth + 1)) {
                    zox_children_end(children);
                    return 1;
                }
            }
        }
    }
    return 0;
}

static void hierarchy_walk(ecs* world, HierarchyWalk* walk) {
    walk->index = 0;
    if (walk->root) {
        hierarchy_walk_tree(world, walk, walk->root, 0);
        return;
    }
    iter roots = zox_children(world, 0);
    while (zox_children_next(roots)) {
        for (int i = 0; i < roots.count; i++) {
            const entity root = roots.entities[i];
            if (hierarchy_root_entity(world, walk->list, root) && hierarchy_walk_tree(world, walk, root, 0)) {
                zox_children_end(roots);
                return;
            }
        }
    }
}

static HierarchyWalk hierarchy_walk_begin(ecs* world, entity window, entity list) {
    HierarchyWalk walk = {
        .list = list,
        .root = zox_getv(window, Target),
        .anchor = zox_get_link(world, list, ListStartLink),
        .anchor_index = -1,
    };
    window_search_get(world, window, walk.search, sizeof(walk.search), &walk.search_type);
    return walk;
}

void button_event_clicked_hierarchy(ecs* world, ClickEventData event) {
    const entity target = zox_get_link(world, event.clicked, RowTargetLink);
    if (target) {
        inspector_select_target(world, event.clicker, target);
        zox_setv(event.clicked, ActiveState, 1);
        zox_add(event.clicked, ActiveStateDirty);
    }
}

static void hierarchy_slots(ecs* world, entity window, entity list, VirtualListPage* page) {
    if (!virtual_list_prepare_slots(world, list, page)) {
        return;
    }
    const byte font = zox_getv(window, ElementFontSize);
    const entity canvas = zox_get_parent_by_id(world, window, zox_id(Canvas));
    const byte layer = zox_getv(list, Layer) + 2;
    for (int i = 0; i < page->capacity; i++) {
        const entity row = spawn_editor_list_row(world, list, prefab_button_hierarchy, "", font, editor_button_outline, i, font);
        page->slots[i] = row;
        zox_add(row, HierarchySlot);
        zox_add(row, ListSlot);
        zox_setv(row, ClickEvent, button_event_clicked_hierarchy);
        spawn_fold_arrow(world, canvas, row, list, 0, 0, font, layer);
    }
}

static void hierarchy_slot_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity row = it->entities[i];
        const entity target = zox_get_link(world, row, RowTargetLink);
        const entity list = zox_get_parent(world, row);
        const entity label = zox_get_child_by_id(world, row, zox_id(TextData));
        const entity fold = zox_get_child_by_id(world, row, zox_id(FoldArrow));
        if (!label || !fold) {
            continue;   // still spawning: its arrow's arrival fills it (fold_arrow_added_event)
        }
        zox_muter(label, TextData, data);
        if (!data->value) {
            // Reserve the label buffer when the slot is created, including initially empty slots.
            initialize_TextData(data, hierarchy_max_line_characters);
            data->length = 0;
        }
        if (!target || !data->value) {
            set_fold_arrow(world, fold, 0, 0, 1);
            continue;
        }
        char text[hierarchy_max_line_characters];
        get_editor_entity_label(world, target, text, sizeof(text));
        if (!is_zext(data, text)) {
            data->length = strlen(text);
            for (int j = 0; j < data->length; j++) {
                data->value[j] = convert_ascii(text[j]);
            }
            zox_add(label, Refresh);
        }
        const int font = zox_getv(label, TextFontSize);
        const byte2 padding = zox_getv(list, TextPadding);
        const int2 size = { font * (strlen(text) + 1) + 2 * padding.x, font + 2 * padding.y };
        if (!int2_equals(size, zox_getv(row, LayoutSize))) {
            zox_setv(row, LayoutSize, size);
            zox_add(row, LayoutSizeDirty);
        }
        byte has_children = 0;
        iter children = zox_children(world, target);
        while (zox_children_next(children)) {
            for (int j = 0; j < children.count && !has_children; j++) {
                has_children = hierarchy_row_entity(world, list, children.entities[j]);
            }
            if (has_children) {
                zox_children_end(children);
                break;
            }
        }
        set_fold_arrow(world, fold, has_children ? target : 0,
            zox_is_linked(world, list, Unfolded, target), !has_children);
    }
}

static void hierarchy_spawn_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity list = it->entities[i];
        const entity window = zox_get_parent_by_id(world, list, zox_id(HierarchyUI));
        if (!window) {
            continue;
        }
        HierarchyWalk walk = hierarchy_walk_begin(world, window, list);
        hierarchy_walk(world, &walk);
        const int count = walk.index;
        walk.start = virtual_list_start(world, list, count, walk.anchor_index);
        VirtualListPage page = { 0 };
        hierarchy_slots(world, window, list, &page);
        walk.page = &page;
        hierarchy_walk(world, &walk);
        virtual_list_finish(world, list, &page, walk.first, count, walk.start);
    }
}


static byte hierarchy_shows_children(ecs* world, entity list, entity parent) {
    return hierarchy_root_entity(world, list, parent) || zox_is_linked(world, list, Unfolded, parent);
}

// A child added to or removed from a shown row (unfolded, or a root's fold arrow) refreshes the tree.
static void hierarchy_child_event(iter* it) {
    zox_sys_world();
    const entity list = hierarchy_list_open;
    if (!list || !zox_valid(list) || zox_has(list, Dirty)) {
        return;
    }
    for (int i = 0; i < it->count; i++) {
        const entity parent = zox_get_parent(world, it->entities[i]);
        if (!zox_valid(parent) || !hierarchy_row_entity(world, list, it->entities[i])) {
            continue;
        }
        const entity grand = zox_get_parent(world, parent);
        if (hierarchy_shows_children(world, list, parent) || (grand && zox_is_linked(world, list, Unfolded, grand))) {
            zox_add(list, Dirty);
            return;
        }
    }
}

