// NOTE: Core components for entire engine!
// General
zox_tag(ZoxSystem);
zox_tag(Disabled);
zox_tag(BuildDisabled);
zox_tag(DebugEntity);
// zoxc_byte(Active);
zoxc_double(DeactivateDelay);
zoxc_byte(GenericEvent);
zoxc_byte(SpawnChance);
zoxc_byte(Busy);
zoxc_byte(Ready);
zoxc_entity(SystemLink);
zoxc_state(Generate);
// zoxc_byte(Initialize);
zox_tag(PreInitialize);
zox_tag(Initialize);
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
zoxc_float2(Size2);
zoxc_float3(Size3);
zoxc_int2(PixelSize);
// Arrays
zoxc_double(DoubleDataMax);
zoxc_arrayd(DoubleData, double)
#define zox_load_none 0
#define zox_load_begin 1
#define zox_load_done 2
byte zox_tst_remove_deactivates = 0;
// Trigers
zoxc_state(TriggerActionA);
zoxc_state(TriggerActionB);
zoxc_state(TriggerActionE);

void zox_components_core(ecs* world) {
    // General
    zoxd_nf_tag(ZoxSystem);
    zoxd_nf_tag(Disabled);
    zoxd_nf_tag(BuildDisabled);
    zoxd_nf_tag(DebugEntity);
    zoxd_nf_tag(Initialize);
    zoxd_nf_tag(PreInitialize);
    // Data
    zoxd_double(DeactivateDelay);
    zoxd_byte(GenericEvent);
    zoxd_byte(SpawnChance);
    zoxd_byte(Busy);
    zoxd_byte(Ready);
    zoxd_state(Generate);
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
    zoxd_float2(Size2);
    zoxd_float3(Size3);
    zoxd_int2(PixelSize);
    // Arrays
    zoxd_double(DoubleDataMax);
    zoxd_arrayd(DoubleData);
    // Triggers
    zoxd_state(TriggerActionA);
    zoxd_state(TriggerActionB);
    zoxd_state(TriggerActionE);
}
