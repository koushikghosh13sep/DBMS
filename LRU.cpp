#include<iostream>
#include<cstdlib>
#include"LRU.h"
#include<cstring>
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<fstream>
#include"avl.h"
#include<errno.h>
using namespace std;

int access_time = -1;
vector <char*> free_pages;
vector<node*> pin;
char **memory;
avl_tree *nodeTree_fd_pos, *nodeTree_timestamp, *filenodeTree_fd;

int Compare_fd_pos(const void *f, const void *s, void *param){
	
	node *first = (node*) f;
	node *second = (node*) s;

	if(first -> file_desc == second -> file_desc){
		if(first -> file_pos < second -> file_pos)
			return -1;
		else if(first -> file_pos > second -> file_pos)
			return 1;
		else
			return 0;
	}

	if (first -> file_desc < second -> file_desc)
		return -1;
	else 
		return 1;
}
int Compare_fd(const void *f, const void *s, void *param){
	
	filenode *first = (filenode*) f;
	filenode *second = (filenode*) s;
	
	if(first -> file_desc < second -> file_desc)
		return -1;
	else if(first -> file_desc > second -> file_desc)
		return 1;
	else
		return 0;
}
int Compare_timestamp(const void *f, const void *s, void *param){
	
	node *first = (node*) f;
	node *second = (node*) s;
	
	if(first -> timestamp < second -> timestamp)
		return -1;
	else if(first -> timestamp > second -> timestamp)
		return 1;
	else
		return 0;
}
void LRU :: Initialize(char *catalog){
	ifstream fp;
	char array[20];
	fp.open(catalog,ios::in);
	if(!fp){
		cout << "File cannot be opened";
		exit(1);
	}
	while(!fp.eof()){
		fp >> array;
		if(strcmp(array,"PAGESIZE") == 0)
			fp >> pagesize;
		if(strcmp(array,"BUFFPAGES") == 0)
			fp >> buffpages;
	}	

	for(int i = 0 ; i < buffpages ; i++){
		char *memory = new char[pagesize];
		free_pages.push_back(memory);
	}
		
	nodeTree_fd_pos = avl_create(Compare_fd_pos, NULL);
	nodeTree_timestamp = avl_create(Compare_timestamp, NULL);
	filenodeTree_fd = avl_create(Compare_fd, NULL);

}
int LRU :: Open(char *filePath , int numPages){
//cout<<"open start\n";
	int fd;
	if(filePath == NULL)
		cout<<"invalid file name\n";

	if(numPages == 0){
		fd = open(filePath,O_RDWR);
		if(fd == -1)
			cout<<"file can not open\n";
	}else{
		fd = open(filePath, O_RDWR | O_CREAT,S_IREAD | S_IWRITE);
		if(fd < 0)
			return fd;
	
		char buff[] = "NewPage!";
		for (int i = 0;i < numPages*(pagesize/8);i++){  
			write(fd,buff,8);
		}
	}
	lseek(fd, 0, SEEK_SET);
	
	filenode *dummy=new filenode;
	dummy->file_desc = fd;
	avl_insert(filenodeTree_fd,dummy); 
//cout<<"open end\n";
	return fd;
}

int LRU :: Close(int fd){
//cout<<"close start\n";
	filenode  *temp;
	filenode dummy;
	dummy.file_desc = fd;

	temp = (filenode *) avl_find(filenodeTree_fd, &dummy);
	if(temp == NULL){
		cout << fd << " file does not exist\n";
		return -1;
	}
	while(temp -> pages_in_buff.size() > 0){
		
		node *f,temp1;
		temp1.file_desc = fd;
		temp1.file_pos = temp->pages_in_buff[temp -> pages_in_buff.size() - 1];
		temp->pages_in_buff.pop_back();

		f = (node *)avl_find(nodeTree_fd_pos,&temp1); 
		avl_delete(nodeTree_fd_pos, f);
		avl_delete(nodeTree_timestamp, f);
	
		if(f->dirty){
			lseek(fd, f -> file_pos * pagesize,SEEK_SET);
			write(fd, f -> mem , pagesize);
		}	
		free_pages.push_back(f -> mem);		
		delete f;
	}
	avl_delete(filenodeTree_fd,temp);

//cout<<"close end\n";
	return close(fd);
}

