;;; SBATMS Mock up 1
;;; Mock-up up the communication system among sbatms

;;; Commentary:
;;  A SBATM is a basically a pattern of integer words
;;  SBATMs communicate these words to each other and the real magic
;;  will be what kinds of changes a SBATM makes to its word upon
;;  receiving the words of another

(include "utils.scm")

;;; Code:

;; basic setup 
(define sbatm-leader '(1 2 1)) ; the leader that will come before every message

(define (make-sbatm behavior pattern)
  (alist-cons
   'behavior behavior                        
   (alist-cons
    'pattern (list sbatm-leader pattern) '())))

(define (print-sbatm sbatm)
  (let ((message  (get-message sbatm))
        (behavior (get-behavior sbatm)))
    (print "SBATM with message: " message " and behavior: " behavior)))

(define (get-behavior sbatm)
  (cdr (assoc 'behavior sbatm)))

(define (get-message sbatm)
  (cadr (cdr (assoc 'pattern sbatm))))

(define (connect source targets)
  (alist-cons 'source source
              (alist-cons 'targets targets '())))

(define (get-source a-list)
  (cdr (assoc 'source a-list)))

(define (get-targets a-list)
  (cdr (assoc 'targets a-list)))

(define (apply-sbatm source target)
  (let ((target-f (get-behavior target))
        (source-pattern (get-message source))
        (target-pattern (get-message target)))
    (make-sbatm target-f (target-f source-pattern target-pattern))))

(define (bi-directional-apply source target)
  (values (apply-sbatm source target)
          (apply-sbatm target source)))

(define (run-bi-directonal source targets)
  (define (bi-accum t acc)
    (let ((acc-source (car acc))
          (acc-targets (cdr acc)))
      (let-values (((new-acc-source acc-target) (bi-directional-apply acc-source t)))
        (cons new-acc-source (append acc-targets (list acc-target))))))
  (apply values (fold bi-accum (cons source '()) targets)))

(define (run-connection conn)
  (let ((source (get-source conn))
        (targets (get-targets conn)))
    (let-values (((new-source . new-targets) (run-bi-directonal source targets)))
      (connect new-source new-targets))))

;; behaviors
(define (nudge source target)
  (+ target (sign (- source target))))

(define (single-converge source-pat target-pat)
  (let ((idx (random (safe-length source-pat target-pat))))
    (apply-at target-pat idx
              (lambda (x) (nudge (nth source-pat idx) x)))))

(define (full-converge source-pat target-pat)
  (map nudge source-pat target-pat))

;; program

(define (sbatms-iterate connections n)
  (define (iterate-iter conns i)
    (if (= i 0)
        connections
        (let ((new-cons (map run-connection conns)))
          (print "Iteration " (- n i) ":")
          (pp (car new-cons))
          (iterate-iter new-cons (- i 1)))))
  (iterate-iter connections n))

(define (test n)
  (let* ((sbatms (map (lambda (_)
                        (make-sbatm single-converge
                                    (random-pattern 5 1 10)))
                      (range 0 10 1)))
         (connection (connect (car sbatms) (cdr sbatms))))
    (sbatms-iterate (list connection) n)
    '()))
