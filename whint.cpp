/*
    WHINT - Whitespace Interpreter
    Version: 1.0
    Author: Adam Bac (abac00s)

    copyright 2015 Adam Bac

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


    Usage: whint <name of file to execute>

    You can learn more about Whitespace language on
    http://compsoc.dur.ac.uk/whitespace/tutorial.html
    The following aspects of the language are not specified on that website, so I specify them here:
	- Firstly, all non-blank characters are treated as comments and ignored, even if they are in the middle of the command - for example [SPACE]abcd[SPACE][TAB] means the same what [SPACE][SPACE][TAB] or [SPACE][SPACE][TAB].
	- Secondly, heap and I/O operations which take parameters from the stack DON'T discard them but leave them on the stack.


*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>

/*
    This value is only arbirary - you can change it, if you need bigger heap.
    Each heap element is 4B, so the actual size of the heap is HEAP_SIZE*4B
*/
#define HEAP_SIZE 1024


char usage_str[] = "WHINT - Whitespace Interpreter v1.0\nAuthor: Adam Bac (abac00s)\n\nUsage: whint <name of file to execute>\n";

char *pr;


/*
    Displays information about unknown kommand.
*/
inline void command_error(unsigned int p) {
    printf("Error: unexpected char on position %u\n", p);
    delete [] pr;
    exit(0);
}

/*
    Sets p to the next white character or returns false if reaches end of program.
*/
inline bool find_next_white_char(unsigned int & p) {
    p++;
    while(pr[p] != ' ' && pr[p] != '\n' && pr[p] != '\t' && pr[p] != '\0') p++;
    return (pr[p] != '\0');
}

/*
    The same as above, but displays error if reaches the end of the program.
*/
inline void find_next_white_char_or_error(unsigned int & p) {
    if(!find_next_white_char(p)) {
        printf("Error: the last command is unfinished\n");
        delete [] pr;
        exit(0);
    }
}

/*
    Reads number from program pr, at position p.
    First character is sign o a number ([SPACE] -> +, [TAB] -> -). This is not ZU(2)!
    The rest o the characters is absolute value of number in binary system ([SPACE] -> 0, [TAB] -> 1).
    The number is terminated by Line Feed character.
    Examples:
    10: [SPACE][TAB][SPACE][TAB][SPACE][LF]
    -8: [TAB][TAB][SPACE][SPACE][SPACE][LF]

    This is an early version, so it doesn't check if the number coding is correct.
*/
int read_number(unsigned int & p) {
	int ret = 0;
	bool neg = (pr[p] == '\t');
	find_next_white_char_or_error(p);
	while(pr[p] != '\n') {
		ret <<= 1;
		if(pr[p] == '\t') ret++;
		find_next_white_char_or_error(p);
	}
	find_next_white_char(p);
	return neg ? -ret : ret;
}

