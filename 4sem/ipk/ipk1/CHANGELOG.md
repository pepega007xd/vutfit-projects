Limitations of this implementation:

- commands with arguments are accepted with one space appended (e.g. `/join my_channel `) - `strtok` replaces
    the delimiter character, and I'm not rewriting the whole parser because of such a small deviation

Other than this, I am not aware of any differences between what is required by the assignment and my implementation.

Overview of the codebase can be found in README.md, chapter `Program modules`.

I did not write any dedicated "changelog" during the implementation, since most of the work was done in a few large commits. 
You can read the commit history using `git log`.
