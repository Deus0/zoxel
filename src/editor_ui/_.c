#ifndef zoxm_editor
#define zoxm_editor

// TODO: make basic structs just to read out generic component data
// TODO: add_entity_to_labels should be a macro that takes in Tag so I can filter it with zox_has before adding: try these - Element, Character, Camera, Position3D, Position2,

zox_tag(EditorElement);
zox_tag(HierarchyUI);
zox_tag(InspectorLabel);
zoxc_byte(HierarchyUIDirty);
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "dbg/_.c"

zox_begin_module(EditorUI)
    add_hook_spawn_prefabs(spawn_prefabs_editor);
    zoxd_tag(EditorElement);
    zoxd_tag(HierarchyUI);
    zoxd_tag(InspectorLabel);
    zoxd(HierarchyUIDirty);
    zox_define_systems_editor(world);
zox_end_module(EditorUI)

#endif