int main(int argc, char **argv) {
    if(argc < 2) {
        puts(usage_str);
        exit(0);
    }

	FILE *f = fopen(argv[1], "r");
	if(f == NULL) {
        printf("Couldn't open the file %s\n", argv[1]);
        exit(0);
	}
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	rewind(f);

	std::vector <int> heap(HEAP_SIZE);
	std::map <int, unsigned int> labels;
	std::stack <int> prg_stack;
	std::stack <unsigned int> call_stack;
	std::fill(heap.begin(), heap.end(), 0);

    pr = new char[size + 1];
	fread(pr, 1, size, f);
	pr[size] = '\0';
	fclose(f);

	unsigned int p = 0;
	while(pr[p]) {
		if(pr[p] == ' ') {
			//Stack manipulation
			find_next_white_char_or_error(p);
			if(pr[p] == ' ') {
                //Push a value to the stack
				find_next_white_char_or_error(p);
				int a = read_number(p);
				prg_stack.push(a);
			}
			else if(pr[p] == '\n') {
				find_next_white_char_or_error(p);
				if(pr[p] == ' ') {
				    //Duplicate value on the top
					int a = prg_stack.top();
                    prg_stack.push(a);
					find_next_white_char(p);
				}
				else if(pr[p] == '\t') {
                    //Swap values on the top
					int a = prg_stack.top();
					prg_stack.pop();
					int b = prg_stack.top();
					prg_stack.top() = a;
					prg_stack.push(b);
					find_next_white_char(p);
				}
				else if(pr[p] == '\n') {
					//Discard value on the value
					prg_stack.pop();
					find_next_white_char(p);
				}
			}
			else command_error(p);
		}

		else if(pr[p] == '\t') {
			find_next_white_char_or_error(p);
			if(pr[p] == ' ') {
				//Arithmetic
				find_next_white_char_or_error(p);
				if(pr[p] == ' ') {
                    find_next_white_char_or_error(p);
                    if(pr[p] == ' ') {
                        //Add
                        int a = prg_stack.top();
                        prg_stack.pop();
                        prg_stack.top() += a;
                        find_next_white_char(p);
                    }
                    else if(pr[p] == '\t') {
                        //Subtract
                        int a = prg_stack.top();
                        prg_stack.pop();
                        prg_stack.top() -= a;
                        find_next_white_char(p);
                    }
                    else if(pr[p] == '\n') {
                        //Multiply
                        int a = prg_stack.top();
                        prg_stack.pop();
                        prg_stack.top() *= a;
                        find_next_white_char(p);
                    }
				}
				else if(pr[p] == '\t') {
                    find_next_white_char_or_error(p);
                    if(pr[p] == ' ') {
                        //Divide
                        int a = prg_stack.top();
                        prg_stack.pop();
                        prg_stack.top() /= a;
                        find_next_white_char(p);
                    }
                    else if(pr[p] == '\t') {
                        //Modulo
                        int a = prg_stack.top();
                        prg_stack.pop();
                        prg_stack.top() %= a;
                        find_next_white_char(p);
                    }
                    else command_error(p);
				}
				else command_error(p);
			}

			else if(pr[p] == '\t') {
                //Heap operations
                find_next_white_char_or_error(p);
                if(pr[p] == ' ') {
                    //Put a value onto the heap
                    int val = prg_stack.top();
                    prg_stack.pop();
                    heap[prg_stack.top()] = val;
                    prg_stack.push(val);
                    find_next_white_char(p);
                }
                else if(pr[p] == '\t') {
                    //Retrieve a value from the heap
                    int val = heap[prg_stack.top()];
                    prg_stack.push(val);
                    find_next_white_char(p);
                }
                else command_error(p);
			}

			else if(pr[p] == '\n') {
                //I/O
                find_next_white_char_or_error(p);
                if(pr[p] == ' ') {
                    find_next_white_char_or_error(p);
                    if(pr[p] == ' ') {
                        //Print a char from the stack
                        putchar(prg_stack.top());
                        find_next_white_char(p);
                    }
                    else if(pr[p] == '\t') {
                        //Print a number from the stack
                        printf("%d", prg_stack.top());
                        find_next_white_char(p);
                    }
                    else command_error(p);
                }
                else if(pr[p] == '\t') {
                    find_next_white_char_or_error(p);
                    if(pr[p] == ' ') {
                        //Read a char to the heap
                        heap[prg_stack.top()] = getchar();
                        find_next_white_char(p);
                    }
                    else if(pr[p] == '\t') {
                        //Read a number to the heap
                        scanf("%u", &heap[prg_stack.top()]);
                        find_next_white_char(p);
                    }
                    else command_error(p);
                }
                else command_error(p);
			}
		}

		else if(pr[p] == '\n') {
            //Flow control
            find_next_white_char_or_error(p);
            if(pr[p] == ' ') {
                find_next_white_char_or_error(p);
                if(pr[p] == ' ') {
                    //Mark a label
                    find_next_white_char_or_error(p);
                    int label = read_number(p);
                    labels[label] = p;
                }
                else if(pr[p] == '\t') {
                    //Call a subroutine
                    find_next_white_char_or_error(p);
                    int label = read_number(p);
                    call_stack.push(p);
                    p = labels[label];
                }
                else if(pr[p] == '\n') {
                    //Jump unconditionally
                    find_next_white_char_or_error(p);
                    int label = read_number(p);
                    p = labels[label];
                }
            }
            else if(pr[p] == '\t') {
                find_next_white_char_or_error(p);
                if(pr[p] == ' ') {
                    //Jump if number on the top of the stack is 0
                    find_next_white_char_or_error(p);
                    int label = read_number(p);
                    if(prg_stack.top() == 0) p = labels[label];
                }
                else if(pr[p] == '\t') {
                    //Jump if a number on the top of the stack is negative
                    find_next_white_char_or_error(p);
                    int label = read_number(p);
                    if(prg_stack.top() < 0) p = labels[label];
                }
                else if(pr[p] == '\n') {
                    //Return from a subroutine
                    p = call_stack.top();
                    call_stack.pop();
                }
            }

            else if(pr[p] == '\n' && pr[p + 1] == '\n') break;  //End program
            else command_error(p);
		}

        if(pr[p] != ' ' && pr[p] != '\n' && pr[p] != '\t' && pr[p] != '\0')
            find_next_white_char(p);
	}


	return 0;
}
