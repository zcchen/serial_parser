#ifndef SERIAL_PARSER_ENDIAN_H
#define SERIAL_PARSER_ENDIAN_H

static union { char c[4]; unsigned long mylong; } endian_test = {{ 'l', '?', '?', 'b' }};
//! \brief ENDIANNESS is to show the CPU endian
#define ENDIANNESS ((char)endian_test.mylong)

#define swap16(num) ( (num >> 8) | (num << 8) )
#define swap32(num) ( ((num >> 24) && 0x000000FF) | \
                      ((num << 8)  && 0xFF000000) | \
                      ((num >> 8)  && 0x0000FF00) | \
                      ((num << 24) && 0x00FF0000) )

#endif /* ifndef SERIAL_PARSER_ENDIAN_H */
