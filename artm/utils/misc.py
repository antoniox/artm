from __future__ import absolute_import, print_function, division

__all__ = [
    'quote', 'dequote',
]

SPECIAL_CHARS = [
    ('\\', '\\\\'),
    ('\0', '\\0'),
    ('\r', '\\r'),
    ('\t', '\\t'),
    ('\n', '\\n'),
]


def quote(string):
    for char, quoted_char in SPECIAL_CHARS:
        if char in string:
            string = string.replace(char, quoted_char)

    return string


def dequote(string):
    for char, quoted_char in SPECIAL_CHARS:
        if quoted_char in string:
            string = string.replace(quoted_char, char)

    return string
