#include<iostream>
#include<cstring>
#include<cstdlib>
#include<cstdio>
#include<fstream>
#include<queue>
#include"dbFile.h"
#include "RecordInfo.h"

using namespace std;

//LRU myBuffer;
char catname[] = "Phase2/catalog";
//Catalog dbCatalog(catname);

Schema globalSchema;
int grs;

Record small(globalSchema);

DBFile ::DBFile(){
}

void DBFile :: CopyAttributes(DBFile oldFile){
	strcpy(relName,oldFile.relName);
	filePages = oldFile.filePages;
	pageSize = oldFile.pageSize;
	FileType fileType;
	strcpy(key,oldFile.key);		//not sure
	recSize = oldFile.recSize;
	numTuples = oldFile.numTuples;
}

struct compare {
	bool operator()(RecordInfo* a, RecordInfo* b) const {
		Record a1 = Record(globalSchema);
		Record b1 = Record(globalSchema);
		a1.FromBinary(a->rec);
		b1.FromBinary(b->rec);
		a1.SetKey(globalSchema.GetKey());
		b1.SetKey(globalSchema.GetKey());
		int retVal =  a1.Compare(b1);
		return (retVal < 0 ) ? false : true;
	}
};

int Compare(const void *a, const void *b){
	Record a1 = Record(globalSchema);
	Record b1 = Record(globalSchema);
	a1.FromBinary(*(char**) a);
	b1.FromBinary(*(char**)b);
	a1.SetKey(globalSchema.GetKey());
	b1.SetKey(globalSchema.GetKey());
	return a1.Compare(b1);

}
int DBFile :: Create(char *fName,Schema &schema,int pagesToAllocate, FileType fType){
	cout<<"creat start\n";	
	fileName =  new char[strlen(fName)];
	fileName = strdup(fName);
	filePages = pagesToAllocate;
	fileType = fType;

	relName = new char[strlen(schema.GetName())];
	relName = strdup(schema.GetName());
	
	fileSchema = Schema(relName);
	fileSchema.Copy(schema);
	globalSchema = Schema(relName);
	globalSchema.Copy(schema);
	pageSize = dbCatalog.GetPageSize();
	
	currentFilePos = 0;
	currentRecord  = 0;
	totalRecord = 0;
	
	numDistincts = new int[schema.NumAtts()];
	key = new char[strlen(schema.GetKey())];
	key = strdup(schema.GetKey());

	fStatus = CREATE;
	recSize = schema.GetRecordSize();
	fd = myBuffer.Open(fileName,filePages);
	if(fd == -1 )
		return -1;

	cout<<"create end\n";
	return 1;

}

int DBFile :: Open (char *fName, Schema &schema, int fiSize,int firstPos, FileType fType,int *numDistincts, int numTup){
//	cout<<"open start\n";

	fd = myBuffer.Open(fName,0);
	if(fd == -1)
		return 0;

	relName = new char[strlen(schema.GetName())];
	relName = strdup(schema.GetName());
	
	fileSchema = Schema(relName);
	fileSchema.Copy(schema);
	
	key = new char[strlen(schema.GetKey())];
	key = strdup(schema.GetKey());

	filePages = fiSize;

	currentFilePos = firstPos;
	pageSize = dbCatalog.GetPageSize();
	fileType = fType;
	recSize = schema.GetRecordSize();
	
	numTuples = numTup;
	
	this -> numDistincts = new int[schema.NumAtts()];
	for(int i = 0; i < schema.NumAtts(); i++)
		this -> numDistincts[i] = numDistincts[i];
	
//	cout<<"open end\n";
	return 1;
}

