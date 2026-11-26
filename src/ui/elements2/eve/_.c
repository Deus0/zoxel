#include "item_list.c"
#include "menubar.c"

static void zox_events_elements2(ecs* world) {
    zox_on_add(item_list_pointer_event, [none] inputs.ZevicePointer, [none] core.Target, [none] core.Dirty);
    zox_on_add(menu_item_layout_event, [none] elements2.ItemListTick, [none] core.Dirty);
    zox_on_add(item_list_item_event, [none] elements2.ItemListItem, [none] core.Dirty);
    zox_on_add(dropdown_selected_event, [none] elements2.Dropdown, [none] core.Dirty);
    zox_on_add(menu_item_click_event, [none] elements2.ItemListItem, [none] interactions.Click);
    zox_on_add(menubar_layout_event, [none] elements2.Menu, [none] core.Dirty);
    zox_on_add(menubar_select_event, [none] elements2.Menu, [none] interactions.TriggerSelect);
}
