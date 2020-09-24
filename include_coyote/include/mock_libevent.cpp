// Mock of LibEvent APIs using coyote
#ifndef MOCKLIBEVENT
#define MOCKLIBEVENT

#include <include_coyote/include/coyote_c_ffi.h>
#include <event.h>

static int count_event_set = 0;
static void* dispatcher_event = NULL;
static void* dispatcher_event_base = NULL;

static std::map<int, void*>* map_fd_to_event  = NULL;
static std::map<void*, void*>* map_event_to_mocked_event = NULL;
static std::map<void*, void*>* map_eventbase_to_event = NULL; 
static std::map<void*, void*>* map_event_to_lock = NULL;

static void (*dispatcher_event_handler)(int, int, void*) = NULL;
static void (*worker_event_handler)(int, int, void*) = NULL;

struct mocked_event{

	void* orig_event;
	int sfd;
	int which;
	void(callback_method*)(int, short, void *);
	void* args;

	mocked_event(void *ev, void(event_handler*)(int, short, void *), int sfd_o, int which_o, void *arg_o){

		orig_event = ev;
		callback_method  = event_handler;
		sfd = sfd_o;
		which  = which_o;
		args = arg_o;
	}
};

struct worker_locks{

	int lock;
	int cond;
	bool is_lock;

	worker_locks(){
		FFI_pthread_mutex_init(&lock);
		FFI_pthread_cond_init(&cond);
		is_lock = true;
	}
};

int FFI_event_set(void* event, int sfd, int flags, void(event_handler*)(int, short, void *), void *arg){

	// If this is the dispatcher event
	if(count_event_set == 0){
		dispatcher_event = event;
		count_event_set ++;

		assert(dispatcher_event_handler == NULL);
		dispatcher_event_handler = event_handler;
	}

	if(dispatcher_event_handler != event_handler && worker_event_handler == NULL){
		worker_event_handler = event_handler;
	}

	// The input event_handler should be any one of these
	assert((event_handler == worker_event_handler) || (event_handler == dispatcher_event_handler) && "Not possible!");

	if (map_fd_to_event == NULL){
		map_fd_to_event  = new std::map<int, void *>();
	}

	if(map_event_to_mocked_event == NULL){
		map_event_to_mocked_event = new std::map<void*, void*>();
	}

	bool rv = (map_fd_to_event->insert({sfd, event})).second;
	assert(rv && "Insertion to event map failed");

	// I don't knwo what to put in c->which
	mocked_event* ev = new mocked_event(event, event_handler, sfd, -1, arg);

	rv = (map_event_to_mocked_event->insert({event, ev})).second;
	assert(rv && "Insertion to mock event map failed");

	// Now call the actual libevent routine
	return event_set(event, sfd, flags, event_handler, arg);
}

int FFI_event_base_set(void* base, void* event){

	if(map_event_to_mocked_event->find(event) != map_event_to_mocked_event->end()){

		// This is dispatcher_thread
		if(map_eventbase_to_event == NULL){
			map_eventbase_to_event = new std::map<void*, void*>();
		}

		if(event == dispatcher_event){
			dispatcher_event_base = base;
		}

		if(map_event_to_lock == NULL){
			map_event_to_lock = new std::map<void*, void*>();
		}

		if(map_event_to_lock->find(event) == map_event_to_lock->end()){

			worker_locks *wl = new worker_locks();
			map_event_to_lock->insert({event, wl});
		}

		bool rv = (map_eventbase_to_event->insert({base, event}));
		assert(rv && " Unable to insert into map_eventbase_to_event");

		return event_base_set(base, set);
	} else {

		// This is clock handler!
		return event_base_set(base, set);
	}
}

int FFI_event_del(void* ev){

	std::map<void*, void*>::iterator it = map_event_to_mocked_event->find(event);
	assert(it != map_event_to_mocked_event->end() && "This event is not mocked!");

	mocked_event* m_ev = (it.second);

	std::map<int, void*>::iterator it = map_fd_to_event->find(m_ev->sfd);
	assert(it != map_fd_to_event->end() && "Interesting! file descriptor not found");

	map_fd_to_event->remove(it);
	map_event_to_mocked_event->remove(it);
	delete m_ev;

	return event_del(ev);
}

int FFI_event_base_loop(void* ev_base, int flags){

	assert(dispatcher_event_base != NULL && "Dispatcher event base can not be null");

	// Dispatcher thread
	if(ev_base == dispatcher_event_base){

		std::map<void*, void*>::iterator it = map_eventbase_to_event->find(ev_base);
		assert(it != map_eventbase_to_event->end() && "Couldn't find in event_base map");

		std::map<void*, void*>::iterator it1 = map_event_to_mocked_event->find( it.second );
		assert(it1 != map_event_to_mocked_event->end());

		mocked_event* m_ev = (mocked_event*)it1.second;

		dispatcher_event_handler(m_ev->sfd, m_ev->which, m_ev->arg);
		clock_event_handler(0, 0, 0);

		return 0;

	} else { /* Worker threads */

		std::map<void*, void*>::iterator it = map_eventbase_to_event->find(ev_base);
		assert(it != map_eventbase_to_event->end());

		std::map<void*, void*>::iterator it2 = map_event_to_lock->find(it.second);
		assert(it2 != map_event_to_lock->end());

		worker_locks *wl = it2.second;

		while(wl->is_lock){

			FFI_pthread_mutex_lock(&(wl->lock));
			FFI_pthread_cond_wait(&(wl->cond), &(wl->lock));
			FFI_pthread_mutex_unlock(&(wl->lock));

			if(!(wl->is_lock)) break;

			std::map<void*, void*>::iterator i3 = map_event_to_mocked_event->find(it.second);
			assert(it3 != map_event_to_mocked_event->end());

			mocked_event* m_ev = i3.second;

			worker_event_handler(m_ev->sfd, m_ev->which, m_ev->args);
		}

		return 0;
	}
}

int FFI_event_base_loopexit(void* ev_base, void* args){

		std::map<void*, void*>::iterator it = map_eventbase_to_event->find(ev_base);
		assert(it != map_eventbase_to_event->end());

		std::map<void*, void*>::iterator it2 = map_event_to_lock->find(it.second);
		assert(it2 != map_event_to_lock->end());

		worker_locks *wl = it2.second;

		wl->is_lock = false;
		FFI_pthread_cond_signal(&(wl->cond));

		return 0;
}

ssize_t FFI_event_write(int fd, void* buff, size_t count){

	std::map<int, void*>::iterator it = map_fd_to_event->find(fd);

	// If this is not an event fd
	if(it == map_fd_to_event->end()){
		return -1;
	}

	ssize_t retval = write(fd, buff, count);

	std::map<void *, void *>::iterator it2 = map_event_to_lock->find(it.second);
	assert(it2 != map_event_to_lock->end());

	worker_locks* wl = it2.second;

	// Signal the worker!!!!!
	FFI_pthread_cond_signal(&(wl->cond));
	FFI_schedule_next();

	return retval;
}

#endif /* MOCKLIBEVENT */
