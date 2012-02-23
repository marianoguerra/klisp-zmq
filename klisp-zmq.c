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
	bind_1tp(K, K->next_value, "exact integer", ttiseinteger, v_io_threads);

	if (ttisfixint(v_io_threads)) {
		io_threads = ivalue(v_io_threads);

		void *ctx = zmq_init(io_threads);

		kapply_cc(K, p2tv(ctx));
	} else {
		klispE_throw_simple_with_irritants(K, "expected fixint for iothreads parameter", 1, v_io_threads);
	}
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
	bind_1tp(K, K->next_value, "exact integer", ttiseinteger, v_errnum);

	if (ttisfixint(v_errnum)) {
		int errnum = ivalue(v_errnum);
		// XXX: should I free msg? (zmq question)
		const char *msg = zmq_strerror(errnum);
		TValue v_msg = kstring_new_b_imm(K, msg);

		kapply_cc(K, v_msg);
	} else {
		klispE_throw_simple_with_irritants(K, "expected fixint for errnum parameter", 1, v_errnum);
	}
}

static void klisp_zmq_socket(klisp_State *K)
{
	int type;
	void *ctx, *socket;

	bind_2tp(K, K->next_value,
			"user pointer", ttisuser, v_ctx,
			"exact integer", ttiseinteger, v_type);

	if (ttisfixint(v_type)) {
		type = ivalue(v_type);
		ctx  = pvalue(v_ctx);

		socket = zmq_socket(ctx, type);

		kapply_cc(K, p2tv(socket));
	} else {
		klispE_throw_simple_with_irritants(K, "expected fixint for type parameter", 1, v_type);
	}
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
			"exact integer", ttisfixint, v_flags);

	if (ttisfixint(v_flags)) {
		socket = pvalue(v_socket);
		data = kbytevector_buf(v_bytevector);
		size = kbytevector_size(v_bytevector);
		flags = ivalue(v_flags);

		int init_data_result = zmq_msg_init_data(&msg, (void *)data, size, NULL, NULL);
		int result = zmq_send(socket, &msg, flags);
		int close_result = zmq_msg_close(&msg);

		kapply_cc(K, i2tv(result));
	} else {
		klispE_throw_simple_with_irritants(K, "expected fixint for flags parameter", 1, v_flags);
	}
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
			"exact integer", ttisfixint, v_flags);

	if (ttisfixint(v_flags)) {
		socket = pvalue(v_socket);
		flags = ivalue(v_flags);

		int init_data_result = zmq_msg_init(&msg);
		int result = zmq_recv(socket, &msg, flags);

		data = zmq_msg_data(&msg);
		size = zmq_msg_size(&msg);

		v_data = kbytevector_new_bs_imm(K, data, size);

		int close_result = zmq_msg_close(&msg);

		kapply_cc(K, v_data);
	} else {
		klispE_throw_simple_with_irritants(K, "expected fixint for flags parameter", 1, v_flags);
	}
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
	safe_add_applicative(K, K->next_env, "zmq-version", klisp_zmq_version);
	safe_add_applicative(K, K->next_env, "zmq-init", klisp_zmq_init);
	safe_add_applicative(K, K->next_env, "zmq-term", klisp_zmq_term);
	safe_add_applicative(K, K->next_env, "zmq-errno", klisp_zmq_errno);
	safe_add_applicative(K, K->next_env, "zmq-str-error", klisp_zmq_strerror);
	safe_add_applicative(K, K->next_env, "zmq-socket", klisp_zmq_socket);
	safe_add_applicative(K, K->next_env, "zmq-close", klisp_zmq_close);
	safe_add_applicative(K, K->next_env, "zmq-connect", klisp_zmq_connect);
	safe_add_applicative(K, K->next_env, "zmq-bind", klisp_zmq_bind);
	safe_add_applicative(K, K->next_env, "zmq-send", klisp_zmq_send);
	safe_add_applicative(K, K->next_env, "zmq-recv", klisp_zmq_recv);
	klisp_assert(K->rooted_tvs_top == 0);
	klisp_assert(K->rooted_vars_top == 0);
}
