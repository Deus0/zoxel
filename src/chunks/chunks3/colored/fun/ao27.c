// Macro to compute an index into the 3x3x3 neighbour array at compile time.
#define IDX(dx,dy,dz) ((((dx) + 1) * 9) + (((dy) + 1) * 3) + ((dz) + 1))

// neighbor_indices_27[face][vertex][3] -> { side1_index, side2_index, corner_index }
// face order matches your previous code: 0=Left,1=Right,2=Down,3=Up,4=Back,5=Front
// vertex order: (down,back), (down,front), (up,front), (up,back)
static const byte neighbor_indices_27[6][4][3] = {
    // Left face  (n = -1,0,0)  u = (0,1,0) (y),  v = (0,0,1) (z)
    {
        { IDX(-1,-1, 0), IDX(-1, 0,-1), IDX(-1,-1,-1) }, // bottom-back
        { IDX(-1,-1, 0), IDX(-1, 0, 1), IDX(-1,-1, 1) }, // bottom-front
        { IDX(-1, 1, 0), IDX(-1, 0, 1), IDX(-1, 1, 1) }, // top-front
        { IDX(-1, 1, 0), IDX(-1, 0,-1), IDX(-1, 1,-1) }  // top-back
    },
    // Right face (n = 1,0,0)  u = (0,1,0), v = (0,0,1)
    {
        { IDX( 1,-1, 0), IDX( 1, 0,-1), IDX( 1,-1,-1) },
        { IDX( 1,-1, 0), IDX( 1, 0, 1), IDX( 1,-1, 1) },
        { IDX( 1, 1, 0), IDX( 1, 0, 1), IDX( 1, 1, 1) },
        { IDX( 1, 1, 0), IDX( 1, 0,-1), IDX( 1, 1,-1) }
    },
    // Down face  (n = 0,-1,0) u = (1,0,0), v = (0,0,1)
    {
        { IDX(-1,-1, 0), IDX( 0,-1,-1), IDX(-1,-1,-1) }, // left-back
        { IDX(-1,-1, 0), IDX( 0,-1, 1), IDX(-1,-1, 1) }, // left-front
        { IDX( 1,-1, 0), IDX( 0,-1, 1), IDX( 1,-1, 1) }, // right-front
        { IDX( 1,-1, 0), IDX( 0,-1,-1), IDX( 1,-1,-1) }  // right-back
    },
    // Up face    (n = 0,1,0)  u = (1,0,0), v = (0,0,1)
    {
        { IDX(-1, 1, 0), IDX( 0, 1,-1), IDX(-1, 1,-1) },
        { IDX(-1, 1, 0), IDX( 0, 1, 1), IDX(-1, 1, 1) },
        { IDX( 1, 1, 0), IDX( 0, 1, 1), IDX( 1, 1, 1) },
        { IDX( 1, 1, 0), IDX( 0, 1,-1), IDX( 1, 1,-1) }
    },
    // Back face  (n = 0,0,-1) u = (1,0,0), v = (0,1,0)
    {
        { IDX(-1, 0,-1), IDX( 0,-1,-1), IDX(-1,-1,-1) }, // left-down
        { IDX(-1, 0,-1), IDX( 0, 1,-1), IDX(-1, 1,-1) }, // left-up
        { IDX( 1, 0,-1), IDX( 0, 1,-1), IDX( 1, 1,-1) }, // right-up
        { IDX( 1, 0,-1), IDX( 0,-1,-1), IDX( 1,-1,-1) }  // right-down
    },
    // Front face (n = 0,0,1)  u = (1,0,0), v = (0,1,0)
    {
        { IDX(-1, 0, 1), IDX( 0,-1, 1), IDX(-1,-1, 1) },
        { IDX(-1, 0, 1), IDX( 0, 1, 1), IDX(-1, 1, 1) },
        { IDX( 1, 0, 1), IDX( 0, 1, 1), IDX( 1, 1, 1) },
        { IDX( 1, 0, 1), IDX( 0,-1, 1), IDX( 1,-1, 1) }
    }
};

// Compute AO darkening factor based on three neighbor states (side1, side2, corner)
static inline float compute_ao_factor_27(byte side1, byte side2, byte corner) {
    // If both side neighbors are solid, use the strongest occlusion (corner darkness).
    if (side1 && side2) {
        return ao_corner_darkness;
    }
    byte solid_count = side1 + side2 + corner;
    if (solid_count == 3) {
        return ao_corner_darkness;
    } if (solid_count == 2) {
        return ao_edge_darkness;
    } else if (solid_count == 1) {
        return ao_small_darkness;
    } else {
        return ao_no_neighbors;
    }
}

// Add face colors using AO from the 27-byte neighbor array
void add_voxel_face_colors_ao_27(color_rgb_array_d* color_rgbs, color_rgb voxel_color, byte direction, const byte* neighbors) {
    for (byte a = 0; a < 4; a++) {
        color_rgb c = voxel_color;
        byte n1 = neighbors[ neighbor_indices_27[direction][a][0] ];
        byte n2 = neighbors[ neighbor_indices_27[direction][a][1] ];
        byte n3 = neighbors[ neighbor_indices_27[direction][a][2] ];
        float ao = compute_ao_factor_27(n1, n2, n3);
        // apply AO * multiplier and clamp to 0..255
        float fr = (float) c.r * ao * AO_MULTIPLIER;
        float fg = (float) c.g * ao * AO_MULTIPLIER;
        float fb = (float) c.b * ao * AO_MULTIPLIER;
        c.r = (fr > 255.0f) ? 255 : (byte)fr;
        c.g = (fg > 255.0f) ? 255 : (byte)fg;
        c.b = (fb > 255.0f) ? 255 : (byte)fb;
        add_to_color_rgb_array_d(color_rgbs, c);
    }
}

#undef IDX
