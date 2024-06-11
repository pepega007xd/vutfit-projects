%include "rw32-2022.inc"

section .text

task32:
    ; insert your code here
    push ebp
    mov ebp, esp
    ; x == ebp + 12
    ; y == ebp + 8
    

    fld dword [ebp + 12] ; y
    push __float32__(9.850)
    fadd  dword [esp] ; y + 9.850
    add esp, 4 ; float32 9.850
    fsqrt ; sqrt(y + 9.850)
    
    fldpi
    fmul dword [ebp + 8] ; pi*x   sqrt(..)
    fadd dword [ebp + 12]
    fsin
    fmulp ; citatel

    fld dword [ebp + 8] ; x   citatel
    push __float32__(8.0)
    fdiv dword [esp] ; x/8   citatel
    add esp, 4 ; float32 8.0
    fsub dword [ebp + 12]
    fabs ; jmenovatel   citatel
    fdivp

    pop ebp
    ret

CMAIN:
    push ebp
    mov ebp, esp

    push __float32__(23.850)
    push __float32__(28.850)
    call task32
    add esp, 8
    call WriteDouble

    mov eax, 0

    pop ebp
    ret
