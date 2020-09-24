#ifndef COYOTE_MC_WRAP
#define COYOTE_MC_WRAP

#include <include_coyote/include/coyote_c_ffi.h>
#include <pthread.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

// pthread APIs
#define pthread_mutex_init(x, y) FFI_pthread_mutex_init(x, y)
#define pthread_mutex_lock(x) FFI_pthread_mutex_lock(x)
#define pthread_mutex_trylock(x) FFI_pthread_mutex_trylock(x)
#define pthread_mutex_unlock(x) FFI_pthread_mutex_unlock(x)
#define pthread_mutex_destroy(x) FFI_pthread_mutex_destroy(x)

#define pthread_cond_init(x, y) FFI_pthread_cond_init(x, y)
#define pthread_cond_wait(x, y) FFI_pthread_cond_wait(x, y)
#define pthread_cond_signal(x) FFI_pthread_cond_signal(x)
#define pthread_cond_broadcast(x) FFI_pthread_cond_signal(x)
#define pthread_cond_destory(x) FFI_pthread_cond_signal(x)

// MC specific defines
#undef mutex_lock
#define mutex_lock(x) FFI_pthread_mutex_lock(x)
#undef mutex_unlock
#define mutex_unlock(x) FFI_pthread_mutex_unlock(x)
#undef THR_STATS_LOCK
#define THR_STATS_LOCK(x) FFI_pthread_mutex_lock(x)
#undef THR_STATS_UNLOCK
#define THR_STATS_UNLOCK(x) FFI_pthread_mutex_unlock(x)

// Override pthread_create and join to add coyote specific instrumentation

// Temporary data structure used for passing parameteres to pthread_create
typedef struct pthread_create_params{
	void *(*start_routine) (void *);
	void* arg;
} pthread_c_params;

void *coyote_new_thread_wrapper(void*);

// This function will be called in pthread_create
void *coyote_new_thread_wrapper(void *p){

	FFI_create_operation((long unsigned)pthread_self());
	FFI_start_operation((long unsigned)pthread_self());

	pthread_c_params* param = (pthread_c_params*)p;

	((param->start_routine))(param->arg);

	FFI_complete_operation((long unsigned)pthread_self());

	return NULL;
}

// Useless declarations for useless '-Werror=missing-prototypes'
int FFI_pthread_join(pthread_t tid, void* arg);

// Call our wrapper function instead of original parameters to pthread_create
int FFI_pthread_create(void *tid, void *attr, void *(*start_routine) (void *), void* arguments){

	pthread_c_params *p = (pthread_c_params *)malloc(sizeof(pthread_c_params));
	p->start_routine = start_routine;
	p->arg = arguments;

	return pthread_create(tid, attr, coyote_new_thread_wrapper, (void*)p);
}

int FFI_pthread_join(pthread_t tid, void* arg){

	FFI_join_operation((long unsigned) tid); // This is a machine & OS specific hack
	return pthread_join(tid, arg);
}

#define pthread_create(x, y, z, a) FFI_pthread_create(x, y, z, a)
#define pthread_join(x, y) FFI_pthread_join(x, y)

/*********************************************** LibEvent APIs **********************************************/

// Declarations for libevent
int FFI_event_set(void* event, int sfd, int flags, void (*event_handler)(int, short, void *), void *arg);
int FFI_event_base_set(void* base, void* event);
int FFI_event_del(void* ev);
int FFI_event_base_loop(void* ev_base, int flags);
int FFI_event_base_loopexit(void* ev_base, void* args);

// libevent APIs
#define event_base_loop(x, y) FFI_event_base_loop(x, y)
#define event_base_set(x, y) FFI_event_base_set(x, y)
#define event_set(x, y, z, a, b) FFI_event_set(x, y, z, a, b)
#define event_del(x) FFI_event_del(x)

#define event_base_loopexit(x, y) FFI_event_base_loopexit(x, y);

/*********************************************** System calls **********************************************/

// Declarations for '-Werror=missing-prototypes'
int FFI_accept(int, void*, void*);
int FFI_getpeername(int sfd, void* addr, void* addrlen);
ssize_t FFI_write(int sfd, void* buff, size_t count);
ssize_t FFI_sendmsg(int sfd, struct msghdr *msg, int flags);
ssize_t FFI_read(int fd, void* buff, int count);
ssize_t FFI_recvfrom(int, void*, size_t, int, struct sockaddr*, socklen_t*);

long long unsigned sfd_counter = 1;

int FFI_accept(int sfd, void* addr, void* addrlen){
	// Return a new sfd! We can return the same sfd also if we want to perform operatoion on an existing connection
	
	// Always return 2, this is a hack to check if the application is trying to write ona  socket
	return 2;
}

// Dummy connection!
int FFI_getpeername(int sfd, void* addr, void* addrlen){

	struct sockaddr_in6 *sockaddr = (struct sockaddr_in6 *)addr;
	sockaddr->sin6_family = AF_INET;
    sockaddr->sin6_port = 8080;
    inet_pton(AF_INET, "192.0.2.33", &(sockaddr->sin6_addr));

    return 0;
}

// It's definition is in the mock_libevent library
ssize_t FFI_event_write(int, void*, size_t);

ssize_t FFI_write(int sfd, void* buff, size_t count){

	ssize_t retval = FFI_event_write(sfd, buff, count);

	// Check if you are tying to signal any event
	if(retval >= 0){

		return retval;
	} else {

		// Means that you are trying to write to a socket
		if(sfd == 2){

			assert(0 && "Missing implementation");
			return -1;
		} else {

			// You are trying to write to an actual file
			return write(sfd, buff, count);
		}
	}
}

ssize_t FFI_sendmsg(int sfd, struct msghdr *msg, int flags){

    assert(strcmp((msg->msg_iov->iov_base), "STORED\r\n") == 0);
    printf("Recieved msg on socket: STORED\n");

    return 0;
}

ssize_t FFI_read(int fd, void* buff, int count){


	if(fd != 2){
		// You are trying to read from an event fd
		
		return read(fd, buff, count);
	} else {

		// You are tyring to read from the socket
		char str[40] = "stats cachedump 0 0\r\n";
		memcpy(buff, str, strlen(str));
		return strlen(str);
	}
}

ssize_t FFI_recvfrom(int socket, void* buffer, size_t length,
       int flags, struct sockaddr* address,
       socklen_t* address_len){

	assert(0 && "Not implemented");
	return 1;
}

// Make it non-blocking. Assign a file descriptor. Set incomming connection params
#define accept(x, y, z) FFI_accept(x, y, z)
#define getpeername(x, y, z) FFI_getpeername(x, y, z)
#define write(x, y, z) FFI_write(x, y, z)
#define read(x, y, z) FFI_read(x, y, z)
#define sendmsg(x, y, z) FFI_sendmsg(x, y, z)
#define recvfrom(a, b, c, d, e, f) FFI_recvfrom(a, b, c, d, e, f)


#endif /* COYOTE_MC_WRAP */