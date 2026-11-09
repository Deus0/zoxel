// Sets a stat icon label to its stats value
void set_stat_icon_label(
    ecs* world,
    entity e,
    TextData* text,
    entity stat)
{
    byte dbg_log = 0;
    const byte label_text_capacity = 8;
    if (!zox_valid(stat) ||
        !zox_has(stat, Stat) ||
        !zox_has(stat, StatValue))
    {
        return;
    }
    zox_geter(stat, StatValue, value);
    char result[label_text_capacity];
    snprintf(result, label_text_capacity, "%i", (int) floor(value->value));
    if (!is_zext(text, result)) {
        set_zext(text, result);
        zox_add(e, Dirty);
        if (dbg_log) {
            zox_log("Stat Frame Label [%s] Set to [%s]",
                zox_getn(e),
                result);
        }
    }
}

// Label spawned or its data changed
void stat_icon_label_event(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(TextData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(TextData, text);
        entity stat = zox_has(e, DataLink) ?
            zox_getv(e, DataLink) :
            0;
        set_stat_icon_label(world, e, text, stat);
    }
}

// Labels that display data, used to find a stats labels
ecs_query_t* stat_labels_query = NULL;

// Stat became DataDirty, update its linked labels
void stat_dirty_labels_event(iter* it) {
    if (!stat_labels_query) {
        return;
    }
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        iter it2 = ecs_query_iter(world, stat_labels_query);
        while (ecs_query_next(&it2)) {
            const DataLink* links = ecs_field(&it2, DataLink, 0);
            TextData* texts = ecs_field(&it2, TextData, 1);
            for (int j = 0; j < it2.count; j++) {
                if (links[j].value == e) {
                    set_stat_icon_label(world, it2.entities[j], &texts[j], e);
                }
            }
        }
    }
}

// Tooltip Event for item icons
void stat_icon_tooltip_event(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DataLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DataLink, data);
        entity stat = data->value;
        // TODO: We should do this for only X types of items
        //      - BlockItem, ConsumeableItem, etc
        if (!zox_valid(stat) ||
            !zox_has(stat, Stat))
        {
            continue;
        }
        entity canvas = zox_get_parent_by_id(
            world,
            e,
            zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }
        entity tooltip = zox_get_child_by_id(
            world,
            canvas,
            zox_id(Tooltip));
        if (!tooltip) {
            zox_loge("Tooltip not found in canvas");
            continue;
        }
        char result[128];
        const char *name = zox_has(stat, ZoxName) ?
            zox_getv(stat, ZoxName) : "none";
        if (zox_has(stat, StatLevel)) {
            float value = zox_getv(stat, StatValue);
            float experience = zox_getv(stat, ExperienceValue);
            float experience_max = zox_getv(stat, ExperienceMax);
            sprintf(result, "[%s] lvl %i [%i/%i]\n", name, (int) value, (int) experience, (int) experience_max);
        } else if (zox_has(stat, StatState)) {
            float value = zox_getv(stat, StatValue);
            float value_max = zox_getv(stat, StatValueMax);
            sprintf(result, "[%s] [%i/%i]\n", name, (int) value, (int) value_max);
        } else if (zox_has(stat, StatRegen)) {
            float value = zox_getv(stat, StatValue);
            sprintf(result, "[%s] [%i]\n", name, (int) value);
        } else if (zox_has(stat, StatAttribute)) {
            float value = zox_getv(stat, StatValue);
            sprintf(result, "[%s] [%i]\n", name, (int) value);
        } else {
            sprintf(result, "[%s]\n", name);
        }
        set_tooltip_text(world, e, tooltip, result);
        if (dbg_log) {
            zox_log("Stat Icon Tooltip: [%s]: [%s]",
                zox_sys_e_name,
                result);
        }
    }
}
