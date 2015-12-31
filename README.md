# WHINT
WHINT is the interpreter of esoteric language Whitespace - to learn about this language visit http://compsoc.dur.ac.uk/whitespace/tutorial.html. The following aspects of the language are not specified on that website, so I specify them here:
- Firstly, all non-blank characters are treated as comments and ignored, even if they are in the middle of the command - for example [SPACE]abcd[SPACE][TAB] means the same what [SPACE][SPACE][TAB] or [SPACE][SPACE][TAB].
- Secondly, heap and I/O operations which take parameters from the stack DON'T discard them but leave them on the stack.

To change the default size of the heap (1024 elements), change HEAP_SIZE value in the source code.

## Installation
### Windows
Requirements:
- MinGW compiler - http://www.mingw.org/

Compile the file whint.cpp using command:

g++ whint.cpp -o whint.exe

Run whint.exe from command line.

Note: You can use other compiler if you want.

### Linux
Requirements:
- GCC compiler - https://gcc.gnu.org/

Compile the file whint.cpp using command:

g++ whint.cpp -o whint

Run whint from command line.

## Usage
whint [name of file to run]

For example:

whint program.wh
  
HAVE FUN!
