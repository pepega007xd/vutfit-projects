from argument import Argument, ArgumentType, Var, Symb, Label, Type
from utils import ReturnCode, print_error_exit

# mapping from each valid instruction to the list of its argument's types
INSTRUCTIONS: dict[str, list[ArgumentType]] = {
    "MOVE": [Var, Symb],
    "CREATEFRAME": [],
    "PUSHFRAME": [],
    "POPFRAME": [],
    "DEFVAR": [Var],
    "CALL": [Label],
    "RETURN": [],
    "PUSHS": [Symb],
    "POPS": [Var],
    "ADD": [Var, Symb, Symb],
    "SUB": [Var, Symb, Symb],
    "MUL": [Var, Symb, Symb],
    "IDIV": [Var, Symb, Symb],
    "LT": [Var, Symb, Symb],
    "GT": [Var, Symb, Symb],
    "EQ": [Var, Symb, Symb],
    "AND": [Var, Symb, Symb],
    "OR": [Var, Symb, Symb],
    "NOT": [Var, Symb],
    "INT2CHAR": [Var, Symb],
    "STRI2INT": [Var, Symb, Symb],
    "READ": [Var, Type],
    "WRITE": [Symb],
    "CONCAT": [Var, Symb, Symb],
    "STRLEN": [Var, Symb],
    "GETCHAR": [Var, Symb, Symb],
    "SETCHAR": [Var, Symb, Symb],
    "TYPE": [Var, Symb],
    "LABEL": [Label],
    "JUMP": [Label],
    "JUMPIFEQ": [Label, Symb, Symb],
    "JUMPIFNEQ": [Label, Symb, Symb],
    "EXIT": [Symb],
    "DPRINT": [Symb],
    "BREAK": [],
}


class Instruction:
    """
    Represents a single instruction
    """
    opcode: str
    order: int
    arguments: list[Argument]

    def __init__(self, order, line):
        """
        Parses an instruction, does type checks of its arguments
        according to `INSTRUCTIONS`
        """
        [opcode, *arguments_str] = line.split()
        self.opcode = opcode.upper()

        # instructions are counted from 1
        self.order = order + 1

        arguments = list(map(Argument, arguments_str))

        parameters = INSTRUCTIONS.get(opcode.upper())
        if parameters is None:
            if opcode.isalnum():
                print_error_exit(ReturnCode.unknown_opcode_error,
                                 f"Unknown instruction: '{opcode}'")
            else:
                print_error_exit(ReturnCode.lex_syntax_error,
                                 f"Syntax error: '{opcode}'")

        if len(arguments) != len(parameters):
            print_error_exit(ReturnCode.lex_syntax_error,
                             "Wrong number of arguments")

        for i, (param, arg) in enumerate(zip(parameters, arguments)):
            # type identifier is also a valid label, we must cast it manually
            if param == Label and arg.type == Type:
                arguments[i].type = Label
                continue

            if param != arg.type:
                print_error_exit(ReturnCode.lex_syntax_error,
                                 f"Wrong type of argument: {line}")

        self.arguments = arguments

    def to_xml(self) -> str:
        args = map(lambda arg: arg[1].to_xml(
            arg[0] + 1), enumerate(self.arguments))

        args_str = "".join(args)

        return f"""
    <instruction order="{self.order}" opcode="{self.opcode}">\
{args_str}
    </instruction>\
        """

    def is_jump(self):
        return ["CALL", "JUMP", "JUMPIFEQ", "JUMPIFNEQ",
                "RETURN"].count(self.opcode) > 0