int DBFile :: Find (Value &key){
//cout<<"find start\n";
	Record temp = Record(fileSchema);
	int p = 0;
	int pos = 0;
	int rec_size;
	int temp_pos = 0;
	int file_size = filePages * pageSize;
	int greater = 0;
	int page_count = 0;
	int mode = 0;
	int recPerPage = pageSize/recSize;
	
	if(fileType == Sorted){
	//	cout<<"in if Sorted filetype "<<fileType<<endl;
		if(strcmp(key.GetName(),fileSchema.GetKey()) == 0)
			return BinarySearchFind(key);
	}		
//	cout<<"IN Heap Find:"<<endl;
	rec_size = temp.GetNumBytes();
	while(p < totalRecord ){
		char **intoMe = new char*[1];
		myBuffer.Seek(fd,page_count);

		myBuffer.Read(fd,intoMe,1);
		pos = 0;
		do{
			p++;
			temp.FromBinary(intoMe[0] + pos);
			char *key_name = key.GetName();
			int key_index = temp.GetIndex(key_name);
			if(key.GetType() == temp[key_index].GetType()){
				if(temp[key_index].Compare(key) == 0 ){
					
					currentFilePos = page_count * pageSize + pos; 
					return 1;
				}
				else if((temp[key_index].Compare(key) == 1) && (mode ==0) ){
						greater = page_count * pageSize + pos;
						mode =1;
				}
			}	
			
			pos += rec_size;
			if(p == totalRecord)
				break;
		}while((pos + rec_size) <= pageSize);

		page_count++;
	}
	if(mode == 1){
		currentFilePos = greater;
		currentRecord = recPerPage*(currentFilePos/pageSize) + (currentFilePos % pageSize)/recSize;
		return 0;
	}
	else{
		currentFilePos = (page_count - 1)* pageSize + pos + recSize;
		currentRecord = totalRecord;
		return -1;
	}	

}
int DBFile :: BinarySearchFind(Value &key){
//cout<<"binary search\n";
	Record temp = Record(fileSchema);
	char **intoMe = new char*[1];
	int min = 0;
	int max = totalRecord - 1;//numTuples-1;
	int mid;
	int recPerPage = pageSize / recSize;
	int mode = 0;
	int greater ,smaller;
	int pageNum=0;
	int pos =0;
	int found = 0;
	int tempPos = 0;
	do{
		mid = (min + max ) / 2;
		pageNum = mid  / recPerPage;

		pos = (mid % recPerPage)*recSize;
		myBuffer.Seek(fd,pageNum);
		myBuffer.Read(fd,intoMe,1);
		temp.FromBinary(intoMe[0] + pos);

		char *key_name = key.GetName();
		int key_index = temp.GetIndex(key_name);
		
		if(key.GetType() == temp[key_index].GetType()){
			if(temp[key_index].Compare(key) == 0 ){
				currentFilePos = pageNum * pageSize + pos;
				found = 1;
				max = mid - 1;
				tempPos = currentFilePos;
			}
			else if((temp[key_index].Compare(key) >= 1) ){
				max = mid - 1;
				if(mode == 0){
					greater = pageNum * pageSize + pos;
					mode = 1;
				}
			}
			else {
				min = mid + 1;
			}
		}

	}while(max >= min);
	if (found == 1){
		currentFilePos = tempPos;
		currentRecord = recPerPage*(currentFilePos/pageSize) + (currentFilePos % pageSize)/recSize;
		return 1;
	}
	if(mode ==  1){
		currentFilePos = greater;
		currentRecord = recPerPage*(currentFilePos/pageSize) + (currentFilePos % pageSize)/recSize;
		return 0;
	}
	else{
		currentFilePos = filePages * pageSize + recPerPage* recSize;//(pageNum - 1)* pageSize + pos;
		currentRecord = totalRecord + 1 ; 
		return -1;
	}
}

int DBFile :: GetNext(Record &fillMe){
	
	char **intoMe = new char*[1];
	int page_num = currentFilePos/pageSize;
	int pos = currentFilePos % pageSize;
//	cout<<"Current file pos "<<currentFilePos<<endl; 		
	if((currentFilePos + recSize) > (pageSize*(page_num + 1))){
		currentFilePos = pageSize*(page_num +1);// - currentFilePos;
		page_num++;
		pos = 0;
	}
	myBuffer.Seek(fd,page_num);
	
	int c =	myBuffer.Read(fd,intoMe,1); 
//	cout<<"current record"<<currentRecord<<endl;
	if(c == -1){
		cout<<"ERROR in Read"<<endl;
		return -1;
	}
	if(totalRecord <= currentRecord){
		return 0;
	}
	fillMe.FromBinary(intoMe[0]+pos);
	currentFilePos += recSize;
	currentRecord++;	
	return 1;
}	

void DBFile :: Reset (){
	currentFilePos = 0;
	currentRecord = 0;
}

