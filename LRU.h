#ifndef LRU_H
#define LRU_H
#include<iostream>
#include<vector>

using namespace std;
/*
typedef struct metadata{
	bool dirty;
	bool pin;
	char *memory;
	metadata (){
		dirty = false;
		pin = false;
	}
}metadata;
*/
typedef struct filenode{
	int file_desc;
	vector <int> pages_in_buff;
}filenode;	

typedef struct node{
	bool dirty;
//	bool pin;
	int file_desc;
	int file_pos;
	int timestamp;
//	int index;
	char *mem;
	node(){
		dirty = false;
//		pin = false;
	}
}node;
	
class LRU{
	private:
	int pagesize, buffpages;
	
	public:
	int fds[100];
	
	void Initialize (char *catalog);
	int Open (char *filePath, int numPages);
	int Close (int fd);
	int Read (int fd, char **intoMe, int numPages);
	int Write (int fd, char **fromMe, int numPages);
	int NewPages (char **intoMe, int numPages);
	int FreePages (char **fromMe, int numPages);
	int Seek(int fd, int pos);
	int Sync(int fd);
	int GetUnusedPageCount();
	int getNextIndex();
	int Pin(char *ptr);
	int UnPin(char *ptr);

//	LRU ( );
//	~LRU ();
};


#endif
