//#define zoxel_convert_to_ascii_start(char, number) if (input == number) return char;

//#define zoxel_convert_to_ascii(char, number) else if (input == number) return char;

#define zoxel_convert_to_ascii_n(c, n) if (input == n) { return c; }
#define zoxel_convert_to_ascii(c) zoxel_convert_to_ascii_n(c, i) i++;

char convert_to_ascii(byte input) {

    zoxel_convert_to_ascii_n('\n', zox_char_newline)

    int i = 0;
    zoxel_convert_to_ascii('?')

    // lower case
    zoxel_convert_to_ascii('a')
    zoxel_convert_to_ascii('b')
    zoxel_convert_to_ascii('c')
    zoxel_convert_to_ascii('d')
    zoxel_convert_to_ascii('e')
    zoxel_convert_to_ascii('f')
    zoxel_convert_to_ascii('g')
    zoxel_convert_to_ascii('h')
    zoxel_convert_to_ascii('i')
    zoxel_convert_to_ascii('j')
    zoxel_convert_to_ascii('k')
    zoxel_convert_to_ascii('l')
    zoxel_convert_to_ascii('m')
    zoxel_convert_to_ascii('n')
    zoxel_convert_to_ascii('o')
    zoxel_convert_to_ascii('p')
    zoxel_convert_to_ascii('q')
    zoxel_convert_to_ascii('r')
    zoxel_convert_to_ascii('s')
    zoxel_convert_to_ascii('t')
    zoxel_convert_to_ascii('u')
    zoxel_convert_to_ascii('v')
    zoxel_convert_to_ascii('w')
    zoxel_convert_to_ascii('x')
    zoxel_convert_to_ascii('y')
    zoxel_convert_to_ascii('z')

    // upper case
    zoxel_convert_to_ascii('A')
    zoxel_convert_to_ascii('B')
    zoxel_convert_to_ascii('C')
    zoxel_convert_to_ascii('D')
    zoxel_convert_to_ascii('E')
    zoxel_convert_to_ascii('F')
    zoxel_convert_to_ascii('G')
    zoxel_convert_to_ascii('H')
    zoxel_convert_to_ascii('I')
    zoxel_convert_to_ascii('J')
    zoxel_convert_to_ascii('K')
    zoxel_convert_to_ascii('L')
    zoxel_convert_to_ascii('M')
    zoxel_convert_to_ascii('N')
    zoxel_convert_to_ascii('O')
    zoxel_convert_to_ascii('P')
    zoxel_convert_to_ascii('Q')
    zoxel_convert_to_ascii('R')
    zoxel_convert_to_ascii('S')
    zoxel_convert_to_ascii('T')
    zoxel_convert_to_ascii('U')
    zoxel_convert_to_ascii('V')
    zoxel_convert_to_ascii('W')
    zoxel_convert_to_ascii('X')
    zoxel_convert_to_ascii('Y')
    zoxel_convert_to_ascii('Z')

    // special
    i = 55;
    zoxel_convert_to_ascii(' ')
    zoxel_convert_to_ascii('.')
    zoxel_convert_to_ascii(',')
    zoxel_convert_to_ascii('/')
    zoxel_convert_to_ascii('\\')

    // numbers
    zoxel_convert_to_ascii('0')
    zoxel_convert_to_ascii('1')
    zoxel_convert_to_ascii('2')
    zoxel_convert_to_ascii('3')
    zoxel_convert_to_ascii('4')
    zoxel_convert_to_ascii('5')
    zoxel_convert_to_ascii('6')
    zoxel_convert_to_ascii('7')
    zoxel_convert_to_ascii('8')
    zoxel_convert_to_ascii('9')

    // more special
    zoxel_convert_to_ascii('_')
    zoxel_convert_to_ascii('[')
    zoxel_convert_to_ascii(']')
    zoxel_convert_to_ascii('-')
    zoxel_convert_to_ascii('+')
    zoxel_convert_to_ascii('=')
    zoxel_convert_to_ascii('%')
    zoxel_convert_to_ascii('!')
    zoxel_convert_to_ascii('>')
    zoxel_convert_to_ascii('<')
    zoxel_convert_to_ascii(':')

    zoxel_convert_to_ascii('?')

    return ' ';
}

char* convert_to_ascii_text(byte *input, int length) {
    char *output = malloc(sizeof(char) * length);
    for (int i = 0; i < length; i++) {
        output[i] = convert_to_ascii(input[i]);
    }
    return output;
}

char* convert_to_ascii_text_at(byte *input, int length, byte at_index) {
    char *output = malloc(sizeof(char) * (length + 1));
    for (byte i = at_index; i < at_index + length; i++) {
        output[(i - at_index)] = convert_to_ascii(input[i]);
    }
    output[length] = '\0';
    return output;
}

// printf("Converting Ascii from byte: %i : %i : %c\n", i, input[i], output[output_index]);
