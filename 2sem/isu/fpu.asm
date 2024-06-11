%include "rw32-2022.inc"

section .data

section .text

newl:
    push eax
    mov al, 10
    call WriteChar
    pop eax
    ret

quadroot:
    push ebp
    mov ebp, esp
    
    mov al, 10

    fld dword [ebp + 8]
    ; a*c
    fmul dword [ebp + 16]
    ; 4*a*c
    TIMES 4 fld1
    TIMES 3 faddp
    fmulp
    ; b^2
    fld dword [ebp + 12]
    fmul dword [ebp + 12]
    ; b^2 - 4*a*c
    fsubrp

    ; load zero
    fld1
    fld1
    fsubp
    fcomip st1
    jb .Dabovezero
    je .Dzero
    ja .Dbelowzero

.Dabovezero:
    fsqrt

    ; -b
    fld dword [ebp + 12]
    fadd st1
    ; citatel
    fld dword [ebp + 8]
    fld1
    fld1
    faddp
    ; 2*a
    fmulp
    fdivp
    fchs
    call WriteDouble
    call newl

    fstp st0

    ; -b
    fld dword [ebp + 12]
    fsub st1
    ; citatel
    fld dword [ebp + 8]
    fld1
    fld1
    faddp
    ; 2*a
    fmulp
    fdivp
    fchs
    call WriteDouble
    call newl
    jmp .end
    
.Dzero:
    fstp st0
    ; -b
    fld dword [ebp + 12]
    ; citatel
    fld dword [ebp + 8]
    fld1
    fld1
    faddp
    ; 2*a
    fmulp
    fdivp
    call WriteDouble
    call newl
    jmp .end

.Dbelowzero:
    ; -b
    fld dword [ebp + 12]
    ; citatel
    fld dword [ebp + 8]
    fld1
    fld1
    faddp
    ; 2*a
    fmulp
    fdivp
    fchs
    call WriteDouble
    call newl


    fstp st0
    
    fchs
    fsqrt

    fld dword [ebp + 8]
    fld1
    fld1
    faddp
    ; 2*a
    fmulp
    fdivp
    call WriteDouble
    call newl
    fchs
    call WriteDouble
    call newl

.end:
    pop ebp
    ret

CMAIN:
    push ebp
    mov ebp, esp

    push dword __float32__(13.0) ; c
    push dword __float32__(-4.0) ; b
    push dword __float32__(1.0) ; a
    call quadroot
    add esp, 4*3

    call newl

    push dword __float32__(4.0) ; c
    push dword __float32__(4.0) ; b
    push dword __float32__(1.0) ; a
    call quadroot
    add esp, 4*3

    call newl

    push dword __float32__(20.0) ; c
    push dword __float32__(-9.0) ; b
    push dword __float32__(1.0) ; a
    call quadroot
    add esp, 4*3

    mov eax, 0
    pop ebp
    ret

