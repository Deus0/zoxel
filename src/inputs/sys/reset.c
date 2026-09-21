// reset each zevice in a system, on EcsOnLoad
zox_sys2(ZeviceButtonResetSystem) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_out(ZeviceButton);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(ZeviceButton, button);
        if (devices_get_pressed_this_frame(button->value)) {
            devices_set_pressed_this_frame(&button->value, 0);
            if (dbg_log) {
                zox_log("Pressed this frame is resetting [%s]", zox_sys_e_name);
            }
        } else if (devices_get_released_this_frame(button->value))  {
            devices_set_released_this_frame(&button->value, 0);
            if (dbg_log) {
                zox_log("Releasing this frame is resetting [%s]", zox_sys_e_name);
            }
        }
    }
} zox_sys_end(ZeviceButtonResetSystem);

zox_sys2(ZevicePointerResetSystem) {
    zox_sys_begin();
    zox_sys_out(ZevicePointer);
    zox_sys_out(ZevicePointerOld);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(ZevicePointer, zevicePointer);
        zox_sys_o(ZevicePointerOld, zevicePointerOld);
        zevicePointerOld->value = zevicePointer->value;
        zevicePointer->value = reset_button_state(zevicePointer->value);
    }
} zox_sys_end(ZevicePointerResetSystem);

zox_sys2(ZevicePointerRightResetSystem) {
    zox_sys_begin();
    zox_sys_out(ZevicePointerRight);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(ZevicePointerRight, click);
        click->value = reset_button_state(click->value);
    }
} zox_sys_end(ZevicePointerRightResetSystem);

zox_sys2(ZevicePointerDeltaResetSystem) {
    zox_sys_begin();
    zox_sys_out(ZevicePointerDelta);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(ZevicePointerDelta, zevicePointerDelta);
        zevicePointerDelta->value = int2_zero;
    }
} zox_sys_end(ZevicePointerDeltaResetSystem);

zox_sys2(ZeviceFingerResetSystem) {
    /*zox_sys_begin();
    zox_sys_in(ZevicePointerOld);
    zox_sys_out(ZevicePointerPosition);
    zox_sys_out(ZevicePointerDelta);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ZevicePointerOld, clicked)
        zox_sys_o(ZevicePointerPosition, position)
        zox_sys_o(ZevicePointerDelta, delta)
        if (devices_get_released_this_frame(clicked->value)) {
            position->value = int2_hidden;
            delta->value = int2_zero;
        }
    }*/
} zox_sys_end(ZeviceFingerResetSystem);

#define reset_keyboard_key(key) \
    reset_key(&keyboard->key);

zox_sys2(KeyboardResetSystem) {
    zox_sys_begin();
    zox_sys_out(Keyboard);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(Keyboard, keyboard);
        reset_key(&keyboard->space);
        reset_key(&keyboard->escape);
        reset_key(&keyboard->enter);
        reset_key(&keyboard->backspace);
        reset_key(&keyboard->tab);
        reset_key(&keyboard->left_control);
        reset_key(&keyboard->right_control);
        reset_key(&keyboard->left_alt);
        reset_key(&keyboard->right_alt);
        reset_key(&keyboard->left_shift);
        reset_key(&keyboard->right_shift);
        reset_key(&keyboard->back_quote);
        reset_key(&keyboard->a);
        reset_key(&keyboard->b);
        reset_key(&keyboard->c);
        reset_key(&keyboard->d);
        reset_key(&keyboard->e);
        reset_key(&keyboard->f);
        reset_key(&keyboard->g);
        reset_key(&keyboard->h);
        reset_key(&keyboard->i);
        reset_key(&keyboard->j);
        reset_key(&keyboard->k);
        reset_key(&keyboard->l);
        reset_key(&keyboard->m);
        reset_key(&keyboard->n);
        reset_key(&keyboard->o);
        reset_key(&keyboard->p);
        reset_key(&keyboard->q);
        reset_key(&keyboard->r);
        reset_key(&keyboard->s);
        reset_key(&keyboard->t);
        reset_key(&keyboard->u);
        reset_key(&keyboard->v);
        reset_key(&keyboard->w);
        reset_key(&keyboard->x);
        reset_key(&keyboard->y);
        reset_key(&keyboard->z);
        reset_key(&keyboard->down);
        reset_key(&keyboard->up);
        reset_key(&keyboard->left);
        reset_key(&keyboard->right);
        reset_keyboard_key(f1);
        reset_keyboard_key(f2);
        reset_keyboard_key(f3);
        reset_keyboard_key(f4);
        reset_keyboard_key(f5);
        reset_keyboard_key(f7);
        reset_keyboard_key(f8);
        reset_keyboard_key(f9);
        reset_keyboard_key(f10);
        reset_keyboard_key(f11);
        reset_keyboard_key(f12);
        reset_keyboard_key(_1);
        reset_keyboard_key(_2);
        reset_keyboard_key(_3);
        reset_keyboard_key(_4);
        reset_keyboard_key(_5);
        reset_keyboard_key(_6);
        reset_keyboard_key(_7);
        reset_keyboard_key(_8);
        reset_keyboard_key(_9);
        reset_keyboard_key(_0);
    }
} zox_sys_end(KeyboardResetSystem);
