zox_tag(MenuMap);
zox_tag(Map);
zox_tag(Minimap);
zox_tag(MapArrow);
zoxc_byte(MapZoom);
zoxc_int2(MapPosition);
zoxc_int2(MapPiecePosition);
// a map's view: one texture of the land round the player, a texel blocks_per_texel blocks, redrawn as they move a
//  texel (map_view_system): it moves per texel, not per tunk
zox_tag(MapView);
zoxc_byte(MapBlocksPerTexel);
zoxc_int2(MapDrawnAt);          // the texel (block / blocks per texel) it was last drawn round
zoxc_int(MapDrawnVersion);      // its terrain's TunkMapsVersion when drawn
zoxc_byte(MapDrawnWhole);       // every tunk it shows had its colours (else it redraws every map_view_retry seconds)
zoxc_double(MapDrawnTime);

void zox_components_maps(ecs* world) {
    zoxd_tag(MenuMap);
    zoxd_tag(Map);
    zoxd_tag(Minimap);
    zoxd_tag(MapArrow);
    zoxd_byte(MapZoom);
    zoxd_int2(MapPosition);
    zoxd_int2(MapPiecePosition);
    zoxd_tag(MapView);
    zoxd_byte(MapBlocksPerTexel);
    zoxd_int2(MapDrawnAt);
    zoxd_int(MapDrawnVersion);
    zoxd_byte(MapDrawnWhole);
    zoxd_double(MapDrawnTime);
}
