//  TODO: This, make it on or off instead
/*
// get
static inline byte zoxs_get_bit(const setting s) {
    return s.value_byte;
}

// set

byte zoxs_set_bit_silently(ecs *world, const char *name, byte value) {
    value = clampf(value, 0, 1);
    (void) world;
    for (uint i = 0; i < settings_count; i++) {
        setting s = settings[i];
        if (strcmp(name, s.name) == 0) {
            if (s.type == zox_data_type_bit) {
                if (value != s.value_byte) {
                    s.value_byte = value;
                    settings[i] = s;
                    save_settings();
                }
                return 1;
            }
        }
    }
    zox_log_error("missing [byte] setting [%s]", name)
    return 0;
}

byte zoxs_set_bit(ecs *world, const char *name, byte value) {
    value = clampf(value, 0, 1);
    for (uint i = 0; i < settings_count; i++) {
        setting s = settings[i];
        if (strcmp(name, s.name) == 0) {
            if (s.type == zox_data_type_bit) {
                if (value != s.value_byte) {
                    s.value_byte = value;
                    s.on_set(world, &value);
                    settings[i] = s;
                    save_settings();
                }
                return 1;
            }
        }
    }
    zox_log_error("missing [byte] setting [%s]", name)
    return 0;
}

#define zoxs_new_biit(name, function, value) {\
    zoxs_set(name, zox_data_type_bit, function);\
    zoxs_set_bit(world, name, value);\
}
*/
