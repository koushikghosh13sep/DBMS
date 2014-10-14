#ifndef STRUCT_H
#define STRUCT_H

#include<vector>

#include<sys/types.h>

#define INIT -1
//#define INIT_POS 0

using namespace std;
/*
*
*/
class Fd_tree_node{
	private:
		int fd;
		vector<int> buf_pages_in_use;
		off_t currPos;
	public:
		static int compare_fd(const void *first,const void *second,void *param);
		vector<int> get_copyOf_pages_in_use_vector();
		void add_page_in_use(int index);
		void remove_page_in_use(int index);
		void clear_pages_in_use();
//		void vector<int> set_buf_pages_in_use_vector(int);
		int get_fd();
		void set_fd(int fd);
		off_t get_currPos();
		void set_currPos(off_t pos);
		Fd_tree_node();
		Fd_tree_node(int fd);
		Fd_tree_node(int fd,off_t currPos);
};
/*
*
*/
class Fd_pos_tree_node /*: public Fd_tree_node*/{
	private:
		int fd;
		off_t pos; /* may need to be changed */
		int index_to_page;
	public:
		static int compare_fdpos(const void *first,const void *second,void *param);
		int get_fd();
		void set_fd(int fd);
		int get_index_to_page();
		void set_index_to_page(int index_to_page);
		off_t get_pos();
		void set_pos(off_t pos);
		Fd_pos_tree_node();
		Fd_pos_tree_node(int fd,off_t pos);
		Fd_pos_tree_node(int fd,off_t pos,int index);
};
/*
*
*/
class Timestamp_tree_node{
	private:
		long timestamp;
		int page_index;
	public:
		static int compare_timestamp(const void *first,const void *second,void *param);
		int get_page_index();
		void set_page_index(int page_index);
		void set_timestamp(long timestamp);
		long get_timestamp();
		Timestamp_tree_node(long timestamp, int page_index);
		Timestamp_tree_node();
};

#define BUFFER_POOL_PAGE_DIRTY true
#define BUFFER_POOL_PAGE_CLEAN false
/*
*
*/
class Buffer_pool_node: public Fd_pos_tree_node, public Timestamp_tree_node{
	private:
		char *page;
		bool dirt;
	public:
		void set_page_ptr(char *ptr);
		char* get_page_ptr();
		bool isDirty();
		void mark_dirty();
		void mark_clean();
		void reset();

		Buffer_pool_node();
//		Buffer_pool_node(char *page,int fd,off_t pos,off_t currPos);
};
#endif
