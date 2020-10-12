#ifndef COYOTE_MC_REDEF
#define COYOTE_MC_REDEF

/****************************** Reset Global variables **************************/
#ifdef IN_LOGGER_FILE

static logger *logger_stack_head;
static logger *logger_stack_tail;
static unsigned int logger_count;
static volatile int do_run_logger_thread;
pthread_mutex_t logger_stack_lock;
pthread_mutex_t logger_atomics_mutex;
int watcher_count;
static uint64_t logger_gid;

void reset_logger_globals(){

    logger_stack_head = NULL;
    logger_stack_tail = NULL;
    logger_count = 0;
    do_run_logger_thread = 1;
    FFI_pthread_mutex_lazy_init(&logger_stack_lock);
    FFI_pthread_mutex_lazy_init(&logger_atomics_mutex);
    watcher_count = 0;
    logger_gid = 0;
}
#endif /*IN_LOGGER_FILE*/

#ifdef IN_MEMCACHED_FILE

static volatile bool allow_new_conns;
void *ext_storage;
static conn *listen_conn;
static int stop_main_loop;

void reset_memcached_globals(){

	allow_new_conns = true;
	ext_storage = NULL;
	listen_conn = NULL;
	stop_main_loop = 0;
}
#endif /*IN_MEMCACHED_FILE*/

#ifdef IN_THREAD_FILE

pthread_mutex_t atomics_mutex;
pthread_mutex_t conn_lock;
static int init_count;
static int last_thread;
static pthread_mutex_t stats_lock;

void reset_thread_globals(){

	FFI_pthread_mutex_lazy_init(&atomics_mutex);
	FFI_pthread_mutex_lazy_init(&conn_lock);
	FFI_pthread_mutex_lazy_init(&stats_lock);
	init_count = 0;
	last_thread = -1;
}
#endif /*IN_THREAD_FILE*/

#ifdef IN_ASSOC_FILE

static volatile int do_run_maintenance_thread;
static unsigned int expand_bucket;
static bool expanding;
int hash_bulk_move;
unsigned int hashpower;
static pthread_cond_t maintenance_cond;
static pthread_mutex_t maintenance_lock;
static item** old_hashtable;
static item** primary_hashtable;

void reset_assoc_globals(){

	do_run_maintenance_thread = 1;
	expand_bucket = 0;
	expanding = false;
    hash_bulk_move = 1;
    hashpower = 16;
    FFI_pthread_cond_lazy_init(&maintenance_cond);
    FFI_pthread_mutex_lazy_init(&maintenance_lock);
    old_hashtable = NULL;
    primary_hashtable = NULL;
}
#endif /*IN_ASSOC_FILE*/

#ifdef IN_CRAWLER_FILE

static int crawler_count;
static volatile int do_run_lru_crawler_thread;
static pthread_cond_t lru_crawler_cond;
static int lru_crawler_initialized;
static pthread_mutex_t lru_crawler_lock;

void reset_crawler_globals(){

	crawler_count = 0;
	do_run_lru_crawler_thread = 0;
	FFI_pthread_cond_lazy_init(&lru_crawler_cond);
	FFI_pthread_mutex_lazy_init(&lru_crawler_lock);
	lru_crawler_initialized = 0;
}
#endif /*IN_CRAWLER_FILE*/

#ifdef IN_ITEMS_FILE

static pthread_mutex_t bump_buf_lock;
static uint64_t cas_id;
static pthread_mutex_t cas_id_lock;
static volatile int do_run_lru_maintainer_thread;
static int lru_maintainer_initialized;
static pthread_mutex_t lru_maintainer_lock;
static int stats_sizes_buckets;
static uint64_t stats_sizes_cas_min;
static unsigned int *stats_sizes_hist;
static pthread_mutex_t stats_sizes_lock;

void reset_items_globals(){

	FFI_pthread_mutex_lazy_init(&bump_buf_lock);
	FFI_pthread_mutex_lazy_init(&cas_id_lock);
	FFI_pthread_mutex_lazy_init(&lru_maintainer_lock);
	FFI_pthread_mutex_lazy_init(&stats_sizes_lock);
	cas_id = 0;
	do_run_lru_maintainer_thread = 0;
	lru_maintainer_initialized = 0;
	stats_sizes_buckets = 0;
	stats_sizes_cas_min = 0;
	stats_sizes_hist = NULL;
}
#endif /*IN_ITEMS_FILE*/

#ifdef IN_SLABS_FILE

static volatile int do_run_slab_rebalance_thread;
static size_t mem_avail;
static void *mem_base;
static void *mem_current;
static size_t mem_limit;
static bool mem_limit_reached;
static size_t mem_malloced;
static pthread_cond_t slab_rebalance_cond;
static pthread_mutex_t slabs_lock;
static pthread_mutex_t slabs_rebalance_lock;
static void *storage;

void reset_slabs_globals(){

	do_run_slab_rebalance_thread = 1;
	mem_avail = 0;
	mem_base = NULL;
	mem_current = NULL;
	mem_limit = 0;
	mem_limit_reached = false;
	mem_malloced = 0;
	FFI_pthread_cond_lazy_init(&slab_rebalance_cond);
	FFI_pthread_mutex_lazy_init(&slabs_lock);
	FFI_pthread_mutex_lazy_init(&slabs_rebalance_lock);
	storage = NULL;
}
#endif /*IN_SLABS_FILE*/

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

#define setbuf(x, y) { setbuf(x, y); FFI_register_clock_handler(clock_handler); FFI_register_main_stop(&stop_main_loop);}

// #define COYOTE_2019_BUGS // For introducing data race bugs

#endif /* COYOTE_MC_REDEF */