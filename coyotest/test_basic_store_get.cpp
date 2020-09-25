// Compile this as: g++ -std=c++11 -shared -fPIC -I./ -g -o libcoyotest.so test_basic_store_get.cpp -g -L/home/udit/memcached_2020/memcached/include_coyote/include -lcoyote_c_ffi -lcoyote

#include <cassert>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <cstring>
#include <vector>
#include <string>

using namespace std;

static int socket_counter = 200;

static map<int, conn*>* map_fd_to_conn = NULL;

struct conn{

	int conn_id; /* Unique dentifier of every connection */
	vector<string>* kv_response; /*  */
	vector<string>* kv_cmd;
	int output_counter;
	int input_counter;
	string expected_response;

	void add_kv_cmd(string ip){

		assert(kv_cmd != NULL);
		kv_cmd->push_back(ip);
	}

	string get_next_cmd(){

		vector<string>::iterator it = kv_cmd->front();

		// TODO: Make sure if the iterator is not end()
		string retval = it->second;
		kv_cmd->erase(it);

		return retval;
	}

	void store_kv_response(char* str){

		string st(str);
		bool rv = (kv_response->push_back(st)).second;
		assert(rv && "Could not insert into the response vector");

		// Check whether this is the expected response or not
		assert( strcmp(str, expected_response.c_str()) == 0);
	}

	string get_kv_response(){

		assert(kv_response != NULL);
		return kv_response->back();
	}

	void set_expected_kv_resp(string st){
		expected_response = st;
	}

	conn(){

		conn_id = socket_counter++;
		kv_response = new vector<string>();
		kv_cmd = new vector<string>();
		output_counter = 0;
		input_counter = 0;
		expected_response = "";

		if(map_fd_to_conn == NULL){
			map_fd_to_conn = new map<int, conn*>();
		}

		bool rv = (map_fd_to_conn->insert({conn_id, this})).second;
		assert(rv && "Insertion to map_fd_to_conn failed!");
	}
};

vector<conn*>* global_conns;

extern "C"{

#include <coyote_c_ffi.h>

bool CT_is_socket(int fd){

	// Check if this is in the map of allocated connections
	if(map_fd_to_conn->find(fd) != map_fd_to_conn->end()){

		return true;
	}
	else {

		return false;
	}
}

ssize_t CT_socket_read(int fd, const void* buff, int count){
	assert(0 && "Not implemented!");
	return -1;
}


ssize_t CT_socket_write(int fd, void* buff, int count){

	assert(CT_is_socket(fd) && "This is not the socket we have opened!");

	map<int, conn*>::iterator it = map_fd_to_conn->find(fd);
	conn* obj = it->second;

	string st = obj->get_next_cmd();
	char msg[1024];

	// Convert string object to C strings and copy it info the buffer
	strcpy(msg, st.c_str());
	memcpy(buff, msg, strlen(msg));

	return strlen(msg);
}

ssize_t CT_socket_recvmsg(int fd, struct msghdr *msg, int flags){

	assert(CT_is_socket(fd));

	// Get the conn object corresponding to this fd
	map<int, conn*>::iterator it = map_fd_to_conn->find(fd);
	conn* obj = it->second;

	// Store the incomming response in the vector
	obj->store_kv_response((char*)(msg->msg_iov->iov_base));

	printf("Recieved on connection number %d, msg: %s", fd, (char*)(msg->msg_iov->iov_base));

	return strlen((char*)(msg->msg_iov->iov_base));
}

int CT_new_socket(){

	if(global_conns == NULL){
		global_conns = new vector<conn*>();
	}

	conn* new_con = new conn(socket_counter);
	global_conns->push_back(new_con);

	return socket_counter++;
}

ssize_t CT_socket_sendto(int socket, void* buffer, size_t length,
       int flags, struct sockaddr* address,
       socklen_t* address_len){

	assert(0 && "Not implemented");
	return -1;
}

// Test main method
int CT_main( int (*run_iteration)(int, char**), int argc, char** argv ){

	conn* c1 = new conn();
	c1->add_kv_cmd("stats cachedump 0 0\r\n");
	c1->set_expected_kv_resp("END\r\n");

	FFI_create_scheduler();

	int num_iter = 10;

	// Lights, Camera, Action!
	for(int j = 0; j < num_iter; j++){

		printf("Starting iteration #%d \n", j);

		FFI_attach_scheduler();

		run_iteration(argc, argv);

		FFI_detach_scheduler();
		FFI_scheduler_assert();

	}

	FFI_delete_scheduler();

	return 0;
}

} /* extern "C" */