;;
;; zeromq bindings
;;
;; usage:
;;    .../src$ make posix USE_LIBFFI=1
;;           $ cd examples
;;           $ make
;;           $ ../klisp klisp-zmq.k
;;
;; files:
;;  klisp-zmq.so ......... interpreter extension compiled to a DLL
;;  klisp-zmq.k .......... example of client code
;;  klisp-zmq.c  ......... C source of the extension
;;  Makefile ............. build script
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
($let* ((ctx (zmq-init 2)) (socket (zmq-socket ctx 3)))
  ($sequence
    (display "zmq errno: ")
    (display (zmq-errno))
    (newline)
    (display "str error for errno 9: ")
    (display (zmq-str-error 9))
    (newline)
    (display "connect socket result: ")
    (display (zmq-connect socket "tcp://127.0.0.1:5555"))
    (newline)
    (display (zmq-send socket (string->bytevector "hello") 1))
    (newline)
    (read-line)
    (display "close socket result: ")
    (display (zmq-close socket))
    (newline)
    (zmq-term ctx)))

