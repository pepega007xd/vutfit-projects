%include "rw32-2022.inc"

section .data
    array dd 5.0, 4.0, 3.0
    N dd 3
    ; f -- je fce
    error dd 0

section .text

f:
    push ebp
    mov ebp, esp

    fld dword [ebp + 8]

    pop ebp
    ret

task31:
    ; insert your code here
    push ebp 
    mov ebp, esp

    ; array == ebp + 8
    ; N == ebp + 12
    ; f == ebp + 16
    ; error == ebp + 20
    mov edx, [ebp + 8]

    ; RMS
    mov ecx, [ebp + 12] ; N
    dec ecx
    ; fld dword [edx] ; a[0]
    push dword [edx]
    call [ebp + 16] ; f(a[0])
    add esp, 4
    fmul st0 ; f(a[0])^2
.loop:
    push dword [edx + 4 * ecx]
    ; fld dword [edx + 4 * ecx]
    call [ebp + 16] ; f(a[i])   f(a[0])^2
    add esp, 4
    fmul st0
    faddp
    loop .loop

    fidiv dword [ebp + 12] ; N
    fsqrt

    mov edx, [ebp + 20]
    mov dword [edx], 0

    pop ebp
    ret

CMAIN:
    push ebp
    mov ebp, esp

    push error
    push f
    push dword [N]
    push array

    call task31
    add esp, 16
    call WriteDouble

    mov eax, 0

    pop ebp
    ret
