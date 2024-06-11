(declare-const d Real)
(declare-const t Real)
(declare-const a Real)
(declare-const v_f Real)
(declare-const v_i Real)

; initial speed
(assert (= v_i 25.0))

; final speed
(assert (= v_f 0.0))

; deceleration
(assert (= a -9.0))

; d = v_i * t + a * t^2 / 2
(assert (=
    d
    (+
        (* v_i t)
        (/ 
            (* a t t)
            2.0
        )
    )
))

; v_f = v_i + a * t
(assert (=
    v_f
    (+ v_i (* a t))
))

(check-sat)
; prints result distance
(get-value (d))
