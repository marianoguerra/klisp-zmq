;;
;; zeromq bindings
;;
;; usage:
;;    .../src$ make posix USE_LIBFFI=1
;;           $ cd examples
;;           $ make -f klisp-zmq.makefile
;;           $ ../klisp klisp-zmq.k
;;
;; files:
;;  klisp-zmq.so ......... interpreter extension compiled to a DLL
;;  klisp-zmq.k .......... example of client code
;;  klisp-zmq.c  ......... C source of the extension
;;  klisp-zmq.makefile ... build script
;;

(ffi-load-library "libzmq.so")

((ffi-make-applicative
  (ffi-load-library "./klisp-zmq.so")
    "klisp_zmq_init_lib"
    (ffi-make-call-interface
      "FFI_DEFAULT_ABI" "void" (list "pointer")))
 (ffi-klisp-state))

(display "zmq version: ")
(display (zmq-version))
(newline)
($let* ((ctx (zmq-init 2)) (socket (zmq-socket ctx 1)))
  ($sequence
    (display "zmq errno: ")
    (display (zmq-errno))
    (newline)
    (display "str error for errno 9: ")
    (display (zmq-str-error 9))
    (newline)
    (display "connect socket result: ")
    (display (zmq-bind socket "tcp://127.0.0.1:5555"))
    (newline)
    (display "close socket result: ")
    (display (zmq-close socket))
    (newline)
    (zmq-term ctx)))

