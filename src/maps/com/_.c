zox_tag(MenuMap);
zox_tag(Map);
zox_tag(Minimap);
zox_tag(MapArrow);
zoxc_byte(MapZoom);
zoxc_int2(MapPosition);
zoxc_int2(MapPiecePosition);

void zox_components_maps(ecs* world) {
    zoxd_tag(MenuMap);
    zoxd_tag(Map);
    zoxd_tag(Minimap);
    zoxd_tag(MapArrow);
    zoxd_byte(MapZoom);
    zoxd_int2(MapPosition);
    zoxd_int2(MapPiecePosition);
}
