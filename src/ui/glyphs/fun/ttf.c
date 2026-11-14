#ifdef zox_lib_ttf
#include <ft2build.h>
#include FT_FREETYPE_H

float4 get_glyph_bounds(const FT_GlyphSlot glyph) {
    float4 bounds;
    if (glyph->outline.n_points == 0) return float4_zero;
    bounds.x = bounds.y = glyph->outline.points[0].x;
    bounds.z = bounds.w = glyph->outline.points[0].y;
    for (int i = 1; i < glyph->outline.n_points; i++) {
        const int x = glyph->outline.points[i].x;
        const int y = glyph->outline.points[i].y;
        if (x < bounds.x) bounds.x = x;
        if (x > bounds.y) bounds.y = x;
        if (y < bounds.z) bounds.z = y;
        if (y > bounds.w) bounds.w = y;
    }
    return bounds;
}

// the bounds of the face's letters, digits and underscore (x min, x max, y min, y max), by character code: points are
//  placed in them (glyph_point_to_byte2), the descenders (g j p q y) below the baseline included
float4 get_face_bounds(FT_Face face) {
    byte initialized = 0;
    float4 bounds = float4_zero;
    for (int c = 0; c < 128; c++) {
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_')) {
            continue;
        }
        if (FT_Load_Glyph(face, FT_Get_Char_Index(face, (FT_ULong) c), FT_LOAD_DEFAULT)) {
            continue;
        }
        const float4 glyph_bounds = get_glyph_bounds(face->glyph);
        if (!initialized) {
            initialized = 1;
            bounds = glyph_bounds;
            continue;
        }
        bounds.x = fminf(bounds.x, glyph_bounds.x);
        bounds.y = fmaxf(bounds.y, glyph_bounds.y);
        bounds.z = fminf(bounds.z, glyph_bounds.z);
        bounds.w = fmaxf(bounds.w, glyph_bounds.w);
    }
    return bounds;
}

int2 get_glyph_size(const FT_GlyphSlot glyph) {
    return (int2) { glyph->metrics.width, glyph->metrics.height };
}

float2 get_glyph_margins(const FT_GlyphSlot glyph) {
    return (float2) { glyph->metrics.horiBearingX, glyph->metrics.horiBearingY };
}

// a point in the face's bounds (get_face_bounds) to 0 to 255 inside the font margins: below the baseline too (a
//  descender's tail was negative, it wrapped to the top as a byte, g and y read as 8 and 9)
byte2 glyph_point_to_byte2(FT_Vector point_, const int2 glyph_size, const float4 bounds) {
    float2 point = {
        (point_.x - bounds.x) / (bounds.y - bounds.x),
        (point_.y - bounds.z) / (bounds.w - bounds.z)
    };
    point.x = 255 * (point.x * (1 - point_font_margins * 2) + point_font_margins);
    point.y = 255 * (point.y * (1 - point_font_margins * 2) + point_font_margins);
    return (byte2) { (byte) int_clamp((int) point.x, 0, 255), (byte) int_clamp((int) point.y, 0, 255) };
}

FT_UInt get_glyph_index(FT_Face face, const char charcode) {
    return FT_Get_Char_Index(face, (FT_ULong) charcode);
}

FT_GlyphSlot get_glyph(FT_Face face, const char charcode) {
    const FT_UInt index = get_glyph_index(face, charcode);
    if (FT_Load_Glyph(face, index, FT_LOAD_DEFAULT)) {
        fprintf(stderr, "Error: Could not load glyph %d\n", index);
        return 0;
    }
    return face->glyph;
}

byte2* glyph_to_points(const FT_GlyphSlot glyph, const float4 face_bounds, int *length) {
    const int2 glyph_size = get_glyph_size(glyph);
    // const float2 glyph_margins = get_glyph_margins(glyph);
    // const float4 glyph_bounds = get_glyph_bounds(glyph);
    *length = 0;
    int start_point_index = 0;
    for (int c = 0; c < glyph->outline.n_contours; c++) {
        int contour_end = glyph->outline.contours[c];
        for (int i = start_point_index; i <= contour_end; i++) {
            *length += 2;
        }
        start_point_index = contour_end + 1;
    }
    byte2 *points = malloc(*length * sizeof(byte2));
    int k = 0;
    start_point_index = 0;
    for (int c = 0; c < glyph->outline.n_contours; c++) {
        int contour_end = glyph->outline.contours[c];
        for (int i = start_point_index; i <= contour_end; i++) {
            FT_Vector point_a = glyph->outline.points[i];
            FT_Vector point_b;
            if (i == contour_end) point_b = glyph->outline.points[start_point_index];
            else point_b = glyph->outline.points[i + 1];
            points[k] = glyph_point_to_byte2(point_a, glyph_size, face_bounds);
            points[k + 1] = glyph_point_to_byte2(point_b, glyph_size, face_bounds);
            k += 2;
        }
        start_point_index = contour_end + 1;
    }
    return points;
}

void print_glyph(const FT_GlyphSlot glyph) {
    const int2 glyph_size = get_glyph_size(glyph);
    const float2 glyph_margins = get_glyph_margins(glyph);
    const float4 glyph_bounds = get_glyph_bounds(glyph);
    zox_log(" + glyph size [%ix%i]\n", glyph_size.x, glyph_size.y)
    zox_log(" + glyph margins [%fx%f]\n", glyph_margins.x, glyph_margins.y)
    zox_log(" + glyph bounds [%fx%f] [%fx%f]\n", glyph_bounds.x, glyph_bounds.y, glyph_bounds.z, glyph_bounds.w)
    zox_log(" + glyph points [%i]:\n", glyph->outline.n_points);
    for (int j = 0; j < glyph->outline.n_points; j++) {
        FT_Vector point = glyph->outline.points[j];
        byte2 converted_point = glyph_point_to_byte2(point, glyph_size, glyph_bounds);
        zox_log("    - [%i]: [%ld, %ld]\n", j, point.x, point.y)
        zox_log("       - [%i]: [%i, %i]\n", j, converted_point.x, converted_point.y)
    }
}

#endif

/*if (i >= 97 && i <= 122) j = i - 97 + 1; // lowercase
else if (i >= 65 && i <= 90) j = i - 65 + 27; // upper case
else if (i >= 48 && i <= 57) j = convert_ascii(i); // numbers
else if (i >= 33 && i <= 47) j = convert_ascii(i); // special characters
else if (i >= 58 && i <= 64) j = convert_ascii(i); // special characters*/
// print_glyph(get_glyph(face, '!'));
// print_glyph(get_glyph(face, 'I'));
// print_glyph(get_glyph(face, 'i'));
// print_glyph(get_glyph(face, 'g'));
// print_glyph(get_glyph(face, '.'));
// print_glyph(get_glyph(face, '^'));
// print_glyph(get_glyph(face, 'D'));

/*int start_point_index = 0;
for (int c = 0; c < glyph->outline.n_contours; c++) {
    int contour_end = glyph->outline.contours[c];
    zox_log("   - shape index [%i] to [%i]\n", start_point_index, contour_end)
    for (int i = start_point_index; i <= contour_end; i++) {
        FT_Vector point_a = glyph->outline.points[i];
        FT_Vector point_b;
        if (i == contour_end) point_b = glyph->outline.points[start_point_index];
        else point_b = glyph->outline.points[i + 1];
        zox_log("       - line [%ix%i] to [%ix%i]\n", point_a.x, point_a.y, point_b.x, point_b.y)
    }
    start_point_index = contour_end + 1;
}*/
