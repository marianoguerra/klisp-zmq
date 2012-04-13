/*
 ** klisp-zmq.c
 **
 ** zeromq bindings for klisp
 **
 */

#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <zmq.h>

#include "kstate.h"
#include "kstring.h"
#include "kinteger.h"
#include "kbytevector.h"
#include "kpair.h"
#include "kport.h"
#include "kwrite.h"
#include "kghelpers.h"

#include "imath.h"

#if !defined(KLISP_USE_POSIX) || !defined(KUSE_LIBFFI)
#  error "Bad klisp configuration."
#endif

static void klisp_zmq_version(klisp_State *K)
{
	int major, minor, patch;
	TValue v_version;

	zmq_version(&major, &minor, &patch);

	v_version = klist(K, 3, i2tv(major), i2tv(minor), i2tv(patch));

	kapply_cc(K, v_version);
}

void display(klisp_State *K, TValue value)
{
	TValue port = kcdr(K->kd_out_port_key);

	kwrite_display_to_port(K, port, value, false);
	kwrite_newline_to_port(K, port);
}

static void klisp_zmq_init(klisp_State *K)
{
	int io_threads;
	bind_1tp(K, K->next_value, "fixint", ttisfixint, v_io_threads);

	io_threads = ivalue(v_io_threads);

	void *ctx = zmq_init(io_threads);

	kapply_cc(K, p2tv(ctx));
}

static void klisp_zmq_term(klisp_State *K)
{
	void *ctx;
	bind_1tp(K, K->next_value, "user pointer", ttisuser, v_ctx);

	ctx = pvalue(v_ctx);

	zmq_term(ctx);

	kapply_cc(K, KINERT);
}

static void klisp_zmq_errno(klisp_State *K)
{
	int err = zmq_errno();
	kapply_cc(K, i2tv(err));
}

static void klisp_zmq_strerror(klisp_State *K)
{
	bind_1tp(K, K->next_value, "fixint", ttisfixint, v_errnum);

	int errnum = ivalue(v_errnum);
	// XXX: should I free msg? (zmq question)
	const char *msg = zmq_strerror(errnum);
	TValue v_msg = kstring_new_b_imm(K, msg);

	kapply_cc(K, v_msg);
}

static void klisp_zmq_socket(klisp_State *K)
{
	int type;
	void *ctx, *socket;

	bind_2tp(K, K->next_value,
			"user pointer", ttisuser, v_ctx,
			"fixint", ttisfixint, v_type);

	type = ivalue(v_type);
	ctx  = pvalue(v_ctx);

	socket = zmq_socket(ctx, type);

	kapply_cc(K, p2tv(socket));
}

static void klisp_zmq_close(klisp_State *K)
{
	int result;
	void *socket;

	bind_1tp(K, K->next_value, "user pointer", ttisuser, v_socket);

	socket = pvalue(v_socket);

	result = zmq_close(socket);

	kapply_cc(K, i2tv(result));
}

static void klisp_zmq_connect(klisp_State *K)
{
	void *socket;

	bind_2tp(K, K->next_value,
			"user pointer", ttisuser, v_socket,
			"string", ttisstring, v_endpoint);

	socket = pvalue(v_socket);
	const char *endpoint = kstring_buf(v_endpoint);

	int result = zmq_connect(socket, endpoint);

	kapply_cc(K, i2tv(result));
}

static void klisp_zmq_bind(klisp_State *K)
{
	void *socket;

	bind_2tp(K, K->next_value,
			"user pointer", ttisuser, v_socket,
			"string", ttisstring, v_endpoint);

	socket = pvalue(v_socket);
	const char *endpoint = kstring_buf(v_endpoint);

	int result = zmq_bind(socket, endpoint);

	kapply_cc(K, i2tv(result));
}

static void klisp_zmq_send(klisp_State *K)
{
	void *socket;
	uint8_t *data;
	uint32_t size;
	int flags;
	zmq_msg_t msg;

	bind_3tp(K, K->next_value,
			"user pointer", ttisuser, v_socket,
			"byte vector", ttisbytevector, v_bytevector,
			"fixint", ttisfixint, v_flags);

	socket = pvalue(v_socket);
	data = kbytevector_buf(v_bytevector);
	size = kbytevector_size(v_bytevector);
	flags = ivalue(v_flags);

	int init_data_result = zmq_msg_init_data(&msg, (void *)data, size, NULL, NULL);
	int result = zmq_send(socket, &msg, flags);
	int close_result = zmq_msg_close(&msg);

	kapply_cc(K, i2tv(result));
}

