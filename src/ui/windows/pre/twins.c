// The 3D twins of the 2D uis' prefabs (Prefab3, ui_prefab_for): each made by its 2D prefab's own function on a 3D
//  base (an element3, a frame3: drawn by a canvas3), so a 2D window's spawn code builds it in 3D unchanged
//  (spawn_window on 3D huds, spawn_ui, spawn_text)
//  - the bases are Elements too: selection, clicking and active states (ui/interaction) are every element's
entity prefab_element3_twin;     // the 3D twins' base element: later modules make theirs on it (render textures)
entity prefab_frame3_twin;       // and their base frame (tooltips)
extern entity prefab_header;
extern entity prefab_body;
extern entity prefab_grid;
extern entity prefab_close_button;

static inline void prefab3_twin(ecs* world, entity prefab, entity twin) {
    if (zox_valid(prefab) && zox_valid(twin)) {
        zox_setv(prefab, Prefab3, twin);
    }
}

void spawn_prefabs_twins3(ecs* world) {
    entity element3 = zox_prefab_from_parent(world, prefab_element3);
    zox_set_unique_name(element3, "prefab_element3_twin");
    zox_add(element3, Element);
    zox_setv(element3, Layer, 1);
    prefab_element3_twin = element3;
    entity frame3 = zox_prefab_from_parent(world, prefab_frame3);
    zox_set_unique_name(frame3, "prefab_frame3_twin");
    zox_add(frame3, Element);
    zox_setv(frame3, Layer, 1);
    prefab_frame3_twin = frame3;
    prefab3_twin(world, prefab_element, element3);
    prefab3_twin(world, prefab_element_frame, frame3);
    prefab3_twin(world, prefab_text, prefab_text3);
    prefab3_twin(world, prefab_label2, spawn_prefab_label(world, prefab_text3));
    // a textured label (a frame behind its text): a 3D text on a frame
    prefab3_twin(world, prefab_label2t, spawn_prefab_label(world, spawn_prefab_text(world, frame3, prefab_zigel3)));
    prefab3_twin(world, prefab_icon, spawn_prefab_icon(world, element3));
    const entity frame = spawn_prefab_frame(world, frame3);
    prefab3_twin(world, prefab_frame, frame);
    prefab3_twin(world, prefab_frame_selectable,
        spawn_prefab_frame_toggleable(world, frame, default_outline_color_frame, button_outline_active));
    const entity clickable = spawn_prefab_button(world, frame3);
    prefab3_twin(world, prefab_clickable, clickable);
    const entity button = spawn_prefab_button(world, frame3);
    zox_add(button, NavigationElement);
    prefab3_twin(world, prefab_button, button);
    prefab3_twin(world, prefab_header, spawn_prefab_header(world, clickable));
    const entity body = spawn_prefab_body(world, frame3);
    prefab3_twin(world, prefab_body, body);
    prefab3_twin(world, prefab_grid, spawn_prefab_grid(world, body));
    prefab3_twin(world, prefab_list, spawn_prefab_list(world, prefab_layout3_child));
    // scrollviews: an invisible panel, its scrollbar and handle
    prefab3_twin(world, prefab_layout2, prefab_layout3_child);
    prefab3_twin(world, prefab_scrollbar, spawn_prefab_scrollbar(world, frame3));
    prefab3_twin(world, prefab_slider, spawn_prefab_slider(world, frame3));
    prefab3_twin(world, prefab_close_button, spawn_prefab_button_close(world, clickable));
    prefab3_twin(world, prefab_handle, spawn_prefab_handle(world, clickable));
}
