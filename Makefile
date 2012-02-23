#
# klisp-zmq Makefile
#
# Build script for klisp-zmq.so. Please follow instructions
# in klisp-zmq.k.
#

INCLUDES := -I..
CFLAGS := -O2 -g -std=gnu99 -Wall -m32 -shared -lzmq -static -fPIC \
          -DKLISP_USE_POSIX -DKUSE_LIBFFI=1

klisp-zmq.so: klisp-zmq.c
	gcc $(CFLAGS) $(INCLUDES) -o $@ klisp-zmq.c

clean:
	rm -f klisp-zmq.so
