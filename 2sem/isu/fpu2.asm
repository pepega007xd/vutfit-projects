%include 'rw32-2022.inc'

section .data
    kentus dq 5.7


section .text

main:
    push ebp
    mov ebp, esp

    fld1
    fldpi 
    fchs ; -pi 1
    fcom st1
    mov al, '>'
    ja greater
    mov al, '<'

greater:
    call WriteChar
    
    mov eax, 0
    pop ebp
    ret
