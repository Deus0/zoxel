typedef struct {
    byte index;
    entity component_id;
    entity_fun spawn;
    char *tooltip_text;
    char *texture_name;
} TaskbarData;
zoxc_custom(TaskbarData);

typedef struct {
    entity_fun value;
} SpawnWindowEvent;
zoxc_custom(SpawnWindowEvent);

entity taskbar_manager;

void spawn_taskbar_manager(ecs *world) {
    taskbar_manager = zox_new();
}

void add_taskbar_button(ecs* world, TaskbarData data) {
    if (!zox_valid(taskbar_manager)) {
        zox_loge("TaskbarManager is invaild");
        return;
    }
    entity item = zox_new();
    zox_set_unique_name(item, data.tooltip_text);
    zox_set_data(item, TaskbarData, data);
    zox_set_parent(world, item, taskbar_manager);
    // TaskbarData_array_d_add(TaskbarDatas, data);
}
