#pragma once
// Sent through the stock LispBM REPL into RAM only. Never stored in VESC flash.
// Retains the original target across throttle overrides. Local lease and brake
// checks run on VESC even when ESP is busy or disconnected. Bench validation required.
constexpr const char* cruiseStart=R"LISP((progn
 (def eb-lease (systime)) (def eb-on t) (def eb-target %.3f)
 (spawn 128 (lambda () (progn
  (loopwhile eb-on (progn
   (if (or (> (secs-since eb-lease) 1.5)
           (> (get-adc-decoded 1) 0.02)
           (not (app-adc-range-ok)) (not (= (get-fault) 0)))
    (setq eb-on nil)
    (if (> (get-adc-decoded 0) 0.02)
     (app-disable-output 0)
     (progn (app-disable-output 60) (set-rpm eb-target))))
   (sleep 0.01)))
  (app-disable-output 0) (print "EBIKE_CC_OFF"))))
 "EBIKE_CC_ON"))LISP";
