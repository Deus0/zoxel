const int hierarchy_max_line_characters = 64;
const ushort hierarchy_max_elements = 128;

void inspector_select_target(
    ecs* world,
    entity player,
    entity target)
{
    if (!zox_valid(player)) {
        return;
    }
    entity canvas = zox_get_link(world, player, Canvas);
    if (!zox_valid(canvas)) {
        return;
    }
    entity inspector = zox_get_child_by_id(world, canvas, zox_id(InspectorUI));
    if (!zox_valid(inspector)) {
        zox_log("Inspector Closed, Spawning for Target [%s]", target ? zox_get_name(target) : "None");
        spawn_inspector(world, canvas, player, target);
        return;
    }
    if (!zox_has(inspector, EntityTarget)) {
        zox_log("Inspector Invalid Components");
        return;
    }
    zox_geter_value(inspector, EntityTarget, entity, old_target);
    if (old_target == target) {
        zox_log("Inspector Same Target [%s]", target ? zox_get_name(target) : "None");
        return;
    }
    zox_set(inspector, EntityTarget, { target });
    zox_set(inspector, InspectorDirty, { zox_dirty_trigger });
    zox_log("+ Inspector Target [%s]", target ? zox_get_name(target) : "None");
}

void button_event_clicked_hierarchy(
    ecs* world,
    ClickEventData event)
{
    entity player = event.clicker;
    entity clicked = event.clicked;
    if (!zox_has(clicked, EntityTarget)) {
        zox_loge("Clicked [%s] Invalid Components", zox_get_name(clicked));
        return;
    }
    zox_geter_value(clicked, EntityTarget, entity, target);
    inspector_select_target(world, player, target);
    zox_set(clicked, ActiveState, { 1 });
    zox_set(clicked, ActiveStateDirty, { zox_dirty_trigger });
}

void add_entity_to_labels(
    ecs *world,
    entity e,
    text_group_dynamic_array_d* labels,
    entity_array_d* entities,
    int tree_level)
{
    if (!zox_valid(e)) {
        return;
    }
    char *text = malloc(hierarchy_max_line_characters);
    if (!zox_has(e, ZoxName)) {
        snprintf(text, hierarchy_max_line_characters, "%s", zox_get_name(e));
    } else {
        zox_geter(e, ZoxName, zox_name);
        snprintf(text, hierarchy_max_line_characters, "%s", zox_name->value);
    }
    /*for (int i = 0; i < tree_level; i++) {
        char *temp = strdup(text);
        if (temp) {
            snprintf(text, hierarchy_max_line_characters, "-%s", temp);
            free(temp);
        } else {
            zox_loge("no temp was created")
        }
    }*/
    // zox_log("%s made label [%s]", zox_get_name(e), text)
    text_group_dynamic_array_d_add(labels, (text_group_dynamic) { .text = text });
    entity_array_d_add(entities, e);
}

void editor_fetch_children(
    ecs *world,
    text_group_dynamic_array_d* labels,
    entity_array_d* entities,
    byte_array_d* tree_levels,
    entity e,
    byte tree_level)
{
    if (entities->size >= hierarchy_max_elements) {
        zox_logw("Hierarchy past limits [%i]", hierarchy_max_elements);
        return;
    }
    if (!zox_valid(e) || zox_has(e, Glyph)) {
        return;
    }
    add_entity_to_labels(world, e, labels, entities, tree_level);
    byte_array_d_add(tree_levels, tree_level);
    tree_level++;
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            if (!zox_valid(e2)) {
                continue;
            }
            editor_fetch_children(world, labels, entities, tree_levels, e2, tree_level);
        }
    }
}