void DBFile :: Insert (Record &insertMe){
//	cout<<"insert start\n";
	if(fileType == Sorted)
		fileType = Heap;
	int pageNum;
	int pos;
	char *binRec;
	int recPerPage = pageSize / recSize;
	
	pageNum = totalRecord  / recPerPage;
	if(pageNum >= filePages)
		filePages++;
	pos = (totalRecord % recPerPage)*recSize;
	
	char **intoMe = new char*[1];
	myBuffer.Seek(fd, pageNum);
	myBuffer.Read(fd,intoMe, 1);

	insertMe.ToBinary(intoMe[0] + pos);
	
	myBuffer.Seek(fd, pageNum);
	myBuffer.Write(fd,intoMe,1);
	
	totalRecord = totalRecord + 1;
	numTuples = totalRecord;

	currentFilePos += recSize;

}
void DBFile :: GetInfo(char * &fName, Schema &schema,int &fiSize,int &firstPos,FileType &myType,int *&numDistincts,int &numTuples){
	
	fName = new char[strlen(fileName)];
	fName = strdup(fileName);

	fiSize = filePages;
	schema.Copy(fileSchema);

	firstPos = 0;
	myType = fileType;

	numTuples = this -> numTuples;

	numDistincts = new int[fileSchema.NumAtts()];
	for(int i = 0; i < fileSchema.NumAtts(); i++)
		 numDistincts[i] = this->numDistincts[i];

}
void DBFile :: Close (){
	myBuffer.Close(fd);
}
int DBFile :: Convert (FileType toWhat, Attribute &newKey){
//	cout<<" file name "<<fileName<< " file type "<<fileType<<endl<<" towhat "<<toWhat<<endl;
	if(fileType == Heap && toWhat == Heap){
		fileType = toWhat;
		fileSchema.SetKey(newKey.GetName());
		globalSchema.Copy(fileSchema);

		return 0;
	}
	if(fileType == Heap && toWhat == Sorted){
		fileType = toWhat;
		fileSchema.SetKey(newKey.GetName());
		globalSchema.Copy(fileSchema);
		return HeapToSorted(newKey);
	}
	
	if(fileType == Sorted && toWhat == Sorted){
		fileType = toWhat;
		fileSchema.SetKey(newKey.GetName());
		globalSchema.Copy(fileSchema);
		return HeapToSorted(newKey);
	}

	if(fileType == Sorted && toWhat == Heap){
		fileType = toWhat;
		fileSchema.SetKey(newKey.GetName());
		globalSchema.Copy(fileSchema);
		fileType = Heap;
		return 0;
	}
}

int DBFile :: Convert(FileType toWhat, DBFile &intoMe,char *newFName,Attribute &newKey){
	
	intoMe.Create(newFName,fileSchema,filePages,fileType);
	intoMe.CopyAttributes(*this);
	intoMe.totalRecord = this->totalRecord;
	intoMe.CopyFile(fd,intoMe.fd);
	return intoMe.Convert(toWhat,newKey);
}


