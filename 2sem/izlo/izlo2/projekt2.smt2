(set-logic NIA)

(set-option :produce-models true)
(set-option :incremental true)

; Deklarace promennych pro vstupy
; ===============================

; Ceny
(declare-fun c1 () Int)
(declare-fun c2 () Int)
(declare-fun c3 () Int)
(declare-fun c4 () Int)
(declare-fun c5 () Int)

; Kaloricke hodnoty
(declare-fun k1 () Int)
(declare-fun k2 () Int)
(declare-fun k3 () Int)
(declare-fun k4 () Int)
(declare-fun k5 () Int)

; Maximalni pocty porci
(declare-fun m1 () Int)
(declare-fun m2 () Int)
(declare-fun m3 () Int)
(declare-fun m4 () Int)
(declare-fun m5 () Int)

; Maximalni cena obedu
(declare-fun max_cena () Int)

; Deklarace promennych pro reseni
(declare-fun n1 () Int)
(declare-fun n2 () Int)
(declare-fun n3 () Int)
(declare-fun n4 () Int)
(declare-fun n5 () Int)
(declare-fun best () Int)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;; START OF SOLUTION ;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Zde doplnte vase reseni

; cena je mensi nebo rovna max_cena
(assert (<= 
    (+ (* c1 n1) (* c2 n2) (* c3 n3) (* c4 n4) (* c5 n5))
    max_cena)
)

; pocty koupenych obedu jsou mensi nebo rovny max. mnozstvi, a zaroven nezaporny
(assert (and
    (<= 0 n1 m1)
    (<= 0 n2 m2)
    (<= 0 n3 m3)
    (<= 0 n4 m4)
    (<= 0 n5 m5)
)) 

; kazda petice poctu jidel (na, nb, nc, nd, ne), ktera splnuje ty same pozadavky,
; musi mit mensi nebo stejny soucet kalorii jako (n1, n2, n3, n4, n5),
; tim padem je (n1, n2, n3, n4, n5) optimalni reseni
(assert (forall 
    ((na Int) (nb Int) (nc Int) (nd Int) (ne Int))
    (=> (and 
            ; mensi nebo rovny max. cene
            (<= 
                (+ (* c1 na) (* c2 nb) (* c3 nc) (* c4 nd) (* c5 ne))
                max_cena
            )
            ; limity na pocet kusu
            (and
                (<= 0 na m1)
                (<= 0 nb m2)
                (<= 0 nc m3)
                (<= 0 nd m4)
                (<= 0 ne m5)
            ) 
        )
        (<=
            ; pocet kalorii musi byt stejny nebo mensi nez optimalni reseni
            (+ (* na k1) (* nb k2) (* nc k3) (* nd k4) (* ne k5))
            (+ (* n1 k1) (* n2 k2) (* n3 k3) (* n4 k4) (* n5 k5))
        )
    )
))

; celkovy pocet kalorii
(assert (= 
    best
    (+ (* n1 k1) (* n2 k2) (* n3 k3) (* n4 k4) (* n5 k5))
))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;; END OF SOLUTION ;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Testovaci vstupy
; ================

(echo "Test 1.a - ocekavany vystup je sat, promenna best ma hodnotu 34")
(check-sat-assuming (
  (= c1 7) (= c2 3) (= c3 6) (= c4 10) (= c5 8)
  (= k1 5) (= k2 2) (= k3 4) (= k4 7)  (= k5 3)
  (= m1 3) (= m2 2) (= m3 4) (= m4 1)  (= m5 3)
  (= max_cena 50)
))
(get-value (best n1 n2 n3 n4 n5))

(echo "Test 1.b - neexistuje jine reseni nez 34, ocekavany vystup je unsat")
(check-sat-assuming (
  (= c1 7) (= c2 3) (= c3 6) (= c4 10) (= c5 8)
  (= k1 5) (= k2 2) (= k3 4) (= k4 7)  (= k5 3)
  (= m1 3) (= m2 2) (= m3 4) (= m4 1)  (= m5 3)
  (= max_cena 50)
  (not (= best 34))
))

; =========================================================


(echo "Test 2.a - ocekavany vystup je sat, promenna best ma hodnotu 0")
(check-sat-assuming (
  (= c1 7) (= c2 3) (= c3 6) (= c4 10) (= c5 8)
  (= k1 5) (= k2 2) (= k3 4) (= k4 7)  (= k5 3)
  (= m1 3) (= m2 2) (= m3 4) (= m4 1)  (= m5 3)
  (= max_cena 0)
))
(get-value (best n1 n2 n3 n4 n5))

(echo "Test 2.b - neexistuje jine reseni nez 0, ocekavany vystup je unsat")
(check-sat-assuming (
  (= c1 7) (= c2 3) (= c3 6) (= c4 10) (= c5 8)
  (= k1 5) (= k2 2) (= k3 4) (= k4 7)  (= k5 3)
  (= m1 3) (= m2 2) (= m3 4) (= m4 1)  (= m5 3)
  (= max_cena 0)
  (not (= best 0))
))
