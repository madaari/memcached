#ifndef COYOTE_MC_REDEF
#define COYOTE_MC_REDEF

#include <sys/socket.h>

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

#define pthread_create(x, y, z, a) FFI_pthread_create(x, y, z, a)
#define pthread_join(x, y) FFI_pthread_join(x, y)

/*********************************************** LibEvent APIs **********************************************/

#define event_base_loop(x, y) FFI_event_base_loop(x, y)
#define event_base_set(x, y) FFI_event_base_set(x, y)
#define event_set(x, y, z, a, b) FFI_event_set(x, y, z, a, b)
#define event_del(x) FFI_event_del(x)
#define event_add(x, y) FFI_event_add(x, y)

#define event_base_loopexit(x, y) FFI_event_base_loopexit(x, y);

/*********************************************** System calls **********************************************/

// Make it non-blocking. Assign a file descriptor. Set incomming connection params
#define accept(x, y, z) FFI_accept(x, y, z)
#define getpeername(x, y, z) FFI_getpeername(x, y, z)
#define write(x, y, z) FFI_write(x, y, z)
#define read(x, y, z) FFI_read(x, y, z)
#define sendmsg(x, y, z) FFI_sendmsg(x, y, z)
#define recvfrom(a, b, c, d, e, f) FFI_recvfrom(a, b, c, d, e, f)
#define fcntl(x, ...) FFI_fcntl(x, __VA_ARGS__)
#define pipe(x) FFI_pipe(x)
#define poll(x, y, z) FFI_poll(x, y, z)

#ifndef IOV_MAX
# define IOV_MAX 1024
/* GNU/Hurd don't set MAXPATHLEN
 * http://www.gnu.org/software/hurd/hurd/porting/guidelines.html#PATH_MAX_tt_MAX_PATH_tt_MAXPATHL */
#ifndef MAXPATHLEN
#define MAXPATHLEN 4096
#endif
#endif

// We doen't yet support accept4() sys call while testing
#undef HAVE_ACCEPT4

#define main(x, y) run_coyote_iteration(x, y)

#define usleep(x) {usleep(0); FFI_schedule_next();}

#define setbuf(x, y) { setbuf(x, y); FFI_register_clock_handler(clock_handler); }

// #define COYOTE_2019_BUGS // For introducing data race bugs

#endif /* COYOTE_MC_REDEF */