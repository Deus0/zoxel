// a grid's window (an inventory's slots): its body a scroll view, the grid its list, a vertical bar shown while its
//  rows don't fit (sys/layout.c window_grid_fit), the wheel and the bar's handle scroll its rows
//  - returns the window, its header, its grid (give it GridSize, GridPadding, GridMargins and its cells)
#define window_grid_bar_width 6
entity3 spawn_window_grid(ecs* world, entity prefab, entity canvas, const char* header, int2 position, int2 size,
    float2 anchor, byte header_font_size, byte2 header_padding, void* on_closed)
{
    const entity3 e3 = spawn_window(world, prefab, prefab_body, header, canvas, position, size, anchor,
        header_font_size, header_padding, on_closed);
    const entity scrollview = spawn_scrollview(world, e3.z, int2_zero, size, float2_half, window_grid_bar_width, 1, 1, 0);
    const entity grid = spawn_uic(world, prefab_grid, scrollview, float2_half, int2_zero, size, size,
        (color) { 0, 0, 0, 0 }, (color) { 0, 0, 0, 0 });
    zox_set_link(world, scrollview, ListUILink, grid);
    zox_set_link(world, grid, ScrollviewLink, scrollview);
    zox_set_link(world, e3.x, ScrollviewLink, scrollview);
    return (entity3) { e3.x, e3.y, grid };
}
