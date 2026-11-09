zox_tag(Statbar);
zox_tag(MenuStats);
zox_tag(StatsLabel);
zox_tag(Healthbar);
zox_tag(StatIconLabel);
zox_tag(StatBars);
zox_tag(HealthbarLink);
zox_tag(StatPlusButton);    // spends a stat point on an attribute

void zox_components_stats_ui(ecs* world) {
    zoxd_tag(Statbar);
    zoxd_tag(MenuStats);
    zoxd_tag(StatsLabel);
    zoxd_tag(Healthbar);
    zoxd_tag(StatIconLabel);
    zoxd_tag(StatBars);
    zoxd_nf_tag(HealthbarLink);
    zoxd_tag(StatPlusButton);
}

// quick fix for now
entity zox_get_id_statbars(ecs* world) {
    return zox_id(StatBars);
}
