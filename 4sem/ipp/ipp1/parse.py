from utils import ReturnCode
from program import Program
from sys import stdin
from argv import check_argv


def main() -> ReturnCode:
    # check validity of arguments, print help if needed
    check_argv()

    # parse input, construct internal representation of input code
    program = Program(stdin.read())

    # print XML representation of input code
    print(program.to_xml())

    # just a precaution, error in non-essential functionality
    # should not affect the whole program
    try:
        # process arguments and write stats into files, if requested
        program.print_stats()
    except:  # noqa
        pass

    return ReturnCode.success


if __name__ == "__main__":
    exit(main().value)