int DBFile :: HeapToSorted(Attribute &newKey){
//cout<<" in heap to sorted\n";	
	char fn[] = "/var/tmp/TEMPFILE";
	DBFile *tempfile=new DBFile();
	tempfile->Create(fn,fileSchema,1,Sorted);
cout<<"totalrecord "<<totalRecord<<endl;

	char **intoMe;
	int recsPerPage = pageSize / recSize;
	int freePages = dbCatalog.GetNumBufPages();//pages in buffer
	int frPg = freePages - 1;//one is left for output purposes
	intoMe = new char*[frPg];
	//calculates the no of pages in which there are records . keeping into mind that some pages may be blank
	//at the end
	int pages = totalRecord/recsPerPage;
	
//	cout<<"frpg "<<frPg<<endl;
//	cout<<"PAGES = "<<pages<<" toatal record "<<totalRecord<<" recs per page  "<<recsPerPage<<endl;
	if((totalRecord % recsPerPage)!=0)
		pages++;
	
	char str[] = "NewPage!";
	int num = strlen(str);  
	int records = 0;//no of records read
	int recpSize=0;//recordpointers size
	int pass = -1;

	myBuffer.Seek(fd,0);
	myBuffer.Seek(tempfile -> fd,0);
	
	for(int pageNum = 0; pageNum < pages; pageNum += frPg){
//		cout<<"pageNum "<<pageNum<<" pages "<<pages <<endl;
		pass++;

		if((pages - pageNum) < frPg)
			frPg = pages - pageNum;

		myBuffer.Read(fd,intoMe,frPg);
		if((totalRecord - records) < recsPerPage*frPg)//no of records left
			recpSize = totalRecord	- records;
		else
			recpSize = recsPerPage * frPg;

		char **recordPointers = new char*[recpSize];

		for(int i = 0; i < recpSize ; i++){
			// (i/recsPerPage) denotes the page in which the record should be inserted"
			// (i*recsPerPage) denotes the position of the record within the page 
			recordPointers[i] = &intoMe[i / recsPerPage][recSize * (i % recsPerPage)];
		}
		cout<<"qsort\n";
//		cout<<"recpsize "<<recpSize<<endl;
		
		qsort(recordPointers,recpSize,sizeof(char*),Compare);
//		cout<<"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n";


		char **fromMe = new char*[frPg];

		for( int j = 0; j < frPg; j++)
			fromMe[j] = new char[pageSize];
		for(int j = 0; j < recpSize; j++)
			bcopy(recordPointers[j],fromMe[j/recsPerPage]+(j%recsPerPage)*recSize,recSize);


		myBuffer.Write(tempfile -> fd,fromMe,frPg);
		
//		cout<<"write \n";
		for(int j = 0; j <frPg; j++)
			delete [] fromMe[j];
		delete []fromMe;

		records += recpSize;
		delete[] recordPointers;
//		cout<<"pageNum "<<pageNum<<" pages "<<pages <<endl;
	}
	
//	tempfile->Sync();
	CopyFile(tempfile->fd ,fd);
//	remove(fn);
	myBuffer.Seek(tempfile->fd,0);
	myBuffer.Seek(fd,0);

	if(pass == 0){
		cout<<"\n pass"<< pass <<endl;
		return 1;
	}
	
	myBuffer.Seek(tempfile->fd,0);
	myBuffer.Seek(fd,0);
	
	frPg = freePages - 1;
	int bufsize = frPg;
	int tpage = filePages;
	int x = tpage % bufsize;
	tpage = filePages / bufsize;
	int lastrun = tpage % bufsize;
	if( x != 0)
		tpage++;

	int blocksize = frPg;
	int low,high;
	int block = bufsize;
	int passcount=1;

	while(tpage > frPg){
	cout<<"ffffffffffffffffffffffffffffffffffffffffffffff\n";
		blocksize = blocksize * frPg;
		x = tpage % frPg;
		tpage = tpage / frPg;
		if(x != 0)
			tpage++;
		cout<<"pass "<< pass <<endl;
		lastrun = filePages % blocksize;

		pass++;

		low = 0;
		high = blocksize;
		for(int i = 0; i < tpage; i++){
			if( i == tpage-1)
				high = filePages;
			cout<<"low "<<low<<" high "<<high<<endl;
//	myBuffer.Seek(tempfile->fd,0);
//	myBuffer.Seek(fd,0);
			merge(low, high, block, frPg,tempfile);
			low += blocksize;
			high += blocksize;
		}
		block = blocksize;
		CopyFile(tempfile->fd, fd);
	}
	
	

//	tempfile->Close();
	
	myBuffer.Seek(tempfile->fd,0);
	myBuffer.Seek(fd,0);
	
	low = 0, high = filePages;
	merge(low, high, block, frPg,tempfile);
	CopyFile(tempfile->fd, fd);
	tempfile->Close();

}

