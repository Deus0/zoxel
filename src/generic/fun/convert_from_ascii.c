#define zoxel_convert_ascii_n(c, n) if (input == c) { return n; }
#define zoxel_convert_ascii(c) zoxel_convert_ascii_n(c, i) i++;

byte convert_ascii(char input) {

    zoxel_convert_ascii_n('\n', zox_char_newline)

    int i = 0;
    zoxel_convert_ascii('?')

    // lower case
    zoxel_convert_ascii('a')
    zoxel_convert_ascii('b')
    zoxel_convert_ascii('c')
    zoxel_convert_ascii('d')
    zoxel_convert_ascii('e')
    zoxel_convert_ascii('f')
    zoxel_convert_ascii('g')
    zoxel_convert_ascii('h')
    zoxel_convert_ascii('i')
    zoxel_convert_ascii('j')
    zoxel_convert_ascii('k')
    zoxel_convert_ascii('l')
    zoxel_convert_ascii('m')
    zoxel_convert_ascii('n')
    zoxel_convert_ascii('o')
    zoxel_convert_ascii('p')
    zoxel_convert_ascii('q')
    zoxel_convert_ascii('r')
    zoxel_convert_ascii('s')
    zoxel_convert_ascii('t')
    zoxel_convert_ascii('u')
    zoxel_convert_ascii('v')
    zoxel_convert_ascii('w')
    zoxel_convert_ascii('x')
    zoxel_convert_ascii('y')
    zoxel_convert_ascii('z')

    // upper case
    zoxel_convert_ascii('A')
    zoxel_convert_ascii('B')
    zoxel_convert_ascii('C')
    zoxel_convert_ascii('D')
    zoxel_convert_ascii('E')
    zoxel_convert_ascii('F')
    zoxel_convert_ascii('G')
    zoxel_convert_ascii('H')
    zoxel_convert_ascii('I')
    zoxel_convert_ascii('J')
    zoxel_convert_ascii('K')
    zoxel_convert_ascii('L')
    zoxel_convert_ascii('M')
    zoxel_convert_ascii('N')
    zoxel_convert_ascii('O')
    zoxel_convert_ascii('P')
    zoxel_convert_ascii('Q')
    zoxel_convert_ascii('R')
    zoxel_convert_ascii('S')
    zoxel_convert_ascii('T')
    zoxel_convert_ascii('U')
    zoxel_convert_ascii('V')
    zoxel_convert_ascii('W')
    zoxel_convert_ascii('X')
    zoxel_convert_ascii('Y')
    zoxel_convert_ascii('Z')

    // special
    i = 55;
    zoxel_convert_ascii(' ')
    zoxel_convert_ascii('.')
    zoxel_convert_ascii(',')
    zoxel_convert_ascii('/')
    zoxel_convert_ascii('\\')

    // numbers
    zoxel_convert_ascii('0')
    zoxel_convert_ascii('1')
    zoxel_convert_ascii('2')
    zoxel_convert_ascii('3')
    zoxel_convert_ascii('4')
    zoxel_convert_ascii('5')
    zoxel_convert_ascii('6')
    zoxel_convert_ascii('7')
    zoxel_convert_ascii('8')
    zoxel_convert_ascii('9')

    // more special
    zoxel_convert_ascii('_')
    zoxel_convert_ascii('[')
    zoxel_convert_ascii(']')
    zoxel_convert_ascii('-')
    zoxel_convert_ascii('+')
    zoxel_convert_ascii('=')
    zoxel_convert_ascii('%')
    zoxel_convert_ascii('!')
    zoxel_convert_ascii('>')
    zoxel_convert_ascii('<')
    zoxel_convert_ascii(':')

    return 0;
}

byte* convert_from_ascii_text_at(const char *input, byte start_buffer) {
    byte length = start_buffer + strlen(input);
    byte *output = malloc(length);
    for (byte i = start_buffer; i < length; i++) {
        byte input_index = i - start_buffer;
        output[i] = convert_ascii(input[input_index]);
        // printf("Converting Ascii to byte: %i : %i : %c\n", i, output[i], input[input_index]);
    }
    return output;
}