// NOTE: Spawns button children of hierarchy list
zox_sys2(HierarchySpawnSystem) {
    const ClickEvent on_click = (ClickEvent) { &button_event_clicked_hierarchy };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(HierarchyUIDirty);
    zox_sys_in(EntityTarget);
    zox_sys_in(ScrollviewLink);
    zox_sys_in(ElementFontSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(HierarchyUIDirty, dirty);
        zox_sys_i(EntityTarget, target);
        zox_sys_i(ScrollviewLink, scrollview);
        zox_sys_i(ElementFontSize, font_size);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }
        if (!zox_valid(scrollview->value)) {
            zox_loge("Scrollview Link is invalid.");
            continue;
        }
        if (!zox_has(scrollview->value, ListUILink)) {
            zox_loge("Scrollview [%s] has no list link", zox_get_name(scrollview->value));
            continue;
        }
        // 2: Fetch our scrollview data
        entity list_ui = zox_getv(scrollview->value, ListUILink);
        if (!zox_valid(list_ui)) {
            zox_loge("Scrollview [%s] Invalid ListUI", zox_get_name(scrollview->value));
            continue;
        }
        if (!zox_has(list_ui, Layer) || !zox_has(list_ui, ListVisible)) {
            zox_loge("List UI [%s] Invalid Components", zox_get_name(list_ui));
            continue;
        }
        // byte scrollview_layer = zox_getv(list_ui, Layer);
        // Fetch Target Hierarchy Data
        entity_array_d* entities = create_entity_array_d(1);
        text_group_dynamic_array_d* labels = create_text_group_dynamic_array_d(1);
        byte_array_d* tree_levels = create_byte_array_d(1);
        editor_fetch_children(
            world,
            labels,
            entities,
            tree_levels,
            target->value,
            0);
        // Initialize Data
        zox_geter_value(list_ui, TextPadding, byte2, text_padding);
        // Delete old list elements
        iter it2 = zox_children(world, list_ui);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                if (zox_valid(e2)) {
                    zox_delete(e2);
                }
            }
        }
        // Spawn new buttons
        byte visible = zox_getv(list_ui, ListVisible);
        byte button_layer = zox_getv(list_ui, Layer) + 1;
        byte button_text_layer = button_layer + 1;
        for (size_t j = 0; j < labels->size; j++) {
            // child_text_data.text = labels->data[j].text;
            const char* text = labels->data[j].text;
            entity target = entities->data[j];
            byte tree_level = tree_levels->data[j];
            entity2 e2 = spawn_button(
                world,
                prefab_button_hierarchy,
                list_ui,
                text,
                int2_zero,
                int2_zero,
                float2_half,
                zox_alignment_centre,
                font_size->value,
                text_padding,
                editor_button_fill,
                editor_button_outline,
                editor_color_font,
                editor_color_fonto);
            zox_set(e2.x, ClickEvent, { on_click.value });
            zox_set(e2.x, EntityTarget, { target });
            zox_set(e2.x, ListIndent, { tree_level });
            zox_set(e2.x, Layer, { button_layer });
            zox_set(e2.y, Layer, { button_text_layer });
            byte rendered = j < visible;
            zox_set(e2.x, RenderDisabled, { !rendered });
            zox_set(e2.y, RenderDisabled, { !rendered });
        }
        // Set ListDirty for positioning / hiding etc
        zox_set(list_ui, ListStart, { 0 });
        zox_set(list_ui, ListDirty, { zox_dirty_trigger });
        zox_set(list_ui, ListPositionDirty, { zox_dirty_trigger });
        // Debug
        zox_logv("Hierarchy Refreshed");
        zox_logv("   - Elements [%i]", labels->size);
        zox_logv("   - Visible [%i]", visible);
        for (size_t j = 0; j < labels->size; j++) {
            zox_logv("   + %i: [%s]", j, labels->data[j].text);
        }
        // Dispose of dynamic arrays
        for (size_t j = 0; j < labels->size; j++) {
            free(labels->data[j].text);
        }
        dispose_text_group_dynamic_array_d(labels);
        dispose_entity_array_d(entities);
        dispose_byte_array_d(tree_levels);
    }
} zox_sys_end(HierarchySpawnSystem);
