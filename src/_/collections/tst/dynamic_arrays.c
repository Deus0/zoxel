// tst: dynamic_arrays, the growing arrays meshes are built in (collections/macros/dynamic_array.c)
//  - added one by one past many growths, every value kept in order; has and index find them, not others
//  - add_block adds a run at once, after what's there, growing to fit a run longer than its room
//  - finalize hands back just the values (a copy the size of them), an empty one gives NULL
//  - run: bash bsh/tst.sh dynamic_arrays
void zox_tst_dynamic_arrays_check(ecs* world) {
    (void) world;
    const int count = 5000;
    int_array_d* array = create_int_array_d(4);
    for (int i = 0; i < count; i++) {
        int_array_d_add(array, i * 3);
    }
    int wrong = 0;
    for (int i = 0; i < count; i++) {
        wrong += array->data[i] != i * 3;
    }
    zox_tst_check(array->size == (size_t) count, "[%zu] values after adding [%i]", array->size, count);
    zox_tst_check(!wrong, "[%i] of [%i] values changed while the array grew", wrong, count);
    zox_tst_check(array->capacity >= array->size, "capacity [%zu] under its size [%zu]", array->capacity, array->size);
    zox_tst_check(int_array_d_has(array, 2997) && !int_array_d_has(array, 2998), "has finds the wrong values");
    zox_tst_check(int_array_d_index(array, 2997) == 999 && int_array_d_index(array, 1) == -1, "index finds the wrong places");
    // a run added at once
    const int block[5] = { -1, -2, -3, -4, -5 };
    add_block_to_int_array_d(array, block, 5);
    wrong = 0;
    for (int i = 0; i < 5; i++) {
        wrong += array->data[count + i] != block[i];
    }
    zox_tst_check(array->size == (size_t) count + 5 && !wrong, "add_block: size [%zu], [%i] values wrong", array->size, wrong);
    // a run longer than the array's room: it grows to fit (it used to double once and overflow)
    int_array_d* small = create_int_array_d(2);
    int run[40];
    for (int i = 0; i < 40; i++) {
        run[i] = i;
    }
    add_block_to_int_array_d(small, run, 40);
    zox_tst_check(small->capacity >= 40 && small->size == 40 && small->data[39] == 39,
        "a run of 40 into room for 2: capacity [%zu] size [%zu]", small->capacity, small->size);
    dispose_int_array_d(small);
    // handed back
    const size_t size = array->size;
    int* values = finalize_arrayd_int(array);
    zox_tst_check(values && values[0] == 0 && values[count - 1] == (count - 1) * 3 && values[size - 1] == -5,
        "finalize lost its values");
    zox_free(values);
    int_array_d* empty = create_int_array_d(4);
    zox_tst_check(finalize_arrayd_int(empty) == NULL, "finalizing an empty array isn't NULL");
}
