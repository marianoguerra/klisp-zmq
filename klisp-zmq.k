;;
;; zeromq bindings
;;
;; usage:
;;    .../src$ make posix USE_LIBFFI=1
;;           $ cd examples
;;           $ make
;;           $ ../klisp klisp-zmq-demo.k
;;
;; files:
;;  klisp-zmq.so ......... interpreter extension compiled to a DLL
;;  klisp-zmq.k .......... library code
;;  klisp-zmq.c  ......... C source of the extension
;;  klisp-zmq-demo.k ..... example of client code
;;  Makefile ............. build script
;;

($provide-library! (zmq) (#:export 
    version init term errno str-error socket close
    connect bind send recv device poll

    AFFINITY BACKLOG DEALER DOWNSTREAM EADDRINUSE EADDRNOTAVAIL EAGAIN
    ECONNREFUSED EFAULT EFSM EINPROGRESS EINVAL EMTHREAD ENETDOWN ENOBUFS
    ENOCOMPATPROTO ENODEV ENOMEM ENOTSOCK ENOTSUP EPROTONOSUPPORT ETERM EVENTS
    FD FORWARDER HWM IDENTITY LINGER MCAST_LOOP NOBLOCK PAIR POLLERR POLLIN
    POLLOUT PUB PULL PUSH QUEUE RATE RCVBUF RCVMORE RECONNECT_IVL
    RECONNECT_IVL_MAX RECOVERY_IVL RECOVERY_IVL_MSEC REP REQ ROUTER SNDBUF
    SNDMORE STREAMER SUB SUBSCRIBE SWAP TYPE UNSUBSCRIBE UPSTREAM XPUB XREP
    XREQ XSUB)

  (ffi-load-library "libzmq.so")

  ((ffi-make-applicative
     (ffi-load-library "./klisp-zmq.so")
     "klisp_zmq_init_lib"
     (ffi-make-call-interface
       "FFI_DEFAULT_ABI" "void" (list "pointer")))
   (ffi-klisp-state))

  ($define! AFFINITY 4)
  ($define! BACKLOG 19)
  ($define! DEALER 5)
  ($define! DOWNSTREAM 8)
  ($define! EADDRINUSE 98)
  ($define! EADDRNOTAVAIL 99)
  ($define! EAGAIN 11)
  ($define! ECONNREFUSED 111)
  ($define! EFAULT 14)
  ($define! EFSM 156384763)
  ($define! EINPROGRESS 115)
  ($define! EINVAL 22)
  ($define! EMTHREAD 156384766)
  ($define! ENETDOWN 100)
  ($define! ENOBUFS 105)
  ($define! ENOCOMPATPROTO 156384764)
  ($define! ENODEV 19)
  ($define! ENOMEM 12)
  ($define! ENOTSOCK 88)
  ($define! ENOTSUP 95)
  ($define! EPROTONOSUPPORT 93)
  ($define! ETERM 156384765)
  ($define! EVENTS 15)
  ($define! FD 14)
  ($define! FORWARDER 2)
  ($define! HWM 1)
  ($define! IDENTITY 5)
  ($define! LINGER 17)
  ($define! MCAST_LOOP 10)
  ($define! NOBLOCK 1)
  ($define! PAIR 0)
  ($define! POLLERR 4)
  ($define! POLLIN 1)
  ($define! POLLOUT 2)
  ($define! PUB 1)
  ($define! PULL 7)
  ($define! PUSH 8)
  ($define! QUEUE 3)
  ($define! RATE 8)
  ($define! RCVBUF 12)
  ($define! RCVMORE 13)
  ($define! RECONNECT_IVL 18)
  ($define! RECONNECT_IVL_MAX 21)
  ($define! RECOVERY_IVL 9)
  ($define! RECOVERY_IVL_MSEC 20)
  ($define! REP 4)
  ($define! REQ 3)
  ($define! ROUTER 6)
  ($define! SNDBUF 11)
  ($define! SNDMORE 2)
  ($define! STREAMER 1)
  ($define! SUB 2)
  ($define! SUBSCRIBE 6)
  ($define! SWAP 3)
  ($define! TYPE 16)
  ($define! UNSUBSCRIBE 7)
  ($define! UPSTREAM 7)
  ($define! XPUB 9)
  ($define! XREP 6)
  ($define! XREQ 5)
  ($define! XSUB 10))
