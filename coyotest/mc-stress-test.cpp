// Compile this as: g++ -std=c++11 -shared -fPIC -I./ -g -o libcoyotest.so test_basic_store_get.cpp -g -L/home/udit/memcached_2020/memcached/include_coyote/include -lcoyote_c_ffi -lcoyote
#include <test_template.h>

using namespace std;

vector<conn*>* global_conns;


char* get_key_name(int i, char prefix = ' '){

	assert( i > 0 && "key shouldn't be less than or equal to 0");

	if(prefix == ' '){

		string st("key_");
		st = st + to_string(i);

		char* retval = (char*)malloc( sizeof(char) * (st.length() + 10));
		strcpy(retval, st.c_str());

		return retval;
	}
	else{

		char* pre = (char*)malloc(sizeof(char)*5);
		pre[0] = prefix;
		pre[1] = '\0';

		string prefix(pre);
		string st("key_");
		st = prefix + st + to_string(i);

		char* retval = (char*)malloc( sizeof(char) * (st.length() + 10));
		strcpy(retval, st.c_str());

		return retval;
	}
}

extern "C"{

int count_num_sockets = 1;

// This function will be called when we recieve response for this request
ssize_t parse_get_response(struct msghdr *msg, string value){

	ssize_t retval = 0;

	if(msg->msg_iovlen <= 1){

		char* msg1 = (char*)( ((struct iovec *)(msg->msg_iov))->iov_base);
		retval = strlen(msg1);

		// This will happen when the iten is not in the kv store
		assert( strcmp(msg1, "END\r\n") == 0);
	}
	else{ // Means that some value is returned!

		char* msg1 = (char*)(msg->msg_iov->iov_base);
		char* msg2 = (char*)(msg->msg_iov[1].iov_base);
		retval = msg->msg_iov->iov_len + msg->msg_iov[1].iov_len + strlen("END\r\n");

		string st(msg2);
		assert( st.find(value) != string::npos );
	}

	return retval;
}

// Used for direct comparision of KV store's response to given value
ssize_t parse_generic_response(struct msghdr *msg, string value){

	string response((char*)(msg->msg_iov->iov_base));

	assert(response.find(value) != string::npos && "We can find the string 'value' in KV store's respone");

	return (msg->msg_iov->iov_len);
}

ssize_t parse_stats_slabs_response(struct msghdr *msg, string value){

	int retval = 0;
	bool isFound = false;

	for(int i = 0; i < msg->msg_iovlen; i++){

		retval += msg->msg_iov[i].iov_len;
		string st((char*)(msg->msg_iov[i].iov_base));

		// Check whether the metadump contains the required string or not
		if(st.find(value) != string::npos){
			isFound = true;
		}
	}

	assert(isFound && "Value not found in the return string");

	return retval;
}

ssize_t parse_stats_items_response(struct msghdr *msg, string value){

	int retval = 0;
	bool isFound = false;

	for(int i = 0; i < msg->msg_iovlen; i++){

		retval += msg->msg_iov[i].iov_len;
		string st((char*)(msg->msg_iov[i].iov_base));

		// Check whether the metadump contains the required string or not
		if(st.find(value) != string::npos){
			isFound = true;
		}
	}

	assert(isFound && "Value not found in the return string");

	return retval;
}

ssize_t parse_lru_crawler_metadump_response(char* buff, string value){

	int total_key_count = 0;
	int retval = strlen(buff);

	string data(buff);
	string toSearch("key=");

	// Get the first occurrence
    size_t pos = data.find(toSearch);

    // Repeat till end is reached
    while( pos != std::string::npos)
    {
        // Add position to the vector
        total_key_count++;

        // Get the next occurrence from the current position
        pos = data.find(toSearch, pos + toSearch.size());
    }

    assert(value == to_string(total_key_count) && "Make sure the total number of keys are same");

	return retval;
}

void set_workload(conn* c){

	// Random number between 1 and 50
	int max = rand() % 50 + 1;
    int sets = 0;

    for (int i = 1; i <= max; i++) {

        char* key = get_key_name(i);
        c->set_key(key, key);
        sets++;
    }

    int max_iteration = rand() % 500;
    for (int i = 1; i < max_iteration; i++) {

    	// Random number from 1 to max
    	int ran = rand() % max + 1;
    	char* key = get_key_name(ran);
    	int meth = rand() % 3;
    	int exp = rand() % 3;

    	if(meth == 0){

    		c->add_key(key, key, exp);

    	} else if(meth == 1){

    		c->delete_key(key);

    	} else {

    		c->set_key(key, key, exp);
    	}

    	ran = rand() % max + 1;
    	key = get_key_name(ran);

    	c->get_and_assert_key(key, key);
    }

    for (int i = 1; i <= max; i++) {

        char* key = get_key_name(i);
        c->get_key(key);
    }
}

void set_workload_lru(conn* obj){

	for(int i = 1; i <= 3; i++){

		char* key = get_key_name(i, 'i');
		obj->set_key(key, "ok", 0, 1);
		obj->set_expected_kv_resp("generic", "STORED\r\n");
	}

	for(int i = 1; i <= 3; i++){

		char* key = get_key_name(i, 'l');
		obj->set_key(key, "ok", 3600, 1);
		obj->set_expected_kv_resp("generic", "STORED\r\n");
	}

	for(int i = 1; i <= 3; i++){

		char* key = get_key_name(i, 's');
		obj->set_key(key, "ok", 1, 1);
		obj->set_expected_kv_resp("generic", "STORED\r\n");
	}

	obj->get_mem_stats_and_assert("slabs", "1:used_chunks", 9);

	sleep(1);

	{
		string lru_crawler("lru_crawler enable\r\n");
		obj->add_kv_cmd(lru_crawler);
		// It can fail also becoz LRU crawler might be running
		obj->set_expected_kv_resp( obj->char_to_string("generic"), obj->char_to_string("OK\r\n") );

		string lru_crawler1("lru_crawler crawl 1\r\n");

		for(int i =0; i < 2000; i++){
			obj->add_kv_cmd(lru_crawler1);
			obj->set_expected_kv_resp( obj->char_to_string("generic"), obj->char_to_string("\r\n") );
		}

		obj->get_mem_stats_and_assert("slabs", "1:used_chunks", 6);
		obj->get_mem_stats_and_assert("items", "items:1:crawler_reclaimed", 3);

		string lru_crawler2("lru_crawler metadump all\r\n");
		obj->add_kv_cmd(lru_crawler2);
		obj->set_expected_kv_resp( obj->char_to_string("lru_crawler metadump"),
								   obj->char_to_string("6") );
	}

	for(int i = 1; i <= 30; i++){

		char* skey = get_key_name(i, 's');
		char* lkey = get_key_name(i, 'l');
		char* ikey = get_key_name(i, 'i');

		// We can put anything here. This should be undef
		obj->get_and_assert_key(skey, "k");
		obj->get_and_assert_key(lkey, "ok");
		obj->get_and_assert_key(ikey, "ok");
	}

	string lru_crawler3("lru_crawler disable\r\n");
	obj->add_kv_cmd(lru_crawler3);
	obj->set_expected_kv_resp( obj->char_to_string("generic"), obj->char_to_string("OK\r\n") );

	// Again store the keys with TTL=1sec
	for(int i = 1; i <= 30; i++){

		char* key = get_key_name(i, 's');
		obj->set_key(key, "ok", 1, 1);
		obj->set_expected_kv_resp("generic", "STORED\r\n");
	}

	sleep(3);

	{
		string lru_crawler("lru_crawler enable\r\n");
		obj->add_kv_cmd(lru_crawler);
		obj->set_expected_kv_resp( obj->char_to_string("generic"), obj->char_to_string("OK\r\n") );

		string lru_crawler1("lru_crawler crawl 1\r\n");
		for(int i =0; i < 2000; i++){
			obj->add_kv_cmd(lru_crawler1);
			obj->set_expected_kv_resp( obj->char_to_string("generic"), obj->char_to_string("\r\n") );
		}
	}

	obj->get_mem_stats_and_assert("slabs", "1:used_chunks", 6);
}


void set_large_workload(conn* obj){

	// Each value is of 4 KB
	char* long_val = (char*)malloc(sizeof(char) * 1000 * 4);
	memset(long_val, '1', sizeof(char) * 1000 * 4);

	for(int i = 1; i <= 500; i++){

		char* key = get_key_name(i);
		obj->set_key(key, long_val, 1);
	}

	for(int i = 1; i <= 500; i++){

		char* key = get_key_name(i, 's');
		obj->set_key(key, long_val, 10);
	}

	for(int i = 1; i <= 500; i++){

		char* key = get_key_name(i, 'i');
		obj->set_key(key, long_val, 0);
	}

	for(int i = 1; i <= 500; i++){

		char* key = get_key_name(i, 'i');
		obj->get_key(key);

		key = get_key_name(i, 's');
		obj->get_key(key);

		key = get_key_name(i);
		obj->get_key(key);
	}
}

void init_sockets(){

	if(global_conns == NULL){
		global_conns = new vector<conn*>();
	}

	for(int i=0; i < count_num_sockets; i++){

		conn* new_con = new conn();
		//set_workload(new_con);  // <<--- General Stress testing of memcached
		set_workload_lru(new_con); // <<--- For Data race bug
		global_conns->push_back(new_con);
	}
}

void del_sockets(){

	for(int i = 0; i < count_num_sockets; i++){
		conn* c = global_conns->at(i);
		delete c;
		c = NULL;
	}

	delete global_conns;
	global_conns = NULL;
}

bool CT_is_socket(int fd){

	// Check if this is in the map of allocated connections
	return (map_fd_to_conn->find(fd) != map_fd_to_conn->end());
}

ssize_t CT_socket_write(int fd, void* buff, int count){

	assert(CT_is_socket(fd) && "This is not the socket we have opened!");

	map<int, void*>::iterator it = map_fd_to_conn->find(fd);
	conn* obj = (conn*)(it->second);

	string st = obj->get_next_cmd();
	char msg[1024];

	// Convert string object to C strings and copy it info the buffer
	strcpy(msg, st.c_str());
	memcpy(buff, msg, strlen(msg));

	return strlen(msg);
}

ssize_t CT_socket_read(int fd, const void* buff, int count){

	assert(CT_is_socket(fd));

	conn* obj = NULL;

	{
		// Get the conn object corresponding to this fd
		map<int, void*>::iterator it = map_fd_to_conn->find(fd);
		obj = (conn*)(it->second);
	}

	vector< pair<string, string>* >::iterator it = obj->expected_response->begin();

	// Check if the container is not empty
	if(it != obj->expected_response->end()){
		pair<string, string>* p = *it;

		if(p->first == obj->char_to_string("lru_crawler metadump")){

			ssize_t retval = parse_lru_crawler_metadump_response((char*)buff, p->second);

			printf("Recieved on connection number %d, lru crawler metadump with keys: %s ", fd, (p->second).c_str());
			obj->expected_response->erase(it);
			return count;
		}
	}

	assert(0);
	return 0;
}

ssize_t CT_socket_recvmsg(int fd, struct msghdr *msg, int flags){

	assert(CT_is_socket(fd));

	conn* obj = NULL;

	{
		// Get the conn object corresponding to this fd
		map<int, void*>::iterator it = map_fd_to_conn->find(fd);
		obj = (conn*)(it->second);
	}

	vector< pair<string, string>* >::iterator it = obj->expected_response->begin();

	// Check if the container is not empty
	if(it != obj->expected_response->end()){

		pair<string, string>* p = *it;

		if(p->first == obj->char_to_string("get")){

			ssize_t retval = parse_get_response(msg, p->second);

			printf("Recieved on connection number %d, msg: %s", fd, (char*)(msg->msg_iov->iov_base));
			obj->expected_response->erase(it);
			return retval;
		}

		if(p->first == obj->char_to_string("stats slabs")){

			ssize_t retval = parse_stats_slabs_response(msg, p->second);

			printf("Recieved on connection number %d, metadump with val: %s", fd, (p->second).c_str());
			obj->expected_response->erase(it);
			return retval;
		}

		if(p->first == obj->char_to_string("stats items")){

			ssize_t retval = parse_stats_items_response(msg, p->second);

			printf("Recieved on connection number %d, metadump with val: %s", fd, (p->second).c_str());
			obj->expected_response->erase(it);
			return retval;
		}

		if(p->first == obj->char_to_string("generic")){

			ssize_t retval = parse_generic_response(msg, p->second);

			printf("Recieved on connection number %d, msg: %s", fd, (char*)(msg->msg_iov->iov_base));
			obj->expected_response->erase(it);
			return retval;
		}

	}

	printf("Recieved on connection number %d, msg: %s", fd, (char*)(msg->msg_iov->iov_base));
    assert(0);
	return strlen((char*)(msg->msg_iov->iov_base));
}


int CT_new_socket(){

	static int i = 0;

	// Block the dispatcher thread, when we have already created all the sockets
	if(count_num_sockets == i){
		//while(1){
		//	FFI_schedule_next();
		//}
		return -1;
	}

	conn* c = global_conns->at(i);
	i++;

	return c->conn_id;
}

int set_options(int argc, char** argv, char** new_argv){

	int i = 0;
	for(i = 0; i < argc; i++){
		new_argv[i] = argv[i];
	}

	char new_opt[4][30] = {"-m", "32", "-o", "no_modern"};
	int num_new_opt = 4;

	for(int j = 0; i < (argc + num_new_opt); i++, j++){

		memcpy(new_argv[i], new_opt[j], 30);
	}

	return argc + num_new_opt;
}

// Test main method
int CT_main( int (*run_iteration)(int, char**), int argc, char** argv ){

	FFI_create_scheduler();

	int num_iter = 10;

	char **new_argv = (char **)malloc(50 * sizeof(char *));
	for(int i = 0; i < 50; i++){
		new_argv[i] = (char *)malloc(50 * sizeof(char));
	}

	int new_argc = set_options(argc, argv, new_argv);

	// Lights, Camera, Action!
	for(int j = 0; j < num_iter; j++){

		printf("Starting iteration #%d \n", j);

		FFI_attach_scheduler();

		init_sockets();

		run_iteration(new_argc, new_argv);

		FFI_detach_scheduler();
		FFI_scheduler_assert();

		del_sockets();
	}

	FFI_delete_scheduler();

	return 0;
}

} /* extern "C" */