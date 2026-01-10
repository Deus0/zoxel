void raycaster_select_first_button(ecs *world, const entity raycaster, const entity window) {
    entity button = find_child_with_tag_recursive(world, window, zox_id(Button));
    // find_child_with_tag(window, Button, element);
    if (button) {
        raycaster_select_element(world, raycaster, button);
    }
}