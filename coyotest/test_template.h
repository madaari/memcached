#include <cassert>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <cstring>
#include <vector>
#include <string>
#include <map>
#include <unistd.h>

extern "C"{
	#include <coyote_c_ffi.h>
}

using namespace std;

static int socket_counter = 200;
map<int, void*>* map_fd_to_conn = NULL;

struct conn{

	int conn_id; /* Unique dentifier of every connection */
	vector<string>* kv_response;
	vector<string>* kv_cmd;
	int output_counter;
	int input_counter;

	// We need a pair to indicate the type and value of response
	vector< pair<string, string>* >* expected_response;

	void add_kv_cmd(string ip){

		assert(kv_cmd != NULL);
		kv_cmd->push_back(ip);
	}

	string char_to_string(const char* inp){

		string a(inp);
		return a;
	}

	void set_key(const char* key, const char* val, int expr = 0, bool isReply = false){

		string base("set ");
		base = base + char_to_string(key);
		base = base + char_to_string(" 01 ") + to_string(expr);
		base = base + char_to_string(" ") + to_string(strlen(val));

		if(isReply)
			base = base + char_to_string("\r\n") + char_to_string(val);
		else
			base = base + char_to_string(" noreply\r\n") + char_to_string(val);

		base = base + char_to_string("\r\n");

		add_kv_cmd(base);
	}

	void add_key(const char* key, const char* val, int expr = 0){

		string base("add ");
		base = base + char_to_string(key);
		base = base + char_to_string(" 01 ") + to_string(expr);
		base = base + char_to_string(" ") + to_string(strlen(val));
		base = base + char_to_string(" noreply\r\n") + char_to_string(val);
		base = base + char_to_string("\r\n");

		add_kv_cmd(base);
	}

	void get_mem_stats_and_assert(const char* type, const char* param, int val){

		string base("stats ");
		base = base + char_to_string(type);
		base = base + char_to_string("\r\n");

		add_kv_cmd(base); // Add the command

		// Command to assert the result
		{

			string resp("stats ");
			resp = resp + char_to_string(type);

			string resp1("");
			resp1 = resp1 + char_to_string(param) + char_to_string(" ");

			// Don't put \r\n for stats items
			if(strcmp(type, "items") == 0)
				resp1 = resp1 + to_string(val);
			else
				resp1 = resp1 + to_string(val) + char_to_string("\r\n");

			set_expected_kv_resp(resp, resp1);
		}
	}

	void delete_key(const char* key){

		string base("delete ");
		base = base + char_to_string(key);
		base = base + char_to_string(" noreply\r\n");

		add_kv_cmd(base);
	}

	void get_and_assert_key(const char* key, const char* value){

		string base("get ");
		base = base + char_to_string(key);
		base = base + char_to_string("\r\n");

		add_kv_cmd(base);

		// Make sure the value have \r\n statements
		set_expected_kv_resp( char_to_string("get"), char_to_string(value) + char_to_string("\r\n"));
	}

	void get_key(const char* key){

		string base("get ");
		base = base + char_to_string(key);
		base = base + char_to_string("\r\n");

		add_kv_cmd(base);
	}

	string get_next_cmd(){

		string retval = kv_cmd->front();

		vector<string>::iterator it = kv_cmd->begin();

		// Means there is no more command to give. Block
		if(it == kv_cmd->end()){
			return string("quit\r\n");
		}

		// TODO: Make sure if the iterator is not end()
		kv_cmd->erase(it);

		return retval;
	}

	void store_kv_response(char* str){

		string st(str);
		kv_response->push_back(st);
	}

	string get_kv_response(){

		assert(kv_response != NULL);
		return kv_response->back();
	}

	// Create a pair and store it in the vector!
	void set_expected_kv_resp(string type, string value){

		expected_response->push_back( new pair<string, string>(type, value) );
	}

	conn(){

		conn_id = socket_counter++;
		kv_response = new vector<string>();
		kv_cmd = new vector<string>();
		expected_response = new vector< pair<string, string>* >();
		output_counter = 0;
		input_counter = 0;

		if(map_fd_to_conn == NULL){
			map_fd_to_conn = new map<int, void*>();
		}

		bool rv = (map_fd_to_conn->insert({conn_id, this})).second;
		assert(rv && "Insertion to map_fd_to_conn failed!");
	}
};

extern "C"{

	ssize_t CT_socket_sendto(int socket, void* buffer, size_t length,
       int flags, struct sockaddr* address,
       socklen_t* address_len){

		assert(0 && "Not implemented");
		return -1;
	}
}