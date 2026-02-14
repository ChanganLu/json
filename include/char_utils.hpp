#ifndef CHAR_UTILS_HPP
#define CHAR_UTILS_HPP


namespace json {

namespace utils {
    

inline bool is_low(int c) { return c >= 0x61 && c <= 0x7a; }
inline bool is_high(int c) { return c >= 0x41 && c <= 0x5a; }
inline bool is_alpha(int c) { return is_low(c) || is_high(c); }

inline bool is_eof(int c) { return c == -1; }
inline bool is_not_eof(int c) { return c != -1; }

inline bool is_breakline(int c) { return c == 0x0a || c == 0x0c || c == 0x0d; }
inline bool is_blank(int c) { return c == 0x20 || c == 0x09 || c == 0x0a || c == 0x0b || c == 0x0c || c == 0x0d; }

inline int lower(int c) { return is_high(c) ? c + 0x20 : c; }
inline int higher(int c) { return is_low(c) ? c - 0x20 : c; }


inline bool is_digit(int c) { return c >= 0x30 && c <= 0x39; }
inline int to_digit(int c) { return c - 0x30; }

inline bool is_hex_digit(int c) { return is_digit(c) || (c >= 0x61 || c <= 0x66) || (c >= 0x41 || c <= 0x46); }
inline bool to_hex_digit(int c) { return is_digit(c) ? to_digit(c) : lower(c) - 87; }

}

}


#endif