static void zox_events_windows(ecs* world) {
    zox_on_add(window_layout_event, [in] layouts.LayoutSize, [none] windows.Window, [none] core.Dirty);
    zox_on_add(window_list_layout_event, [none] containers.ListVisible, [none] core.Dirty);
    zox_on_add(window_search_bar_event, [none] windows.WindowSearchBar, [none] core.Dirty);
    zox_on_add(window_search_input_event, [none] texts.TextInput, [none] core.Dirty);
    zox_on_add(window_search_input_event, [none] elements2.Dropdown, [none] core.Dirty);
}
