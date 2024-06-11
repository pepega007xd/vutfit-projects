import argparse


def check_argv():
    """
    Checks the syntax of arguments, generates Usage and Help messages
    """
    # the argparse library cannot retain the order of arguments, we will only
    # use this to check the syntax of arguments and generate a help message
    parser = argparse.ArgumentParser(
        description="Convert IPPcode24 into XML representation.",
        allow_abbrev=False
    )

    parser.add_argument("--stats", action="append", nargs=1,
                        help="File to write the following stats into")

    printing_stats = parser.add_argument_group(
        description="""Each of these options specifies an item to be printed
        into the current stats file (specified by --stats). These options
        cannot be used before specifying the stats file before them. Stats file
        can be specified multiple times.""")

    printing_stats.add_argument("--loc", action="count",
                                help="Number of instructions")
    printing_stats.add_argument("--comments", action="count",
                                help="Number of comments")
    printing_stats.add_argument("--jumps", action="count",
                                help="Number of jump instructions")
    printing_stats.add_argument("--fwjumps", action="count",
                                help="Number of forward jump instructions")
    printing_stats.add_argument("--backjumps", action="count",
                                help="Number of backward jump instructions")
    printing_stats.add_argument("--badjumps", action="count",
                                help="Number of jumps to nonexistent labels")
    printing_stats.add_argument("--frequent", action="count",
                                help="Most used instructions")

    printing_stats.add_argument("--print", action="append", nargs=1,
                                help="Prints given string into statistics")
    printing_stats.add_argument("--eol", action="count",
                                help="Prints a newline into statistics")
    parser.parse_args()