void DBFile :: merge(int start,int blockpage, int block, int frpg, DBFile *tempfile){
cout<<"\t\t\t\tin merge \n\n";	
	cout << "start "<<start<<" block " <<block << " blockpage "<< blockpage<<endl;
cout<<"filepage "<<filePages<<endl;
	int x = blockpage % block;
	int noofblock = (blockpage-start) /block;

	if(x!=0)
		noofblock++;
	
	int recordno[noofblock];
	int posarr[noofblock];
	int blockpagecount[noofblock];
	
	char **bufPointer = new char*[noofblock];

	int lastpagerecord;
	int lastblock=0;

	int y = block;

	int recperpage = pageSize / recSize;
//	cout<<"recperpage: "<<recperpage<<endl;
	
	for(int i = 0; i < noofblock; i++){
		if((y+start) < blockpage)
			blockpagecount[i] = y+start;
		else{
			lastblock = 1;
			blockpagecount[i] = blockpage;
		}
		y += block;
	}
	lastpagerecord = 0;
	if(blockpage == filePages)
		lastpagerecord = totalRecord % recperpage;
//	if(lastpagerecord == 0){
//		lastpagerecord = recperpage;
//	}
cout<<"noofblock "<<noofblock<<endl;
cout<<"recperpage "<<recperpage<<endl;
cout<<"lastblock : "<<lastblock<<endl;
	int position = start;
	int temppos = start;
	myBuffer.Seek(tempfile->fd,temppos);

	int s = block;
	priority_queue < RecordInfo*, vector < RecordInfo*>, compare> pq;
	
	char **fromMe = new char*[1];
	fromMe[0] = new char[pageSize];
	

	for(int i = 0; i< noofblock; i++){
//		cout<<"position "<<position<<" i " <<i<<endl;
		myBuffer.Seek(fd,position);
		myBuffer.Read(fd, &bufPointer[i], 1);
		myBuffer.Pin(bufPointer[i]);
		
		RecordInfo *ri = new RecordInfo;
		ri -> rec = new char[recSize];
		bcopy(bufPointer[i] + 0, ri->rec, recSize);
		ri -> position = i;
		pq.push(ri);
//		Record R = Record(globalSchema);
//		R.FromBinary(ri -> rec);
//		char *key = R.GetKey();
//		cout<<R[R.GetIndex(key)]<<endl;
		recordno[i] = 0;
		posarr[i] = position;
		position += block;

	}
	int temparrpos = 0;
	int tempcount = start;
	while(pq.size()){
//		cout<<" size " << pq.size()<<endl;
		int flag = 1;
		RecordInfo *r = pq.top();
		int index = r-> position;
		pq.pop();
	
		bcopy(r->rec,fromMe[0]+temparrpos*recSize, recSize);
		temparrpos++;

		if(temparrpos == recperpage){
			temparrpos = 0;
			myBuffer.Seek(tempfile->fd,tempcount);
			myBuffer.Write(tempfile -> fd,fromMe,1);
			tempcount++;
		} 
		
		delete r->rec;
		delete r;
		
		recordno[index]++;
		
		
		if(lastpagerecord != 0 && index == noofblock - 1 && posarr[index]  == blockpagecount[index]-1){
//cout<<"tttttttttttttttttttttttttttttttttt\n";
			if(recordno[index] == lastpagerecord){
				cout<<"QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ\n\n";
				posarr[index] = -1;
			}
		}else if(recordno[index] == recperpage){
			posarr[index]++;
			if(posarr[index] == blockpagecount[index]){
				cout<<"qqqqqqqqqqqqqqqqqqqqqqqqqqqqqq\n\n";
				posarr[index] = -1;
			}else{
//				cout<<"\t\tposarr [ "<<index<<" ] "<< posarr[index]<<endl;
				myBuffer.Seek(fd,posarr[index]);
//				myBuffer.Seek(fd,start + s*index + posarr[index]);
				myBuffer.UnPin(bufPointer[index]);
				myBuffer.Read(fd, &bufPointer[index],1);
				myBuffer.Pin(bufPointer[index]);
				recordno[index] = 0;
			}
		}
		if(posarr[index] != -1){
			RecordInfo *ri = new RecordInfo;
			ri -> rec = new char[recSize];
			bcopy(bufPointer[index] + recordno[index]*recSize, ri -> rec, recSize);
			ri -> position = index;
			pq.push(ri);
		}
		
	}
//	cout<<"end while\n";
	if(lastpagerecord != 0)
		myBuffer.Write(tempfile->fd,fromMe,1);
	
	for(int i =0 ; i< noofblock; i++)
		myBuffer.UnPin(bufPointer[i]);
	
	myBuffer.Sync(fd); 
	myBuffer.Sync(tempfile->fd);
	
	delete fromMe[0];
	delete fromMe;

	delete bufPointer;

	cout<<"\n\n";
}

void DBFile::CopyFile(int readfd, int writefd){
	cout<<"in copy file \n";
	
	myBuffer.Sync(readfd); //sync sets filepos to 0
	myBuffer.Sync(writefd);
	myBuffer.Seek(readfd,0);
	myBuffer.Seek(writefd,0);

	int freepagecount = myBuffer.GetUnusedPageCount();
	int numpages = 0, numpagescopied=0;
	numpages = (filePages <= freepagecount) ? filePages : freepagecount;


	char **intome = new char*[numpages];
	cout<<"numpages "<<numpages<<" filePages "<<filePages<<endl;
//	cout<<"numpages "<<numpages<<" pagesize "<<pageSize<<endl;

	for(int numpagescopied=0; numpagescopied<filePages; numpagescopied+=numpages) {
		if(filePages-numpagescopied<numpages) {
			numpages = filePages-numpagescopied;
		}
		myBuffer.Read(readfd, intome, numpages);
		myBuffer.Write(writefd, intome, numpages);
	}
}
DBFile::~DBFile(){}
