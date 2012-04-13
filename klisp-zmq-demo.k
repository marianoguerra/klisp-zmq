(require "klisp-zmq")

($import-library! (#:prefix (zmq) zmq.))

($define! show ($lambda (msg result)
  (display msg)
  (display result)
  (newline)))

(display "zmq version: ")
(display (zmq.version))
(newline)
($let* (
        (ctx (zmq.init 2))
        (socket0 (zmq.socket ctx zmq.REQ))
        (socket1 (zmq.socket ctx zmq.REQ))
        (socket2 (zmq.socket ctx zmq.REQ))
        (message (string->bytevector "hello")))

       ($sequence
         (show "zmq errno: " (zmq.errno))
         (show "str error for errno 9: " (zmq.str-error 9))
         (show "connect socket result: " (zmq.connect socket0 "tcp://127.0.0.1:5555"))
         (show "sending message: " (bytevector->string message))
         (show "send result: " (zmq.send socket0 message zmq.NOBLOCK))
         (show "waiting for reply" (bytevector->string (zmq.recv socket0 0)))
         (show "close socket0 result: " (zmq.close socket0))
         (show "close socket1 result: " (zmq.close socket1))
         (show "close socket2 result: " (zmq.close socket2))
         (zmq.term ctx)))

