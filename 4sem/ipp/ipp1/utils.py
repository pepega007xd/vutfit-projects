from enum import Enum
from typing import NoReturn
from sys import stderr


class ReturnCode(Enum):
    success = 0
    no_stats_file_error = 10  # args for printing stats are before --stats
    input_file_error = 11  # error while opening an input file
    output_file_error = 12  # error while opening an output file
    missing_header_error = 21  # missing ".IPPcode24" at the start of file
    unknown_opcode_error = 22  # unknown name of instruction
    lex_syntax_error = 23  # other lexical or syntactic error
    internal_error = 99  # internal error not caused by user


def print_error_exit(code: ReturnCode, message: str) -> NoReturn:
    """
    Prints an error message and exits the program with provided return code
    """
    # pring error message in bold red
    print("\x1b[31;1m" + message + "\x1b[0m", file=stderr)
    exit(code.value)


def xml_escape(string: str) -> str:
    return string.replace("&", "&amp;") \
        .replace(">", "&gt;") \
        .replace("<", "&lt;")
