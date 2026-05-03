// Spawn a basic invisible panel and a scrollbar
//  - used with list or grid entities

// TODO: Set scrollbar visible/invisible based on list count
// TODO: spawn list panel, and scrollbar as children of list entity

// NOTE: Set children on scrollview after spawning
entity spawn_scrollview(ecs *world, LayoutParentData canvas, LayoutParentData parent, ElementSpawnData edata, Children* children, int visible, int max) {

    int scrollbar_width = 10 * ui_scale;
    int scrollbar_margins = 0;

    zox_instance(prefab_element_invisible);
    set_element_spawn_data(world, e, canvas, parent, edata);
    zox_name("scrollview");
    // zox_set(e, ListUILink, { parent.e });

    int2 scrollbar_position = (int2) { -(scrollbar_width / 2) - scrollbar_margins, 0 };
    byte scrollbar_layer = edata.layer + 3;

    entity scrollbar = spawn_scrollbar(world, e, canvas.e, scrollbar_position, scrollbar_layer, edata.size, scrollbar_width, scrollbar_margins, visible, max, visible != max, e);
    add_to_Children(children, scrollbar);

    zox_set(parent.e, ScrollviewLink, { e });

    return e;
}
