(declare-const a Bool)
(declare-const b Bool)

; validity is asserted by getting UNSAT on the negation of the DeMorgan law

; first rule: not(A or B) = not(A) and not(B)
(assert (not
    (= 
        (not (or a b))
        (and (not a) (not b))
    )
))

; expecting UNSAT
(check-sat)

; second rule: not(A and B) = not(A) or not(B)
(assert (not
    (= 
        (not (and a b))
        (or (not a) (not b))
    )
))

; expecting UNSAT
(check-sat)