static void klisp_zmq_recv(klisp_State *K)
{
	void *socket;
	uint8_t *data;
	uint32_t size;
	int flags;
	zmq_msg_t msg;
	TValue v_data;

	bind_2tp(K, K->next_value,
			"user pointer", ttisuser, v_socket,
			"fixint", ttisfixint, v_flags);

	socket = pvalue(v_socket);
	flags = ivalue(v_flags);

	int init_data_result = zmq_msg_init(&msg);
	int result = zmq_recv(socket, &msg, flags);

	data = zmq_msg_data(&msg);
	size = zmq_msg_size(&msg);

	v_data = kbytevector_new_bs_imm(K, data, size);

	int close_result = zmq_msg_close(&msg);

	kapply_cc(K, v_data);
}

static void klisp_zmq_device(klisp_State *K)
{
	int device;
	void *frontend, *backend;

	bind_3tp(K, K->next_value,
			"fixint", ttisfixint, v_device,
			"user pointer", ttisuser, v_frontend,
			"user pointer", ttisuser, v_backend);

	frontend = pvalue(v_frontend);
	backend = pvalue(v_backend);
	device = ivalue(v_device);

	int result = zmq_device(device, frontend, backend);

	kapply_cc(K, i2tv(result));
}

int get_list_size(TValue list) {
	int size = 0;
	TValue current = list;

	while (ttispair(current)) {
		size += 1;
		current = kcdr(current);
	}

	return size;
}

void get_user_pointer_list(TValue list, void **pointers, int *nitems, bool *error) {
	int count = 0, size = get_list_size(list);
	pointers = (void**) malloc(sizeof(void*) * size);
	TValue current = list, pointer;

	*error = false;
	*nitems = size;

	while (ttispair(current)) {
		pointer = kcar(current);
		current = kcdr(current);

		if (!ttisuser(pointer)) {
			*error = true;
			break;
		}

		pointers[count] = pvalue(pointer);
		count += 1;
	}

}

static void klisp_zmq_poll(klisp_State *K)
{
	long timeout = 1000;
	int i, nitems = 1;
	void **sockets = NULL;
	bool error = false;
	zmq_pollitem_t **items;

	bind_2tp(K, K->next_value,
			"exact integer", ttisexact, v_timeout,
			"pair", ttispair, v_sockets);

	get_user_pointer_list(v_sockets, sockets, &nitems, &error);

	if (error) {
		klispE_throw_simple_with_irritants(K, "expected list of sockets", 1, v_sockets);
	} else {
		items = (zmq_pollitem_t**) malloc(sizeof(zmq_pollitem_t*) * nitems);

		for (i = 0; i < nitems; i++) {
			items[i] = (zmq_pollitem_t*) malloc(sizeof(zmq_pollitem_t));
			items[i]->socket = sockets[i];
			// TODO allow to set it by the user
			items[i]->events = ZMQ_POLLIN | ZMQ_POLLOUT;
		}

		zmq_poll(*items, nitems, timeout);
		free(items);

		// TODO return list of pairs (socket result)
		kapply_cc(K, i2tv(0));
	}

	free(sockets);
}

static void safe_add_applicative(klisp_State *K, TValue env,
		const char *name,
		klisp_CFunction fn)
{
	TValue symbol = ksymbol_new_b(K, name, KNIL);
	krooted_tvs_push(K, symbol);
	TValue value = kmake_applicative(K, fn, 0);
	krooted_tvs_push(K, value);
	kadd_binding(K, env, symbol, value);
	krooted_tvs_pop(K);
	krooted_tvs_pop(K);
}

void klisp_zmq_init_lib(klisp_State *K)
{
	safe_add_applicative(K, K->next_env, "version", klisp_zmq_version);
	safe_add_applicative(K, K->next_env, "init", klisp_zmq_init);
	safe_add_applicative(K, K->next_env, "term", klisp_zmq_term);
	safe_add_applicative(K, K->next_env, "errno", klisp_zmq_errno);
	safe_add_applicative(K, K->next_env, "str-error", klisp_zmq_strerror);
	safe_add_applicative(K, K->next_env, "socket", klisp_zmq_socket);
	safe_add_applicative(K, K->next_env, "close", klisp_zmq_close);
	safe_add_applicative(K, K->next_env, "connect", klisp_zmq_connect);
	safe_add_applicative(K, K->next_env, "bind", klisp_zmq_bind);
	safe_add_applicative(K, K->next_env, "send", klisp_zmq_send);
	safe_add_applicative(K, K->next_env, "recv", klisp_zmq_recv);
	safe_add_applicative(K, K->next_env, "device", klisp_zmq_device);
	safe_add_applicative(K, K->next_env, "poll", klisp_zmq_poll);
	klisp_assert(K->rooted_tvs_top == 0);
	klisp_assert(K->rooted_vars_top == 0);
}
