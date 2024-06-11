; Autor reseni: Tomas Brablec xbrabl04
; Pocet cyklu k serazeni puvodniho retezce: 955
; Pocet cyklu razeni sestupne serazeneho retezce: 1339
; Pocet cyklu razeni vzestupne serazeneho retezce: 179
; Pocet cyklu razeni retezce s vasim loginem: 257
; Implementovany radici algoritmus: Insertion sort
; ------------------------------------------------

; DATA SEGMENT
                .data

padding:        .space 1 ; because of rearranging instructions,
                         ; the code once reads address str[-1],
                         ; which cannot be -1
; login:          .asciiz "vitejte-v-inp-2023"  ; puvodni uvitaci retezec
; login:          .asciiz "vvttpnjiiee3220---"  ; sestupne serazeny retezec
; login:          .asciiz "---0223eeiijnpttvv"  ; vzestupne serazeny retezec
login:          .asciiz "xbrabl04"              ; SEM DOPLNTE VLASTNI LOGIN
                                                ; A POUZE S TIMTO ODEVZDEJTE

params_sys5:    .space  8   ; misto pro ulozeni adresy pocatku
                            ; retezce pro vypis pomoci syscall 5
                            ; (viz nize - "funkce" print_string)

; CODE SEGMENT
                .text
main:
        ; SEM DOPLNTE VASE RESENI

        ; this code is implemented using assembly
        ; (more or less, there are many optimizations in asm),
        ; it's a simple insertion sort,
        ; should be fast enough for short strings

        ; void insertionSort(char *str) {
        ;     for (char *idx = str; *idx != 0; idx++) {
        ;         for (char *insert = idx; insert != str; insert--) {
        ;             char *prev = insert - 1;
        ;             if (*prev > *insert) {
        ;                 char tmp = *prev;
        ;                 *prev = *insert;
        ;                 *insert = tmp;
        ;             } else {
        ;                 break;
        ;             }
        ;         }
        ;     }
        ; }

        daddi   r1, r0, login ; r1 is char* idx
        daddi   r30, r0, login ; r30 is char* str

outer_for:
        lb      r5, 1(r1) ; r5 is char at insert
        lb      r4, 0(r1) ; r4 is char at prev
        ; nop ; RAW r4 
        daddi   r3, r1, 1 ; r3 is char* insert
        dsub    r6, r5, r4 ; *prev - *insert 
        daddi   r1, r1, 1 ; idx++
        beqz    r5, outer_end

inner_for:
        bgez    r6, outer_for ; "else" branch

        ; "if" branch
        sb      r4, 0(r3) ; prev -> insert
        lb      r4, -2(r3) ; r4 is char at prev
        sb      r5, -1(r3) ; insert -> prev

        daddi   r3, r3, -1 ; insert--
        dsub    r6, r5, r4 ; *prev - *insert 
        b       inner_for ; end of inner_for

outer_end:
        daddi   r4, r0, login   ; vozrovy vypis: adresa login: do r4
        jal     print_string    ; vypis pomoci print_string - viz nize

        syscall 0   ; halt


print_string:   ; adresa retezce se ocekava v r4
                sw      r4, params_sys5(r0)
                daddi   r14, r0, params_sys5    ; adr pro syscall 5 musi do r14
                syscall 5   ; systemova procedura - vypis retezce na terminal
                jr      r31 ; return - r31 je urcen na return address
