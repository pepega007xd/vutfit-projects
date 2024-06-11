from instruction import Instruction
from utils import ReturnCode, print_error_exit
import functools
from typing import Callable
from sys import argv
from collections import Counter
from io import TextIOWrapper


class Program:
    """
    Represents the whole source code
    """
    instructions: list[Instruction]
    number_of_comments: int

    def __init__(self, code: str):
        """
        Parses the source code, counts comments
        """
        lines = code.splitlines()

        self.number_of_comments = len(list(
            filter(lambda line: line.count('#') > 0, lines)))

        lines = list(
            # remove extra whitespace
            map(lambda line: line.strip(),
                # remove blank lines
                filter(lambda line: not (line.isspace() or line == ""),
                       # remove comments
                       map(lambda line: line.split('#')[0], lines))))

        if len(lines) == 0 or lines[0] != ".IPPcode24":
            print_error_exit(ReturnCode.missing_header_error,
                             "Missing '.IPPcode24' header")

        [_, *instructions_str] = lines

        self.instructions = list(
            map(lambda instr: Instruction(instr[0], instr[1]),
                enumerate(instructions_str)))

    def to_xml(self) -> str:
        instructions = "".join(
            map(lambda instr: instr.to_xml(), self.instructions))

        xml = f"""\
<?xml version="1.0" encoding="UTF-8"?>
<program language="IPPcode24">\
{instructions}
</program>\
        """

        return xml

    # the following methods correspond to the stats arguments,
    # documented in `argv.py`

    # memoization to avoid recomputing values
    @functools.cache
    def loc(self) -> int:
        return len(self.instructions)

    @functools.cache
    def comments(self) -> int:
        return self.number_of_comments

    @functools.cache
    def __get_labels(self) -> dict[str, int]:
        """
        Returns mapping from label names to their locations
        """
        label_instructions = list(
            filter(lambda instr: instr.opcode == "LABEL", self.instructions))

        labels = map(lambda label: (
            label.arguments[0].content, label.order), label_instructions)

        return dict(labels)

    @functools.cache
    def __get_jumps(self) -> dict[int, str]:
        """
        Returns mapping from jump source locations to their target labels
        """
        jump_instructions = list(
            filter(lambda instr: instr.is_jump() and instr.opcode != "RETURN",
                   self.instructions))

        jumps = map(lambda jump: (
            jump.order, jump.arguments[0].content), jump_instructions)

        return dict(jumps)

    @functools.cache
    def __direction_jumps(self, compare: Callable[[int, int], bool]) -> int:
        """
        Returns the number of forward/backward jumps
        based on provided comparison function
        """
        jumps = self.__get_jumps()
        labels = self.__get_labels()

        count = 0
        for jump_order, jump_target in jumps.items():
            label_order = labels.get(jump_target)
            if label_order is not None and compare(jump_order, label_order):
                count += 1

        return count

    @functools.cache
    def jumps(self) -> int:
        return len(self.__get_jumps())

    @functools.cache
    def fwjumps(self) -> int:
        return self.__direction_jumps(lambda jump, label: jump < label)

    @functools.cache
    def backjumps(self) -> int:
        return self.__direction_jumps(lambda jump, label: jump > label)

    @functools.cache
    def badjumps(self) -> int:
        jumps = set(self.__get_jumps().values())
        labels = set(self.__get_labels().keys())

        return len(jumps - labels)

    @functools.cache
    def frequent(self) -> str:
        counts = Counter(map(lambda instr: instr.opcode,
                         self.instructions)).most_common()
        if len(counts) == 0:
            return ""

        max_count = counts[0][1]
        most_used = filter(
            lambda instr_count: instr_count[1] == max_count, counts)

        return ",".join(sorted(map(lambda instr: instr[0], most_used)))

    def print_stats(self):
        """
        Iterates through arguments, opens files and writes stats,
        documented in `argv.py`
        """
        if len(argv) <= 1:
            return

        def open_file(filename: str) -> TextIOWrapper:
            try:
                outfile = open(filename, "w")
            except OSError:
                print_error_exit(
                    ReturnCode.output_file_error,
                    f"stats file {filename} cannot be opened for writing")

            return outfile

        if not argv[1].startswith("--stats="):
            print_error_exit(ReturnCode.no_stats_file_error,
                             f"{argv[1]} specified before --stats")

        outfile = open_file(argv[1].split("=", maxsplit=1)[1])

        for argument in argv:
            [argument_name, *content_opt] = argument.split("=", maxsplit=1)
            content = "".join(content_opt)

            match argument_name:
                case "--stats":
                    outfile.close()
                    outfile = open_file(content)

                case "--print":
                    outfile.write(content)

            match argument:
                case "--loc":
                    outfile.write(str(self.loc()) + "\n")
                case "--comments":
                    outfile.write(str(self.comments()) + "\n")
                case "--jumps":
                    outfile.write(str(self.jumps()) + "\n")
                case "--fwjumps":
                    outfile.write(str(self.fwjumps()) + "\n")
                case "--backjumps":
                    outfile.write(str(self.backjumps()) + "\n")
                case "--badjumps":
                    outfile.write(str(self.badjumps()) + "\n")
                case "--frequent":
                    outfile.write(self.frequent() + "\n")
                case "--eol":
                    outfile.write("\n")

        outfile.close()