int LRU :: Read (int fd, char **intoMe, int numPages){
//cout<<"read start\n";
	for(int i = 0; i < numPages; i++){
		node temp;
		temp.file_desc = fd;
		temp.file_pos = lseek(fd, 0, SEEK_CUR) / pagesize;
		node *temp1;
		temp1 = (node*) avl_find (nodeTree_fd_pos, &temp);
		if(temp1 != NULL){
			intoMe[i] = temp1->mem;
			avl_delete(nodeTree_timestamp,temp1);
			access_time++;
			temp1->timestamp = access_time;
			avl_insert(nodeTree_timestamp, temp1);
			lseek(fd, pagesize, SEEK_CUR);
		} else {
			char *page;//=new char [pagesize];
			
			NewPages(&page,1);
			read(fd, page, pagesize);
			temp1 = new node;
			temp1 -> mem = page;
			intoMe[i] = page;

			filenode *t,dummy;
			dummy.file_desc = fd;
			t = (filenode *)avl_find(filenodeTree_fd, &dummy);
			t->pages_in_buff.push_back(temp.file_pos);


			temp1 -> file_desc = fd;
			temp1 -> file_pos = temp.file_pos;
			access_time++;
			temp1 -> timestamp = access_time;
			avl_insert(nodeTree_timestamp,temp1);
			avl_insert(nodeTree_fd_pos,temp1);
		}
	}
//cout<<"read end\n";
	return numPages;
}
int LRU :: Write(int fd, char **fromMe, int numPages){
//cout<<"write start\n";
//cout<<"numPages "<<numPages<<endl;
	for(int i = 0; i < numPages; i++){
		node temp;
		temp.file_desc = fd;
		temp.file_pos = lseek(fd, 0, SEEK_CUR) / pagesize;
		node *temp1;
		temp1 = (node*) avl_find (nodeTree_fd_pos, &temp);
		if(temp1 != NULL){
			bcopy(fromMe[i], temp1->mem, pagesize);
			avl_delete(nodeTree_timestamp,temp1);
			access_time++;
			temp1->timestamp = access_time;
			temp1->dirty = true;
			avl_insert(nodeTree_timestamp, temp1);
			lseek(fd, pagesize, SEEK_CUR);
		} else {
			char *page;
			NewPages(&page,1);
			temp1 = new node;
			temp1 -> mem = page;
			bcopy(fromMe[i], temp1->mem, pagesize);
			
			filenode *t,dummy;
			dummy.file_desc = fd;
			t = (filenode *)avl_find(filenodeTree_fd, &dummy);
			t-> pages_in_buff.push_back(temp.file_pos);

			temp1 -> file_desc = fd;
			temp1 -> file_pos = temp.file_pos;
			temp1 -> dirty = true;
			access_time++;
			temp1 -> timestamp = access_time;
			avl_insert(nodeTree_timestamp,temp1);
			avl_insert(nodeTree_fd_pos,temp1);
			lseek(fd, pagesize, SEEK_CUR);
		}
	}
//cout<<"write end\n";
	return numPages;
}

int LRU :: Seek(int fd, int pos){
//cout<<"seek start\n";
	lseek(fd, pos*pagesize, SEEK_SET);
//cout<<"seek end\n";
	return pos;
}

