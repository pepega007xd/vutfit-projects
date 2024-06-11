(declare-fun pos (Int Int) Bool)
(define-fun is-in-range ((x Int)) Bool (and (> x 0) (<= x 8)))

(declare-const x Bool)
(declare-const y Bool)

(assert 
  (<=
    (not (and x y))
    (or (not x) (not y))
  )
)
 
(assert 
  (=>
    (not (and x y))
    (or (not x) (not y))
  )
)
 
(check-sat)
(get-model)
