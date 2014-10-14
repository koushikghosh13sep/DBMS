#ifndef _DBFILE_H
#define _DBFILE_H

#include <iostream>
#include "Phase2/Catalog.h"
#include "Phase2/Attribute.h"
#include "Phase2/Value.h"
#include "Phase2/Schema.h"
#include "Phase2/Record.h"
#include "LRU.h"
#include "avl.h"
#include "structures.h"
using namespace std;

extern Catalog dbCatalog;
extern class LRU myBuffer; // this is the buffer manager
/*
#ifndef _FILETYPE
#define _FILETYPE
enum FileType {
	Sorted,
	Heap,
	BTree
//Note, we will use Sorted and BTree types in the next assignment.
};
#endif
*/
enum FileStatus{
	CREATE,
	OPEN,
	CLOSED
};		

class DBFile{
	
//	private:
	public:		
		char *fileName;
		int fd;
		char *relName;
		Schema fileSchema;
		int filePages;
		int currentFilePos;
		int currentRecord;
		int totalRecord;
		int pageSize;
		FileType fileType;
		int *numDistincts;
		char *key;		//not sure
		FileStatus fStatus;
		int recSize;
		int numTuples;
	
	public:
		friend class Catalog;
		int Create (char *fName, Schema &schema, int pagesToAllocate, FileType fType);
		int Open (char *fName, Schema &schema, int fileSize,int firstPos, FileType fType,int *numDistincts, int numTuples);
		int Find (Value &key);
		int BinarySearchFind(Value &key);
		int GetNext(Record &fillMe);
		void Reset ();
		void Insert (Record &insertMe);
		void GetInfo(char * &fName, Schema &schema,int &fileSize, int &firstPos,FileType &myType, int *&numDistincts,int &numTuples);
		int Convert (FileType toWhat, Attribute &newKey);
		int Convert (FileType toWhat, DBFile &intoMe, char *newFName,Attribute &newKey);
		void Close ();
//		static int Compare(const void* a, const void* b);
		int HeapToSorted(Attribute &newKey);
		void merge(int, int, int, int, DBFile*);
//		void Copy(DBFile*,char*);
		void CopyFile(int sfd,int dfd);
		void CopyAttributes(DBFile);
		void NextPage(int, int, char**);
		DBFile ();
		~DBFile ();
};

#endif
