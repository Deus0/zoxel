// Checks if can place icon in skill frame
byte can_place_icon_in_skill_frame(ecs* world, entity frame, entity data) {
    if (!zox_valid(frame)) {
        return 0;
    } else if (!zox_has(frame, SkillFrame)) {
        return 1;
    } else {
        return zox_has(data, Skill);
    }
}
