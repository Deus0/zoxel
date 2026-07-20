// NOTE: Core components for entire engine!
// General
zoxc_byte(Active);
zoxc_ushort(DeactivateDelay);
zoxc_byte(GenericEvent);
zoxc_byte(SpawnChance);
zoxc_byte(Busy);
zoxc_byte(Ready);
zoxc_entity(SystemLink);
zoxc_state(Generate);
zoxc_state(InitializeEntity);
// identitys
zoxc_int(ID);
zoxc_lint(Seed);
zoxc_fixed_string(ZoxName, 64);
// time events
zox_tag(DestroyInFrame);
zoxc_double(EventTime);
zoxc_entity(EventInput);
zoxc_function(TimedEvent, void, ecs*, const entity);
// used for editor
zoxc_byte(EntityDirty);
zoxc_entity(EntityTarget);
zoxc_entity(ComponentTarget);
zoxc_byte(ComponentType);
// Measurements
zoxc_byte(Radius);
zoxc_byte(Height);
// Arrays
zoxc_double(MaxDoubleData);
zoxc_arrayd(DoubleData, double)
#define zox_load_none 0
#define zox_load_begin 1
#define zox_load_done 2

// Adds the latest time onto system and moves rest of list up
void add_double_to_samples(ecs *world, entity e, double value) {
    if (!zox_valid(e) || !zox_has(e, DoubleData)) {
        return;
    }
    /*if (zox_has(e, PlotPaused) && zox_getv(e, PlotPaused)) {
        zox_log("plot [%lu] is paused.", e);
        return;
    }*/
    zox_muter(e, DoubleData, data);
    if (!data || !data->length || !data->value) {
        return;
    }
    for (int i = 0; i < data->length - 1; i++) {
        data->value[i] = data->value[i + 1];
    }
    data->value[data->length - 1] = value;
}

void zox_define_components_core(ecs* world) {
    // General
    zoxd_byte(Active);
    zoxd_ushort(DeactivateDelay);
    zoxd_byte(GenericEvent);
    zoxd_byte(SpawnChance);
    zoxd_byte(Busy);
    zoxd_byte(Ready);
    zoxd_state(Generate);
    zoxd_state(InitializeEntity);
    zoxd_entity(SystemLink);
    // ids
    zoxd_int(ID);
    zoxd_lint(Seed);
    zoxd_fixed_string(ZoxName);
    // timed
    zoxd_tag(DestroyInFrame);
    zoxd_double(EventTime);
    zoxd_entity(EventInput);
    zoxd(TimedEvent);
    // editor
    zoxd_byte(EntityDirty);
    zoxd_entity(EntityTarget);
    zoxd_entity(ComponentTarget);
    zoxd_byte(ComponentType);
    // Measurements
    zoxd_byte(Radius);
    zoxd_byte(Height);
    // Arrays
    zoxd_double(MaxDoubleData);
    zoxd_arrayd(DoubleData);
}
