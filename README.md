# highlight

highlight v0.1.1

Usage: highlight [OPTIONS...] [FILE]

Example: highlight -p ERROR red -p WARN yellow debug.log

FILE            a path to an input text file

OPTIONS

  -p, --pattern REGEX COLOR

        REGEX   Regular expression (extended)

        COLOR   Color to be used for highlighting of the given REGEX (one of red, green, yellow, blue, magneta, cyan, white; you can also use just the first char, e.g. red=r)

  -h, --help                    display this help and exit

  -b, --background              highlight background

  -s, --selection-only          highlight only matched parts of the line, not the whole line

  -i, --ignore-case             ignore case in regex match
