entity spawn_prefab_inspector(ecs *world, entity p) {

    zox_prefab_child(p);
    zox_prefab_name("inspector");

    zox_set(e, WindowType, { zox_window_inspector });

    return e;
}

// const text_group labels[] = { { "[ ------ select entity ------ ]" } };
// const text_group labels[] = { { "[ select entity ]" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "" }, { "[ ---------- -------------- ]" }  };
