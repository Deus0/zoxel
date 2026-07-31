#include "label.c"
#include "button.c"
#include "icon.c"
#include "frame.c"
#include "handle.c"
#include "slider.c"
#include "bar.c"
#include "scrollbar.c"
// Panels - TODO: Move these to containers
#include "grid.c"
#include "list.c"
#include "overlay.c"
// Has a child text entity
entity prefab_label;
// Combines texture with the text entity
entity prefab_label_textured;
// Interactive Element with child text
entity prefab_clickable;
entity prefab_button;
entity prefab_icon;
entity prefab_frame;
entity prefab_frame_selectable;
entity prefab_scrollbar;
entity prefab_elementbar2;
entity prefab_handle;
entity prefab_slider;
// Panels
entity prefab_list;
entity prefab_grid;
// misc
entity prefab_canvas_overlay;

void spawn_prefabs_elements2(ecs *world) {
    // elements
    prefab_label = spawn_prefab_label(world, prefab_text);
    prefab_label_textured = spawn_prefab_label(world, prefab_text_textured);
    prefab_clickable = spawn_prefab_button(world, prefab_element_frame);
    prefab_button = spawn_prefab_button(world, prefab_element_frame);
    zox_add_tag(prefab_button, NavigationElement);
    // statbars
    {
        prefab_elementbar2 = spawn_prefab_elementbar(world, prefab_element_frame);
        zox_add_tag(prefab_elementbar2, Elementbar);
    }
    // icons
    prefab_icon = spawn_prefab_icon(world, prefab_element);
    prefab_frame = spawn_prefab_frame(world, prefab_element_frame);
    prefab_frame_selectable = spawn_prefab_frame_toggleable(world, prefab_frame, default_outline_color_frame, button_outline_active);
    // handles
    prefab_handle = spawn_prefab_handle(world, prefab_clickable);
    prefab_slider = spawn_prefab_slider(world, prefab_element_frame);
    prefab_scrollbar = spawn_prefab_scrollbar(world, prefab_element_frame);
    // lists
    prefab_list = spawn_prefab_list(world, prefab_layout2);
    // more stuffs
    prefab_canvas_overlay = spawn_prefab_overlay(world, prefab_element_frame);
}
