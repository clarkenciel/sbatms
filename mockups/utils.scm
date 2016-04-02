;;; utils.scm
;;; Commentary:
;;  generally useful utils for other things

;; utils
(define (random-pattern pat-len lo hi)
  (define (rpat-iter pat len)
    (if (= 0 len)
        pat
        (rpat-iter (cons (+ lo (random (- hi lo)))
                         pat)
                   (- len 1))))
  (rpat-iter '() pat-len))

(define (sign val)
  (cond ((> 0 val) -1)
        ((< 0 val) 1)
        (else 0)))

(define (range start fin step)
  (define (range-iter coll cur)
    (if (= cur fin)
        coll
        (range-iter (append coll (list cur))
                    (+ step cur))))
  (if (not (= (sign (- fin start)) ; catch dumb infinite recursion
              (sign step)))
      '()
      (range-iter '() start)))

(define (nth coll idx)
  (cond ((null? coll) coll)
        ((= 0 idx) (car coll))
        (else (nth (cdr coll) (- idx 1)))))

(define (apply-at coll idx f)
  (let-values (((head tail) (split-at coll idx)))
    (append head (cons (f (car tail)) (cdr tail)))))

(define (safe-length . colls)
  (apply min (map length colls)))
