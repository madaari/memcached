// This file has to be included before coyote_mc_redefs.h file.
#ifndef COYOTE_MC_REDEF

#ifndef COYOTE_MC_WRAP
#define COYOTE_MC_WRAP

#include <include_coyote/include/coyote_c_ffi.h>
#include <pthread.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>

// Override pthread_create and join to add coyote specific instrumentation
void *coyote_new_thread_wrapper(void*);

// Useless declarations for useless '-Werror=missing-prototypes'
int FFI_pthread_join(pthread_t tid, void* arg);

/*********************************************** LibEvent APIs **********************************************/

// Declarations for libevent
int FFI_event_set(void* event, int sfd, int flags, void (*event_handler)(int, short, void *), void *arg);
int FFI_event_base_set(void* base, void* event);
int FFI_event_del(void* ev);
int FFI_event_base_loop(void* ev_base, int flags);
int FFI_event_base_loopexit(void* ev_base, void* args);
int FFI_event_add(void *ev, void* tv);

/*********************************************** System calls **********************************************/

// Declarations for '-Werror=missing-prototypes'
int FFI_accept(int, void*, void*);
int FFI_getpeername(int sfd, void* addr, void* addrlen);
ssize_t FFI_write(int sfd, const void* buff, size_t count);
ssize_t FFI_sendmsg(int sfd, struct msghdr *msg, int flags);
int FFI_fcntl(int fd, int cmd, ...);
ssize_t FFI_read(int fd, void* buff, int count);
ssize_t FFI_recvfrom(int, void*, size_t, int, struct sockaddr*, socklen_t*);
int FFI_pipe(int[]);
int FFI_poll(struct pollfd *fds, nfds_t nfds, int timeout);
void FFI_register_clock_handler(void (*clk_handle)(int, short int, void*));

// Its definition is in the mock_libevent library
ssize_t FFI_event_write(int, const void*, size_t, int);
void FFI_clock_handler();

// It is in mocklibevent.cpp
int run_coyote_iteration(int argc, char **argv);

// Defined in coyote_mc_wrapper.c
void FFI_check_stats_data_race(bool isWrite);

#endif /* COYOTE_MC_WRAP */

#endif /* COYOTE_MC_REDEF */