// Spawn a basic invisible panel and a scrollbar
//  - used with list or grid entities

// TODO: Set scrollbar visible/invisible based on list count
// TODO: spawn list panel, and scrollbar as children of list entity

// NOTE: Set children on scrollview after spawning
entity spawn_scrollview(
    ecs *world,
    const LayoutParentData canvas_data,
    const LayoutParentData parent_data,
    const ElementSpawnData element_data,
    Children* children,
    int visible,
    int max
) {
    int scrollbar_width = 4 * ui_scale;
    int scrollbar_margins = 0;

    zox_instance(prefab_element_invisible);
    set_element_spawn_data(
        world,
        e,
        canvas_data,
        parent_data,
        element_data
    );
    zox_name("scrollview");

    const entity scrollbar = spawn_scrollbar(
        world,
        e,
        canvas_data.e,
        (int2) { -(scrollbar_width / 2) - scrollbar_margins, 0 },
        element_data.layer + 1,
        element_data.size,
        scrollbar_width,
        scrollbar_margins,
        visible,
        max,
        visible != max
    );
    add_to_Children(children, scrollbar);

    return e;
}