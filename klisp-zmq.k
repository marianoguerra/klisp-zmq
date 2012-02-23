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

($define! ZMQ-AFFINITY 4)
($define! ZMQ-BACKLOG 19)
($define! ZMQ-DEALER 5)
($define! ZMQ-DOWNSTREAM 8)
($define! ZMQ-EADDRINUSE 98)
($define! ZMQ-EADDRNOTAVAIL 99)
($define! ZMQ-EAGAIN 11)
($define! ZMQ-ECONNREFUSED 111)
($define! ZMQ-EFAULT 14)
($define! ZMQ-EFSM 156384763)
($define! ZMQ-EINPROGRESS 115)
($define! ZMQ-EINVAL 22)
($define! ZMQ-EMTHREAD 156384766)
($define! ZMQ-ENETDOWN 100)
($define! ZMQ-ENOBUFS 105)
($define! ZMQ-ENOCOMPATPROTO 156384764)
($define! ZMQ-ENODEV 19)
($define! ZMQ-ENOMEM 12)
($define! ZMQ-ENOTSOCK 88)
($define! ZMQ-ENOTSUP 95)
($define! ZMQ-EPROTONOSUPPORT 93)
($define! ZMQ-ETERM 156384765)
($define! ZMQ-EVENTS 15)
($define! ZMQ-FD 14)
($define! ZMQ-FORWARDER 2)
($define! ZMQ-HWM 1)
($define! ZMQ-IDENTITY 5)
($define! ZMQ-LINGER 17)
($define! ZMQ-MCAST_LOOP 10)
($define! ZMQ-NOBLOCK 1)
($define! ZMQ-PAIR 0)
($define! ZMQ-POLLERR 4)
($define! ZMQ-POLLIN 1)
($define! ZMQ-POLLOUT 2)
($define! ZMQ-PUB 1)
($define! ZMQ-PULL 7)
($define! ZMQ-PUSH 8)
($define! ZMQ-QUEUE 3)
($define! ZMQ-RATE 8)
($define! ZMQ-RCVBUF 12)
($define! ZMQ-RCVMORE 13)
($define! ZMQ-RECONNECT_IVL 18)
($define! ZMQ-RECONNECT_IVL_MAX 21)
($define! ZMQ-RECOVERY_IVL 9)
($define! ZMQ-RECOVERY_IVL_MSEC 20)
($define! ZMQ-REP 4)
($define! ZMQ-REQ 3)
($define! ZMQ-ROUTER 6)
($define! ZMQ-SNDBUF 11)
($define! ZMQ-SNDMORE 2)
($define! ZMQ-STREAMER 1)
($define! ZMQ-SUB 2)
($define! ZMQ-SUBSCRIBE 6)
($define! ZMQ-SWAP 3)
($define! ZMQ-TYPE 16)
($define! ZMQ-UNSUBSCRIBE 7)
($define! ZMQ-UPSTREAM 7)
($define! ZMQ-XPUB 9)
($define! ZMQ-XREP 6)
($define! ZMQ-XREQ 5)
($define! ZMQ-XSUB 10)

(display "zmq version: ")
(display (zmq-version))
(newline)
($let* (
    (ctx (zmq-init 2))
    (socket (zmq-socket ctx ZMQ-REQ))
    (message (string->bytevector "hello")))

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
    (display (zmq-send socket message ZMQ-NOBLOCK))
    (newline)
    (read-line)
    (display "close socket result: ")
    (display (zmq-close socket))
    (newline)
    (zmq-term ctx)))

