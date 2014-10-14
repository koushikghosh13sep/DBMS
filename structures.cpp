#include<iostream>
#include"structures.h"

/* fd tree members */
Fd_tree_node::Fd_tree_node(){
	fd = INIT;
	buf_pages_in_use.clear();
	currPos = INIT;
}

Fd_tree_node::Fd_tree_node(int fd){
	this->fd = fd;
	buf_pages_in_use.clear();
	currPos = INIT;
}

Fd_tree_node::Fd_tree_node(int fd,off_t currPos){
	this->fd = fd;
	Fd_tree_node::currPos = currPos;
	buf_pages_in_use.clear();
}

void Fd_tree_node::add_page_in_use(int page_index){
	buf_pages_in_use.push_back(page_index);
}

void Fd_tree_node::remove_page_in_use(int page_index){
	for(int i=0;i<(int)buf_pages_in_use.size();i++)
		if(buf_pages_in_use[i] == page_index){
			buf_pages_in_use.erase(buf_pages_in_use.begin()+i);
			break;
		}
			
}

void Fd_tree_node::clear_pages_in_use(){
	buf_pages_in_use.clear();
}

off_t Fd_tree_node::get_currPos(){
	return currPos;
}

void Fd_tree_node::set_currPos(off_t pos){
	currPos = pos;
}

int Fd_tree_node::get_fd(){
	return fd;
}

void Fd_tree_node::set_fd(int fd){
	this->fd = fd;
}

vector<int> Fd_tree_node::get_copyOf_pages_in_use_vector()
{
	vector<int> ret;
	ret = Fd_tree_node::buf_pages_in_use;

	return ret;
}


/* fd pos tree members */
int Fd_pos_tree_node::get_fd(){
	return fd;
}

void Fd_pos_tree_node::set_fd(int fd){
	Fd_pos_tree_node::fd=fd;
}

off_t Fd_pos_tree_node::get_pos(){
	return pos;
}

void Fd_pos_tree_node::set_pos(off_t offset){
	pos = offset;
}

Fd_pos_tree_node::Fd_pos_tree_node(){
	fd = INIT;
	pos = INIT;
	index_to_page = INIT;
}

Fd_pos_tree_node::Fd_pos_tree_node(int fd,off_t offset,int index){
	pos = offset;
	Fd_pos_tree_node::fd = fd;
	index_to_page = index;
}

Fd_pos_tree_node::Fd_pos_tree_node(int fd,off_t offset){
	pos = offset;
	Fd_pos_tree_node::fd = fd;
	index_to_page = INIT;
}

int Fd_pos_tree_node::get_index_to_page()
{
	return index_to_page; 
}

void Fd_pos_tree_node::set_index_to_page(int index_to_page)
{
	Fd_pos_tree_node::index_to_page = index_to_page; 
}

/* Timestamp tree members */
Timestamp_tree_node::Timestamp_tree_node(){
	timestamp = INIT;
	page_index = INIT;
}

Timestamp_tree_node::Timestamp_tree_node(long timestamp,int index){
	Timestamp_tree_node::timestamp = timestamp;
	Timestamp_tree_node::page_index = index;
}

int Timestamp_tree_node::get_page_index(){
	return page_index;
}

void Timestamp_tree_node::set_page_index(int page_index){
	Timestamp_tree_node::page_index=page_index;
}

long Timestamp_tree_node::get_timestamp(){
	return timestamp;
}

void Timestamp_tree_node::set_timestamp(long timestamp){
	Timestamp_tree_node::timestamp=timestamp;
}

/* Buffer_pool_node member functions */
Buffer_pool_node::Buffer_pool_node(){
	page=NULL;
	dirt = BUFFER_POOL_PAGE_CLEAN;
}

bool Buffer_pool_node::isDirty(){
	return dirt;
}

void Buffer_pool_node::mark_dirty(){
	dirt = BUFFER_POOL_PAGE_DIRTY;
}

void Buffer_pool_node::mark_clean(){
	dirt=BUFFER_POOL_PAGE_CLEAN;
}

char* Buffer_pool_node::get_page_ptr(){
	return page;
}

void Buffer_pool_node::set_page_ptr(char *ptr){
	page=ptr;
}

void Buffer_pool_node::reset(){
	set_fd(INIT);
	set_pos(INIT);
	set_timestamp(INIT);

	mark_clean();
}

/*
* return -1 if first < second
* returns 1 if first > second,
* and 0 if first == second
*/
int Fd_tree_node::compare_fd(const void *first,const void *second,void *param){
	Fd_tree_node *one,*two;

	one = (Fd_tree_node *) first;
	two = (Fd_tree_node *) second;

	if(one->fd > two->fd)
		return 1;
	else if(one->fd < two->fd)
		return -1;
	else /* one->fd == two->fd */
		return 0;
}

int Fd_pos_tree_node::compare_fdpos(const void *first,const void *second
								,void *param)
{
	Fd_pos_tree_node *one,*two;

	one = (Fd_pos_tree_node *) first;
	two = (Fd_pos_tree_node *) second;

	if(one->get_fd() > two->get_fd())
		return 1;
	else if(one->get_fd() < two->get_fd())
		return -1;
	else	/* one->fd == two->fd */
		if(one->pos > two->pos)
			return 1;
		else if(one->pos < two->pos)
			return -1;
		else
		 	return 0;
}

int Timestamp_tree_node::compare_timestamp(const void *first
								,const void *second,void *param)
{
	Timestamp_tree_node *one,*two;

	one = (Timestamp_tree_node *) first;
	two = (Timestamp_tree_node *) second;

	if(one->timestamp > two->timestamp)
		return 1;
	else if(one->timestamp < two->timestamp)
		return -1;
	else
		return 0;
}
