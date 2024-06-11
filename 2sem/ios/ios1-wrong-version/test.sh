#!/bin/bash

assert_editor() {
    read editor
    read empty
    [ "$editor" = "$1" ] || echo "TESTING FAIL: Assertion failed: got: $editor != expected: $1" >&2
    [ -z "$empty" ] || echo "TESTING FAIL: Extra line at output: $empty" >&2
}

assert_eq() {
    [ "$1" = "$2" ] || echo "TESTING FAIL: Assertion failed: \$1: $1 != \$2: $2" >&2
}

assert_ret() {
    echo "TESTING FAIL: Return code is not as expected"
}

reset_env() {
    prefix=/tmp/moletest
    null=/dev/null
    rm -rf $prefix
    mkdir $prefix
    cd $prefix

    export MOLE_RC=$prefix/molerc-testing

    export EDITOR=$prefix/dummyeditor
    echo 'echo "$1" ; exit 0' > $EDITOR
    chmod +x $EDITOR

    rm -rf ~/.mole 2> $null

    mkdir dir1
    touch dir1/file1
    touch dir1/file2

    mkdir dir2

    touch file1
    touch file2
    touch file3
}

reset_env
echo "basic open..."

mole file1 > null || assert_ret
mole file1 > null || assert_ret
mole file1 | assert_editor $prefix/file1

mole file2 | assert_editor $prefix/file2
mole kentus 2> null && assert_ret

mole | assert_editor $prefix/file2
mole -m | assert_editor $prefix/file1

reset_env
echo "invalid args and help..."

mole -e 2> $null && assert_ret
mole -h 2> $null && assert_ret
mole 2> $null && assert_ret

reset_env
echo "groups..."

mole -g group1 file1 | assert_editor $prefix/file1
mole -g group2 file2 | assert_editor $prefix/file2

mole -g group1 | assert_editor $prefix/file1
mole -g group2 | assert_editor $prefix/file2
mole -g kentus 2> $null && assert_ret

reset_env
echo "multiple folders"

mole dir1/file1 > null
mole dir1/file2 > null
mole file1 > null
mole file2 > null
mole dir1 | assert_editor $prefix/dir1/file2

cd dir1
mole | assert_editor $prefix/dir1/file2
mole /tmp/moletest/file1 | assert_editor $prefix/file1

cd /
mole /tmp/moletest | assert_editor $prefix/file2
mole $prefix/dir1/ | assert_editor $prefix/dir1/file2

reset_env
echo "group tests"

mole -g f1 file1 > $null

mole -g f2 file2 > $null
mole -g f2 file2 > $null
mole -g f2 file2 > $null
mole -g f2 file2 > $null

mole -g f1 file3 > $null
mole -g f2 file3 > $null

mole -g f1 | assert_editor $prefix/file3
mole -g f2 -m | assert_editor $prefix/file2
mole -g blentus 2>$null && assert_ret

reset_env
echo "a/b filter tests"

mole file1 > $null
sed -i 's/2023/2022/g' molerc-testing
mole file2 > $null

mole -a 2021-01-01 | assert_editor $prefix/file2
mole -b 2023-01-01 | assert_editor $prefix/file1
mole -a 2024-01-01 2> $null && assert_ret
mole -a 2023-01-01 | assert_editor $prefix/file2

reset_env
echo "list tests"

mole -g f1 file1 > $null
mole -g f2 file2 > $null

touch dir1/longfile
mole -g longgroup dir1/longfile > $null
mole dir1/file1 > $null

list="$(mole list)" || assert_ret
assert_eq "$list" 'file1: f1
file2: f2'

list="$(mole list $prefix/dir1)" || assert_ret
assert_eq "$list" 'file1:    -
longfile: longgroup'

reset_env
echo "log tests"

mole file1 > $null
mole file2 > $null
mole file2 > $null
mole dir1/file1 > $null
mole dir1/file2 > $null

mole secret-log | assert_editor ""
lines="$(cat ~/.mole/* | bunzip2 | wc -l)"
assert_eq "$lines" 4

echo "All tests done."
