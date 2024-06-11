#! /usr/bin/bash

# Makefile
# Řešení IJC-DU1, příklad a), 2022-03-04 
# Autor: Tomáš Brablec
# Přeloženo: gcc 10.2.1
# testovací skript pro projekt

echo "running macro bitset tests..."
./test
echo "running inline bitset tests..."
./test-i

echo "running steg encode-decode tests"
folder=testimg
rm $folder/enc-* 2> /dev/null

for img in `ls $folder`
do
    message=`openssl rand -base64 30`
    ./steg-encode $folder/$img $folder/enc-$img $message || \
        echo "steg-encode ended with an error"

    decoded=`./steg-decode $folder/enc-$img`

    [[ $message == $decoded ]] || echo "Assertion failed: $message != $decoded"
done

primes="229999831
229999849
229999871
229999897
229999933
229999937
229999949
229999961
229999969
229999981"

output=`./primes`
[[ $primes == $output ]] || echo "Assertion failed: $primes != $output"
output=`./primes-i`
[[ $primes == $output ]] || echo "Assertion failed: $primes != $output"

echo "all tests finished" 