int LRU :: NewPages(char** intoMe, int numPages){
//cout<<"new page start\n";
	for(int i = 0; i < numPages; i++){
		if(free_pages.size() > 0){
			intoMe[i] = free_pages[free_pages.size() -1];
			free_pages.pop_back();

		} else{
			node *temp;
			node dummy;
			dummy.timestamp = 0;
			temp = (node*)avl_find_close(nodeTree_timestamp,&dummy);

			avl_delete(nodeTree_timestamp,temp);
			avl_delete(nodeTree_fd_pos,temp);
			
			filenode *t,fn;
			fn.file_desc = temp -> file_desc;
			t = (filenode*)avl_find(filenodeTree_fd, &fn);
			vector<int>::iterator pos;
			for(pos = t->pages_in_buff.begin(); pos <= t->pages_in_buff.end();pos++){
				if(*pos == temp -> file_pos){
					t -> pages_in_buff.erase(pos);
					break;
				}
			}
			
			if(temp -> dirty){
				int curloc = lseek(temp -> file_desc, 0, SEEK_CUR);
				lseek(temp->file_desc,temp->file_pos*pagesize,SEEK_SET);
				write(temp->file_desc, temp->mem, pagesize);
				lseek(temp -> file_desc,curloc,SEEK_SET);
			}
			intoMe[i] = temp -> mem;
			delete temp;
		}
	}

//cout<<"new page end\n";
	return numPages;
}

int LRU :: FreePages(char** fromMe, int numPages){
//cout<<"free page start\n";
	for(int i = 0; i < numPages; i++){
		free_pages.push_back(fromMe[i]);
	}
//cout<<"free page end\n";
//	return numPages;
	return 0;
}

int LRU :: Sync(int fd){
//cout<<"sync start\n";
	filenode  *temp;
	filenode dummy;
	dummy.file_desc = fd;

	temp = (filenode *) avl_find(filenodeTree_fd, &dummy);
	if(temp == NULL){
		cout << fd << " file does not exist\n";
		return -1;
	}
	int curloc = lseek(temp -> file_desc, 0, SEEK_CUR);
	while(temp -> pages_in_buff.size()  > 0){
		
		node *f,temp1;
		temp1.file_desc = fd;
		temp1.file_pos = temp->pages_in_buff[temp -> pages_in_buff.size() - 1];
		temp->pages_in_buff.pop_back();
		f = (node *)avl_find(nodeTree_fd_pos,&temp1); 
		avl_delete(nodeTree_fd_pos, f);
		avl_delete(nodeTree_timestamp, f);
		if(f->dirty){
			lseek(fd, f -> file_pos * pagesize,SEEK_SET);
			write(fd, f -> mem , pagesize);
		}	
		free_pages.push_back(f -> mem);		
		delete f;
	}
	lseek(temp -> file_desc,curloc,SEEK_SET);

//cout<<"sync end\n";
	return 0;
}

int LRU :: GetUnusedPageCount(){
//cout<<"get free page count start\n";
        return free_pages.size();
//cout<<"get free page count end\n";
}

int LRU::Pin(char *ptr){
//cout<<"pin start\n";
//cout<<"pin size "<<pin.size()<<endl;
	node *temp, dummy;
	dummy.timestamp = access_time;
	temp = (node*) avl_find(nodeTree_timestamp, &dummy);
	node temp1;
	temp1.timestamp = temp -> timestamp;
	temp1.file_desc = temp -> file_desc;
	temp1.mem = temp -> mem;

	pin.push_back(temp);

	avl_delete(nodeTree_timestamp,temp);
	avl_delete(nodeTree_fd_pos,temp);
//cout<<"pin end\n";
	return 1;
}
int LRU::UnPin(char *ptr){
//cout<<"unpin start\n";

	vector<node*>::iterator pos;
	for(pos = pin.begin(); pos <= pin.end();pos++){
		node *dummy = *pos;
		if(dummy->mem == ptr){
			avl_insert(nodeTree_timestamp,dummy);
			avl_insert(nodeTree_fd_pos,dummy);
			pin.erase(pos);
			break;
		}
	}
//cout<<"unpin end\n";
	return 1;
}
