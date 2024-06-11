%include "rw32-2022.inc"

section .data
    string db "Hello, world %d %c%c",0
    num dd 42
    numbers dd 69, 420

section .text

EXTERN printf

fn:
    push ebp
    mov ebp, esp
    sub esp, 8

    mov ebx, [ebp + 8]
    mov ecx, [ebp + 12]
    mov edx, [ebp + 16]
    mov esi, [ebp + 20]

    cmp ebx, 0
    mov eax, 0
    je .end
    mov eax, [ebx]

.end:
    add esp, 8
    pop ebp
    ret 4*4


CMAIN:
    push ebp
    mov ebp, esp

    push dword `\n`
    push dword `~`
    push dword [num]
    push dword string
    call printf ; printf("Hello, world %d%c", num, '\n')
    add esp, 4*4

    mov eax, 78
    push 0
    push 0
    push 0
    push numbers
    call fn

    call WriteInt32

    mov eax, 0

    pop ebp
    ret

