static inline int zoxs_get_int(const setting s) {
    return s.value_int;
}

// limit
byte zoxs_limit_int(ecs* world, const char *name, int min, int max) {
    for (uint i = 0; i < settings_count; i++) {
        setting s = settings[i];
        if (strcmp(name, s.name) == 0) {
            if (s.type == zox_data_type_int) {
                s.min_int = min;
                s.max_int = max;
                // now set again
                int new_value = clampf(s.value_int, s.min_int, s.max_int);

                if (new_value != s.value_int) {
                    s.value_int = new_value;

                    s.on_set(world, &new_value);
                    settings[i] = s;

                    save_settings();
                }
                settings[i] = s;

                return 1;
            }
        }
    }

    zox_log_error("? [int] setting [%s]", name)
    return 0;
}

byte zoxs_set_int(ecs *world, const char *name, int value) {
    for (uint i = 0; i < settings_count; i++) {
        setting setting = settings[i];
        if (strcmp(name, setting.name) == 0) {
            // zox_log("+ setting int [%s] at [%i]", name, i)
            if (setting.type == zox_data_type_int) {
                setting.value_int = value;
                setting.on_set(world, &value);
                settings[i] = setting;
                save_settings();
                return 1;
            }
        }
    }
    zox_log_error("missing [int] setting [%s]", name)
    return 0;
}

#define zoxs_new_int_lim(name, function, value, min, max) {\
   zoxs_set(name, zox_data_type_int, function);\
   zoxs_limit_int(world, name, min, max);\
   zoxs_set_int(world, name, value);\
}

#define zoxs_new_int(name, function, value) {\
   zoxs_set(name, zox_data_type_int, function);\
   zoxs_set_int(world, name, value);\
}
