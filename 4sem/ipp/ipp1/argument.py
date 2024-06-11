from enum import Enum
from utils import ReturnCode, print_error_exit, xml_escape


def check_ident(ident: str) -> bool:
    """
    checks if the string is a valid identifier:

    - first character is either alpha, or special char
    - all other characters are either alphanumeric, or special char
    """

    if ident == "":
        return False
    special_chars = ["_", "-", "$", "&", "%", "*", "!", "?"]
    [first, *rest] = list(ident)

    # check first characteInt
    if not (first.isalpha() or special_chars.count(first) > 0):
        return False

    # check all other characters
    return all(map(
        lambda char: char.isalnum() or special_chars.count(char) > 0, rest
    ))


def check_string(string: str) -> bool:
    """
    checks if the string is a valid string literal:

    - must be valid UTF-8 (guaranteed by python's 'str' type)
    - non-printable characters, whitespace, '#', and backslash must be escaped
    by \\xyz, (single backslash), where xyz are exactly three decimal digits
    """

    chars = map(ord, list(string))
    # check that there are no non-printable chars or '#'
    if any(map(lambda char: char < 32 or char == 35, chars)):
        return False

    # check that there are always three decimal digits after \
    for i in range(len(string)):
        if string[i] == "\\":
            digits = list(string[i+1:i+4])

            if len(digits) != 3:
                return False

            if not all(map(lambda char: char.isdecimal(), digits)):
                return False

    return True


class ArgumentType(Enum):
    """
    Enum representing all possible types of arguments
    """
    # special variant for checking instruction format, implemented in __eq__
    Symb = -1

    Type = 1
    Label = 2
    Var = 3
    Int = 4
    Bool = 5
    String = 6
    Nil = 7

    def __eq__(self, other) -> bool:
        """
        Variant `Symb` should be equal to any value or variable
        """
        match self.name, other.name:
            case lhs, rhs if lhs == rhs: return True
            case "Symb", ("Var" | "Int" | "Bool" | "String" | "Nil"):
                return True
            case ("Var" | "Int" | "Bool" | "String" | "Nil"), "Symb":
                return True
            case _: return False


Symb = ArgumentType.Symb

Var = ArgumentType.Var
Type = ArgumentType.Type
Label = ArgumentType.Label
Int = ArgumentType.Int
Bool = ArgumentType.Bool
String = ArgumentType.String
Nil = ArgumentType.Nil


class Argument:
    """
    Represents a single instruction argument
   """
    content: str
    type: ArgumentType

    def __init__(self, arg: str):
        """
        Parses an argument
        """
        # arguments without '@'
        if arg.find("@") == -1:
            match arg:
                case "int" | "string" | "bool":
                    self.content = arg
                    self.type = Type
                    return

                case label if check_ident(label):
                    self.content = arg
                    self.type = Label
                    return

                case other:
                    print_error_exit(ReturnCode.lex_syntax_error,
                                     "Invalid identifier")

        # arguments with '@'
        kind, content = arg.split("@", maxsplit=1)
        self.content = content

        match kind:
            case ("GF" | "LF" | "TF") if check_ident(content):
                # keep the frame indentifier in the content
                self.content = arg
                self.type = Var

            case "string" if check_string(content):
                self.type = String

            case "int" if len(content) > 0:
                self.type = Int

            case "bool" if content == "true" or content == "false":
                self.type = Bool

            case "nil" if content == "nil":
                self.type = Nil

            case other:
                print_error_exit(ReturnCode.lex_syntax_error,
                                 f"Invalid instruction argument: '{other}'")

    def to_xml(self, order: int) -> str:
        type_name = self.type.name.lower()
        content = xml_escape(self.content)
        return f"""
        <arg{order} type="{type_name}">{content}</arg{order}>\
        """
