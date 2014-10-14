#include "dbFile.h"
#include "stdlib.h"
#include <iostream>
#include <string.h>
#include <time.h>
//#include "LRU.h"
#define NUMDIST 100
#define NUMALLOC 10
using namespace std;
LRU myBuffer;
Catalog dbCatalog ("cat1");

int randomizers[10];
int howManyRecs;
int *keys;
char **strings = new char*[607];

int GetLine (char *inMe, FILE *fromMe);

struct two {
	int first;
	double second;
};

int intcompar (const void *a, const void *b) {

	int *aa = (int *) a;
	int *bb = (int *) b;
	if (*aa < *bb)
		return -1;
	else if (*aa > *bb)
		return 1;
	else
		return 0;
}

int strcompar(const void* a, const void* b) {
	
	char **aa = (char **) a;
	char **bb = (char **) b;
	int min = strlen(*aa);
	
	//cerr<<"inside strcompr: "<<*aa<<endl;
	
	if (strlen(*bb) < min)
		min = strlen(*bb);
	return strncmp ( *aa, *bb, min); 
	
}

//********************************************
void Test1 (int numRecs, char* catName, char* fPath) { 
		
	howManyRecs = numRecs;
	keys = new int[howManyRecs];
	srand48(2376234823);
	for (int i = 0; i < howManyRecs; i++) {
		keys[i] = lrand48 ();
	}
	
	// get the scehma for our file (I'll just ignore the file type
	// and the file information)
	int fSize, startPos, numTuples;
        FileType type;
        Schema mySchema ("");
        int numDistincts[NUMDIST];
       	int *distinctString = numDistincts;
        dbCatalog.GetFileInfo (catName, mySchema, type, distinctString, numTuples, fSize, startPos);

	// create a database 
	cerr<<"Creating file\n";
	DBFile myFile;
	myFile.Create (fPath, mySchema, NUMALLOC, Heap);

	// add all of the data into it
	cerr<<"Inserting records\n";
	Record myData (mySchema);
	for (int i = 0; i < howManyRecs; i++) {
		// build up the record
		myData[0].InsertVal (&(keys[i]));
		for (int j = 0; j < 10; j++) {
			myData[j + 1].InsertVal(strings[(keys[i] % randomizers[j]) % 607]);
		}

		// put him into the file
		myFile.Insert (myData);
		if (i % 5000 == 0) {
			cerr << i << " ";
		}
	}		
	cerr<<endl;

	// sort the file

	cerr<<"Now converting heap file\n";
	myFile.Convert(Sorted, mySchema.GetAtts(mySchema.GetKey()));
	cout << "Done sorting. Checking the sorted file for correctness.\n";
		
	// sort our own copy of the keys
	qsort (keys, howManyRecs, sizeof (int), intcompar);

	// and see if the answer is correct;  first, find the first record
	myData[0].InsertVal (&keys[0]);
	cout<<keys[0]<<endl;
	cout<<myData[0]<<endl;
	cout<<"find resultttttttttttttttttttttttttttttttttttt   "<<myFile.Find (myData[0]);
		
	// now, scan the file and see if we were OK
	for (int i = 0; i < howManyRecs; i++) 
	{
		// build up the reocrd that we _schould_ have
		myData[0].InsertVal (&(keys[i]));
		for (int j = 0; j < 10; j++) {
			myData[j + 1].InsertVal (strings[(keys[i] % randomizers[j]) % 607]);
		}
		// get the record from the file
		Record fromFile (mySchema);
		myFile.GetNext (fromFile);
			
		// and compare all of the attributes
		int OK = 1;
		for (int j = 0; j < 11; j++) {
			if (fromFile[j].Compare (myData[j])){
				OK = 0;
			}
		}

		if (numRecs > 10000 && i%10000 == 0)
			cerr<<i<<"*";

		// let 'em know if there was an error
		if (!OK) {
			cerr << "Error in record #" << i;
			cerr << " Found:";
			cerr << fromFile;
			cerr << " Expected:";
			cerr << myData << "\n";
		}

	}
	cerr <<endl<< "File check complete...Now converting to an heap " << endl;
	
	//now convert it to a heap..shudn't seg fault
	myFile.Convert (Heap, mySchema.GetAtts(mySchema.GetKey()));
	
	//check if GetInfo returns the right information
	int file_size, first_pos;
	FileType file_type;
	char *foo;
	myFile.GetInfo(foo, mySchema, file_size, first_pos, file_type, distinctString, numTuples);
	cerr << "File size: " << file_size << " First pos: " << first_pos <<endl;
	cerr << "Num Tuples: " << numTuples << " File Type: " << file_type << endl;

	myFile.Close();	

  	delete [] keys;
	delete [] distinctString;
	delete foo;
}

//********************************************
void Test2 (int numRecs, char* catName, char* fPath) { 
		
	howManyRecs = numRecs;
	keys = new int[howManyRecs];
	srand48(362346877);
	for (int i = 0; i < howManyRecs; i++) {
		keys[i] = lrand48 ();
	}
	
	// get the scehma for our file (I'll just ignore the file type
	// and the file information)
	int fSize, startPos, numTuples;
    FileType type;
    Schema mySchema ("");
	int numDistincts[NUMDIST];
    int *distinctString = numDistincts;
    dbCatalog.GetFileInfo (catName, mySchema, type, distinctString, numTuples, fSize, startPos);
  	
	// create a database file
	cerr<<"Creating file\n";
	DBFile myFile;
	myFile.Create (fPath, mySchema, NUMALLOC, Heap);

	// add all of the data into it
	cerr<<"Inserting records\n";
	Record myData (mySchema);
	for (int i = 0; i < howManyRecs; i++) 
	{
		// build up the record
		myData[0].InsertVal (&(keys[i]));
		for (int j = 0; j < 10; j++) {
			myData[j + 1].InsertVal (strings[(keys[i] % randomizers[j]) % 607]);
		}

		// put him into the file
		myFile.Insert (myData);
		if (i % 10000 == 0) {
			cerr << i << " ";
		}
	}		
	
	cerr<<endl;
	
	// sort the file
	cerr<<"Now sorting the file\n";
	myFile.Convert (Sorted, mySchema.GetAtts(mySchema.GetKey()));
	cout << "Done sorting it.  Checking the sorted file for correctness.\n";
		
	// sort our own copy of the keys
	qsort (keys, howManyRecs, sizeof (int), intcompar);

	//reset pointer to the start of the file
	myFile.Reset();
	
	//find some existent and some non-existent records
	cerr << "Checking correctness for some existing and some non-existing " << endl;
	int ret;
	for(int i = 0;i < howManyRecs; i++) {
		if(i % 3 == 0){
			//find an existent record
			myData[0].InsertVal(&(keys[i]));

			ret = myFile.Find(myData[0]); 
			if (ret != 1){
				cout<<myData[0];
				cerr << "ERROR: Couldn't find an existent record !" << endl;
			}
			if(numRecs < 5000){ 
				cerr<<ret<<"*";	
			}else {
				if(i%999 == 0) cerr<<i<<"+";
			}	
		}
		else{ //find a non-existent record
			myData[0].InsertVal(&i);
			myFile.Find(myData[0]);
			if(numRecs < 5000){ 
				cerr<<ret<<"*";	
			}else{
				if(i%500 == 0) cerr<<i<<"+";
			}
		}
	}
	cerr<<endl;
	
	//check if GetInfo returns the right information
	int file_size, first_pos;
	FileType file_type;
	char *foo;
	myFile.GetInfo(foo, mySchema, file_size, first_pos, file_type, distinctString, numTuples);
	cerr << "File size: " << file_size << " First pos: " << first_pos <<endl;
	cerr << "Num Tuples: " << numTuples << " File Type: " << file_type << endl;
	myFile.Close();	

  	delete [] keys;
	delete [] distinctString;
	delete foo;
}

//********************************************
void Test3 (int numRecs, char* catName, char* fPath, char* sortedFName) { 
		
	howManyRecs = numRecs;
	keys = new int[howManyRecs];
	srand48(362346877);
	for (int i = 0; i < howManyRecs; i++) {
		keys[i] = lrand48 ();
	}
	
	// get the scehma for our file (I'll just ignore the file type
	// and the file information)
	int fSize, startPos, numTuples;
    FileType type;
    Schema mySchema ("");
	int numDistincts[NUMDIST];
    int *distinctString = numDistincts;
    dbCatalog.GetFileInfo (catName, mySchema, type, distinctString, numTuples, fSize, startPos);
		
	// create a database file
	cerr<<"Creating file\n";
	DBFile myFile;
	myFile.Create (fPath, mySchema, NUMALLOC, Heap);

	// add all of the data into it
	cerr<<"Inserting records\n";
	Record myData (mySchema);
	for (int i = 0; i < howManyRecs; i++) {
		// build up the record
		myData[0].InsertVal (&(keys[i]));
		for (int j = 0; j < 10; j++) {
			myData[j+1].InsertVal (strings[(keys[i] % randomizers[j]) % 607]);
		}

		// put him into the file
		myFile.Insert (myData);
		if (i % 10000 == 0) {
			cerr << i << " ";
		}
	}		
	cerr<<endl;

	// sort the file
	DBFile dummy;
	cerr<<"Now sorting the file\n";
	myFile.Convert (Sorted, dummy, sortedFName, mySchema.GetAtts(mySchema.GetKey()));
	cout << "Done sorting it.  Checking the sorted file for correctness.\n";
		
	// sort our own copy of the keys
	qsort (keys, howManyRecs, sizeof (int), intcompar);

	// and see if the answer is correct;  first, find the first record
	myData[0].InsertVal (&keys[0]);
	dummy.Find (myData[0]);
		
	// now, scan the file and see if we were OK
	for (int i = 0; i < howManyRecs; i++) 
	{
		// build up the reocrd that we _schould_ have
		myData[0].InsertVal (&(keys[i]));
		for (int j = 0; j < 10; j++) {
			myData[j+1].InsertVal (strings[(keys[i] % randomizers[j]) % 607]);
		}
	
		// get the record from the file
		Record fromFile (mySchema);
		dummy.GetNext (fromFile);
		
		// and compare all of the attributes
		int OK = 1;
		for (int j = 0; j < 11; j++) {
			if (fromFile[j].Compare (myData[j])) {
				OK = 0;
			}
		}
	
		if (numRecs > 10000 && i%10000 == 0)
		cerr<<i<<"*";

		// let 'em know if there was an error
		if (!OK) {
			cerr << "Error in record #" << i;
			cerr << " Found:";
			cerr << fromFile;
			cerr << " Expected:";
			cerr << myData << "\n";
		}

	}

	cerr<<endl;
	//check if GetInfo returns the right information
	int file_size, first_pos;
	FileType file_type;
	char *foo;
	dummy.GetInfo(foo, mySchema, file_size, first_pos, file_type, distinctString, numTuples);
	cerr << "File size: " << file_size << " First pos: " << first_pos <<endl;
	cerr << "Num Tuples: " << numTuples << " File Type: " << file_type << endl;
	dummy.Close();	

  	delete [] keys;
	delete [] distinctString;
	delete foo;
			
}

//********************************************
void Test4 (int numRecs, char* catName, char* fPath, char* sortedFName) {
		
	howManyRecs = numRecs;
	keys = new int[howManyRecs];
	srand48(362346877);
	for (int i = 0; i < howManyRecs; i++) {
		keys[i] = lrand48 ();
	}
	
	// get the scehma for our file (I'll just ignore the file type
	// and the file information)
	int fSize, startPos, numTuples;
    FileType type;
    Schema mySchema ("");
	int numDistincts[NUMDIST];
    int *distinctString = numDistincts;
    dbCatalog.GetFileInfo (catName, mySchema, type, distinctString, numTuples, fSize, startPos);
		
	// create a database file
	cerr<<"Creating file\n";
	DBFile myFile;
	myFile.Create (fPath, mySchema, NUMALLOC, Heap);

	// add all of the data into it
	cerr<<"Inserting records\n";
	Record myData (mySchema);
	for (int i = 0; i < howManyRecs; i++) {
		// build up the record
		myData[0].InsertVal (&(keys[i]));
		for (int j = 0; j < 10; j++) {
				myData[j+1].InsertVal (strings[(keys[i] % randomizers[j]) % 607]);
		}

		// put him into the file
		myFile.Insert (myData);
		if (i % 10000 == 0) {
			cerr << i << " ";
		}

	}		
	cerr<<endl;

	// sort the file
	DBFile dummy;
	cerr<<"Now sorting the file\n";
	myFile.Convert (Sorted, dummy, sortedFName, mySchema.GetAtts(mySchema.GetKey()));
	cout << "Done sorting it.  Checking the sorted file for correctness.\n";
		
	// and see if the answer is correct;  first, find the first record
	myData[0].InsertVal (&keys[0]);
	myFile.Find(myData[0]);

	// sort our own copy of the keys
	qsort (keys, howManyRecs, sizeof (int), intcompar);
	myData[0].InsertVal (&keys[0]);
	dummy.Find(myData[0]);

	cerr << "Matching up the records from the original and the sorted files " << endl;
			
	int retval1=0, retval2=0;
	for (int i = 0; i < howManyRecs; i++) {
	
		myData[5].InsertVal(strings[(keys[i] % randomizers[4]) % 607]); 
		retval1 = myFile.Find(myData[5]);
		retval2 = dummy.Find(myData[5]);
//		cout<<"retval1="<<retval1<<"  retval2="<<retval2<<endl;
		if(retval1 != 1 || retval2 != 1)
			cerr << " Error  matching the records in the two files !" << endl;
		
		if (i % 5000 == 0) {
			cerr << i << "+";
		}
	}
	
	cerr<<endl;
	//check if GetInfo returns the right information
	int file_size, first_pos;
	FileType file_type;
	char *foo;
	myFile.GetInfo(foo, mySchema, file_size, first_pos, file_type, distinctString, numTuples);
	cerr << "File size: " << file_size << " First pos: " << first_pos <<endl;
	cerr << "Num Tuples: " << numTuples << " File Type: " << file_type << endl;
	myFile.Close();	
	dummy.Close();	

  	delete [] keys;
	delete [] distinctString;
	delete foo;
	
}
	
//********************************************
void Test5 (int numRecs, char* catName, char* fPath, char* sortedFName) { 
		
	howManyRecs = numRecs;
	keys = new int[howManyRecs];
	srand48(362346877);
	for (int i = 0; i < howManyRecs; i++) {
		keys[i] = lrand48 ();
	}
	
	// get the scehma for our file (I'll just ignore the file type
	// and the file information)
	int fSize, startPos, numTuples;
    FileType type;
    Schema mySchema ("");
	int numDistincts[NUMDIST];
    int *distinctString = numDistincts;
    dbCatalog.GetFileInfo (catName, mySchema, type, distinctString, numTuples, fSize, startPos);
		
	// create a database file
	cerr<<"Creating file\n";
	DBFile myFile;
	myFile.Create (fPath, mySchema, NUMALLOC, Heap);

	// add all of the data into it
	cerr<<"Inserting records\n";
	Record myData (mySchema);

	for (int i = 0; i < howManyRecs; i++) {
	
		// build up the record
		myData[0].InsertVal (&(keys[i]));
		for (int j = 0; j < 10; j++) {
			myData[j + 1].InsertVal (strings[(keys[i] % randomizers[j]) % 607]);
		}

		// put him into the file
		myFile.Insert (myData);

		if (i % 10000 == 0) {
			cerr << i << " ";
		}
	}	
	cerr << endl;
	
	//find some existent and some non-existent records
	cerr << "Checking correctness randomly for some existing and some non-existing records..!" << endl;
	int chkLimit = 20;
	for(int i = 0;i < chkLimit; i++) {
		//randomly choose a record to check
		int record = (drand48 () * howManyRecs) + 1;
		if(i % 2 == 0){
			//find an existent record
			myData[0].InsertVal(&(keys[record]));
			if(myFile.Find(myData[0]) != 1)
					cerr << "ERROR: Didnt find an existent record !" << endl;
		}
		else	//find a non-existent record..
		{
			record = -record;
			myData[0].InsertVal(&record);
			myFile.Find(myData[0]);
		}
		cerr <<i<< ".";
	}
		
	cerr <<endl<< "Sorting the file..TPMMS " << endl;

	//sort the file to a new file
	DBFile dummy;
	cerr<<"Now sorting the file\n";
	myFile.Convert (Sorted, dummy, sortedFName, mySchema.GetAtts(mySchema.GetKey()));
	cerr << "Sort complete..verifying sort correctness " << endl;

	//sort the keys array
	qsort (keys, howManyRecs, sizeof (int), intcompar);

	//move pointer to first record in the file
	myData[0].InsertVal(&(keys[0])); 
	dummy.Find(myData[0]);
		
	//now check if records are in ascending order of the key
	Record nextRec(mySchema);
		
	//verify whether the data is in sorted order
	for (int i=0;i<howManyRecs;i++) {
			
		myData[0].InsertVal (&(keys[i]));
		for (int j = 0; j < 10; j++) {
			myData[j + 1].InsertVal (strings[(keys[i] % randomizers[j]) % 607]);
		}	
		
		//get the next record from the file
		dummy.GetNext (nextRec);
		
		// and compare all of the attributes
		int OK = 1;
		for (int j = 0; j < 11; j++) {
			if (nextRec[j].Compare (myData[j])) {
				OK = 0;
			}
		}	
		
		if (numRecs > 10000 && i%10000 == 0)
			cerr<<i<<"*";

		// let 'em know if there was an error
		if (!OK) {
			cerr << "Error in record #" << i;
			cerr << " Expected:";
			cerr << myData << "\n";
		}	
	}

	cerr << endl<< "Searching records in the sorted file..." << endl;
	chkLimit = 100;
	for(int i = 0;i < chkLimit; i++) {
			
		//randomly choose a record to check
		int record = (drand48 () * howManyRecs)+1;
		if(i % 2 == 0){
			//find an existent record
			myData[0].InsertVal(&(keys[record]));
			cerr << record << "+";
			if(dummy.Find(myData[0]) != 1)
				cerr << "ERROR: Didnt find an existent record !" << endl;
		}
		else	//find a non-existent record..
		{
			record = -record;
			myData[0].InsertVal(&record);
			dummy.Find(myData[0]);
		}
		cerr <<i<<".";
	}
	cerr<<endl;
  	delete [] keys;
	delete [] distinctString;
}


//********************************************
void Test6 (int numRecs, char* catName, char* fPath, char* sortedFName) { 
		
	howManyRecs = numRecs;
	keys = new int[howManyRecs];
	srand48(362346877);
	for (int i = 0; i < howManyRecs; i++) {
		keys[i] = lrand48 ();
	}
	
	// get the scehma for our file (I'll just ignore the file type
	// and the file information)
	int fSize, startPos, numTuples;
    FileType type;
    Schema mySchema ("");
	int numDistincts[NUMDIST];
    int *distinctString = numDistincts;
    dbCatalog.GetFileInfo (catName, mySchema, type, distinctString, numTuples, fSize, startPos);
		
	// create a database file
	cerr<<"Creating file\n";
	DBFile myFile;
	myFile.Create (fPath, mySchema, NUMALLOC, Heap);

	// add all of the data into it
	cerr<<"Inserting records\n";
	Record myData (mySchema);

	for (int i = 0; i < howManyRecs; i++) {
	
		// build up the record
		myData[0].InsertVal (&(keys[i]));
		myData[1].InsertVal (strings[(keys[i] % randomizers[1]) % 607]);

		// put him into the file
		myFile.Insert (myData);

		if (i % 10000 == 0) {
			cerr << i << " ";
		}
	}	
	cerr << endl;
	
	//sort the file to a new file
	DBFile dummy;
	cerr<<"Now sorting the file\n";
	myFile.Convert (Sorted, dummy, sortedFName, mySchema.GetAtts(mySchema.GetKey()));
	cerr << "Done with sorting " << endl;
		
	//sort the keys array
	qsort (keys, howManyRecs, sizeof (int), intcompar);

	//move pointer to first record in the file
	myData[0].InsertVal(&(keys[0])); 
	dummy.Find(myData[0]);
	Record nextRec(mySchema);
 
	for (int i=0;i<howManyRecs;i++) {
			
		myData[0].InsertVal (&(keys[i]));
		myData[1].InsertVal (strings[(keys[i] % randomizers[1]) % 607]);
			
		//get the next record from the file
		dummy.GetNext(nextRec);
		
		// and compare all of the attributes
		int OK = 1;
		for (int j = 0; j < 2; j++) {
			if (nextRec[j].Compare (myData[j])) {
				OK = 0;
			}
		}	
		
		if (numRecs > 10000 && i%10000 == 0)
		cerr<<i<<"*";

		// let 'em know if there was an error
		if (!OK) {
			cerr << "Error in record #" << i;
			cerr << " Found:";
			cerr << nextRec;
			cerr << " Expected:";
			cerr << myData << "\n";
		}

	}

	cerr<<endl;
	//check if GetInfo returns the right information
	int file_size, first_pos;
	FileType file_type;
	char *foo;
	dummy.GetInfo(foo, mySchema, file_size, first_pos, file_type, distinctString, numTuples);
	cerr << "File size: " << file_size << " First pos: " << first_pos <<endl;
	cerr << "Num Tuples: " << numTuples << " File Type: " << file_type << endl;
	dummy.Close();	

  	delete [] keys;
	delete [] distinctString;
	delete foo;
}

//********************************************
void Test7 (int numRecs, char* catName, char* fPath, char* sortedFName) { 
		
	howManyRecs = numRecs;
	keys = new int[howManyRecs];
	srand48(362346877);
	for (int i = 0; i < howManyRecs; i++) {
		keys[i] = lrand48 ();
	}
	
	// get the scehma for our file (I'll just ignore the file type
	// and the file information)
	int fSize, startPos, numTuples;
    FileType type;
    Schema mySchema ("");
	int numDistincts[NUMDIST];
    int *distinctString = numDistincts;
    dbCatalog.GetFileInfo (catName, mySchema, type, distinctString, numTuples, fSize, startPos);
		
	// create a database file
	cerr<<"Creating file\n";
	DBFile myFile;
	myFile.Create (fPath, mySchema, NUMALLOC, Heap);

	// add all of the data into it
	cerr<<"Inserting records\n";
	Record myData (mySchema);
	for (int i = 0; i < howManyRecs; i++) {
		// build up the record
		myData[0].InsertVal (&(keys[i]));
		for (int j = 0; j < 10; j++) {
				myData[j+1].InsertVal (strings[(keys[i] % randomizers[j]) % 607]);
		}

		// put him into the file
		myFile.Insert (myData);
		if (i % 10000 == 0) {
			cerr << i << " ";
		}

	}		
	cerr<<endl;

	// sort the file

	//load up the new schema
	Schema myNewSchema ("");	
	dbCatalog.GetFileInfo ("test7NewSchema", myNewSchema, type, distinctString, numTuples, fSize, startPos);
	cerr << "Doing heap-heap conversion.." << endl;
	myFile.Convert (Heap, myNewSchema.GetAtts(myNewSchema.GetKey()));
	cerr << "Done conversion" << endl;

	Record myData1 (myNewSchema);
	myFile.Reset();
	
	cerr<<"Checking the file for correctness\n";
	// now, scan the file and see if we were OK
	for (int i = 0; i < howManyRecs; i++) {

		// build up the reocrd that we _schould_ have
		myData1[0].InsertVal (&(keys[i]));
		for (int j = 0; j < 10; j++) {
			myData1[j + 1].InsertVal (strings[(keys[i] % randomizers[j]) % 607]);
		}
	
		// get the record from the file
		Record fromFile (myNewSchema);
		myFile.GetNext (fromFile);
			
		// and compare all of the attributes
		int OK = 1;
		for (int j = 0; j < 11; j++) {
			if (fromFile[j].Compare (myData1[j])) {
				OK = 0;
			}
		}

		if (numRecs > 10000 && i%10000 == 0)
		cerr<<i<<"*";

		// let 'em know if there was an error
		if (!OK) {
			cerr << "Error in record #" << i;
			cerr << " Found:";
			cerr << fromFile;
			cerr << " Expected:";
			cerr << myData << "\n";
		}

	}

	cerr<<endl;
	//check if GetInfo returns the right information
	int file_size, first_pos;
	FileType file_type;
	char *foo;
	myFile.GetInfo(foo, mySchema, file_size, first_pos, file_type, distinctString, numTuples);
	cerr << "File size: " << file_size << " First pos: " << first_pos <<endl;
	cerr << "Num Tuples: " << numTuples << " File Type: " << file_type << endl;
	myFile.Close();	

  	delete [] keys;
	delete [] distinctString;
	delete foo;
			
}

//test8: heap-sorted convertion test.
//sort on string attribute, test find() meanwhile.
void Test8 (int numRecs, char* catName, char* fPath) { 
		
	howManyRecs = numRecs;
	keys = new int[howManyRecs];
	srand48(362346877);
	for (int i = 0; i < howManyRecs; i++) {
		keys[i] = lrand48 ();
	}
	
	// get the scehma for our file (I'll just ignore the file type
	// and the file information)
	int fSize, startPos, numTuples;
    FileType type;
    Schema mySchema ("");
	int numDistincts[NUMDIST];
    int *distinctString = numDistincts;
    dbCatalog.GetFileInfo (catName, mySchema, type, distinctString, numTuples, fSize, startPos);
		
	// create a database file
	cerr<<"Creating file\n";
	DBFile myFile;
	myFile.Create (fPath, mySchema, NUMALLOC, Heap);
	
	char **att0 = new char*[howManyRecs]; 
	
	// add all of the data into it
	cerr<<"Inserting records...\n";
	Record myData (mySchema);
	
	int att0ind = mySchema.GetIndex("att0");
	
	//cerr<<"att0ind :"<<att0ind<<endl;
	
	for (int i = 0; i < howManyRecs; i++) {
		// build up the record
		myData[0].InsertVal (&(keys[i]));
		for (int j = 0; j < 10; j++) {
				myData[j+1].InsertVal (strings[(keys[i] % randomizers[j]) % 607]);
		}
		
		//construct the array for attribute att0
		att0[i] = new char[100];
		//cout << i << "," << keys[i] <<","<< strings[(keys[i] % randomizers[0]) % 607] << endl;
		
		//cout << i << "," << keys[i] <<","<< randomizers[2]<< "," <<(keys[i] % randomizers[2]) % 607 << endl;
		strcpy(att0[i], strings[(keys[i] % randomizers[att0ind - 1]) % 607]); 
		
		// put him into the file
		myFile.Insert (myData);
		if (i % 10000 == 0) {
			cerr << i << " ";
		}

	}		
	cerr<<endl;
		
	//sort the file to a new file
	
	cerr<<"Now sorting the file on attribute 'att0'\n";
	myFile.Convert (Sorted, mySchema.GetAtts("att0"));
	
	cerr << "Sort complete..verifying sort correctness " << endl;
	
	//sort the new keys array
	qsort (att0, howManyRecs, sizeof(char *),strcompar);
		
	// and see if the answer is correct;  first, find the first record
	int keyindex = mySchema.GetIndex ("att0");	
	
	myData[keyindex].InsertVal (att0[0]);
	cout<<"find starts\n";	
	int ret = myFile.Find (myData[keyindex]);
	cout<<"find ends\n";
	if(ret != 1)
		cerr << "Didn't find an existing record" << endl;

	// now, scan the file and see if we were OK
	cerr<<"Scanning the sorted file..."<<endl;
	for (int i = 0; i < howManyRecs; i++) 
	{
		// build up the reocrd that we _schould_ have
		myData[keyindex].InsertVal (att0[i]);
	//	cout<<"att0"<<att0[i]<<endl;		
		// get the record from the file
		Record fromFile (mySchema);
		myFile.GetNext (fromFile);
			
		// and compare all of the attributes
		int OK = 1;
		
		if (fromFile[keyindex].Compare (myData[keyindex])) {
			OK = 0;
		}

		if (numRecs > 10000 && i%10000 == 0)
			cerr<<i<<"*";

		// let 'em know if there was an error
		if (!OK) {
			cerr << "Error in record #" << i;
			cerr << " Found record with key:";
			cerr << fromFile[keyindex];
			cerr << " Expected:";
			cerr << myData[keyindex] << "\n";
		}

	}
	cerr<<endl;
	
	myFile.Close();	
  	delete [] keys;
  	
  	for (int i = 0; i < howManyRecs; i++) {
  		delete []att0[i];
  	}
    delete [] att0;
    	
  	delete [] distinctString;
	//delete foo;
}

//test9: find operation test
//heap->sorted by key->find
void Test9 (int numRecs, char* catName, char* fPath, char* sortedFName) { 
		
	howManyRecs = numRecs;
	keys = new int[howManyRecs];
	srand48(362346877);
	int randnum;
	for (int i = 0; i < howManyRecs; i++) {
		randnum = lrand48();
		//let all keys be even numbers
		if (randnum % 2 == 0)
			keys[i] = randnum;
		else
			keys[i] = randnum + 1;			
	}
	
	
	// get the scehma for our file (I'll just ignore the file type
	// and the file information)
	int fSize, startPos, numTuples;
    FileType type;
    Schema mySchema ("");
	int numDistincts[NUMDIST];
    int *distinctString = numDistincts;
    dbCatalog.GetFileInfo (catName, mySchema, type, distinctString, numTuples, fSize, startPos);
		
	// create a database file
	cerr<<"Creating file\n";
	DBFile myFile;
	myFile.Create (fPath, mySchema, NUMALLOC, Heap);

	// add all of the data into it
	cerr<<"Inserting records\n";
	Record myData (mySchema);

	for (int i = 0; i < howManyRecs; i++) {
	
		// build up the record
		myData[0].InsertVal (&(keys[i]));
		for (int j = 0; j < 10; j++) {
			myData[j + 1].InsertVal (strings[(keys[i] % randomizers[j]) % 607]);
		}

		// put him into the file
		myFile.Insert (myData);

		if (i % 10000 == 0) {
			cerr << i << " ";
		}
	}	
	cerr << endl;
		
	cerr <<endl<< "Sorting the file..TPMMS" << endl;

	//sort the file to a new file
	DBFile dummy;
	cerr<<"Now sorting the file\n";
	myFile.Convert (Sorted, dummy, sortedFName, mySchema.GetAtts(mySchema.GetKey()));
	cerr << "Sort complete..verifying sort correctness " << endl;

	//sort the keys array
	qsort (keys, howManyRecs, sizeof (int), intcompar);

	//find using key attribute
	int chkLimit = 100;
	cerr << "test search of existent record!"<<endl;
	for(int i = 0;i < chkLimit; i++) {
			
		//randomly choose a record to check
		int record = (drand48 () * howManyRecs)+1;
	
		//find an existent record
		myData[0].InsertVal(&(keys[record]));
		//cerr << record << "+";
		if(dummy.Find(myData[0]) != 1)
			cerr << "ERROR: Didnt find an existent record !" << endl;
		cerr <<i<<".";
	}
	cerr << endl;
	cerr<<"Test search of non-existent record!"<<endl;
	for (int i=0;i< chkLimit; i++) {
		
		//randomly choose a record to check
		int record = (drand48 () * howManyRecs)+1;
		//find a non-existent record..
		int num;
		num = keys[record] - 1;
		myData[0].InsertVal(&num);
		
		if(dummy.Find(myData[0]) == 1)
			cerr << "ERROR: Found a non-existent record!"<<endl;
			
		cerr <<i<<".";
	}

	//find an non-existent record and the pointer should be put at the last record.
	cerr<<endl;
	cerr<<"Test search of a non-existent record larger than all the records!"<<endl;
	int num = keys[howManyRecs - 1] + 1;
	myData[0].InsertVal(&num);
	if(dummy.Find(myData[0]) != -1)
		cerr << "ERROR: Found a non-existent record!"<<endl;
		
	//test if the pointer is located at the last record
	Record rec1(mySchema);
	if (dummy.GetNext(rec1)) 
		cerr << "ERROR: The pointer is not pointed past the end of the dbfile!"<<endl;
	  	
  	dummy.Close();
  	delete [] keys;
}

//test10: TPMMS test
//Create a heap file with a bunch of same values and a small fraction (10%) of random values.
//Test the convert function.
void Test10 (int numRecs, char* catName, char* fPath) { 
		
	howManyRecs = numRecs;
	keys = new int[howManyRecs];
	srand48(2376234823);
	int p = howManyRecs/10;
	for (int i = 0; i < p; i++) {
		keys[i] = lrand48 ();
	}
	for (int i = p; i<howManyRecs; i++){
		keys[i] = 11;
	}
	
	// get the scehma for our file (I'll just ignore the file type
	// and the file information)
	int fSize, startPos, numTuples;
    FileType type;
    Schema mySchema ("");
    int numDistincts[NUMDIST];
    int *distinctString = numDistincts;
    dbCatalog.GetFileInfo (catName, mySchema, type, distinctString, numTuples, fSize, startPos);

	// create a database 
	cerr<<"Creating file\n";
	DBFile myFile;
	myFile.Create (fPath, mySchema, NUMALLOC, Heap);

	// add all of the data into it
	cerr<<"Inserting records\n";
	Record myData (mySchema);
	for (int i = 0; i < howManyRecs; i++) {
		// build up the record
		myData[0].InsertVal (&(keys[i]));
		for (int j = 0; j < 10; j++) {
			myData[j + 1].InsertVal (strings[(keys[i] % randomizers[j]) % 607]);
		}

		// put him into the file
		myFile.Insert (myData);
		if (i % 5000 == 0) {
			cerr << i << " ";
		}
	}		
	cerr<<endl;

	// sort the file
	cerr<<"Now converting heap file\n";
	myFile.Convert (Sorted, mySchema.GetAtts(mySchema.GetKey()));
	cout << "Done sorting. Checking the sorted file for correctness.\n";
		
	// sort our own copy of the keys
	qsort (keys, howManyRecs, sizeof (int), intcompar);

	// and see if the answer is correct;  first, find the first record
	myData[0].InsertVal (&keys[0]);
	myFile.Find (myData[0]);
		
	// now, scan the file and see if we were OK
	for (int i = 0; i < howManyRecs; i++) 
	{
		// build up the reocrd that we _schould_ have
		myData[0].InsertVal (&(keys[i]));
		for (int j = 0; j < 10; j++) {
			myData[j + 1].InsertVal (strings[(keys[i] % randomizers[j]) % 607]);
		}
		// get the record from the file
		Record fromFile (mySchema);
		myFile.GetNext (fromFile);
			
		// and compare all of the attributes
		int OK = 1;
		for (int j = 0; j < 11; j++) {
			if (fromFile[j].Compare (myData[j])) {
				OK = 0;
			}
		}

		if (numRecs > 10000 && i%10000 == 0)
			cerr<<i<<"*";

		// let 'em know if there was an error
		if (!OK) {
			cerr << "Error in record #" << i;
			cerr << " Found:";
			cerr << fromFile;
			cerr << " Expected:";
			cerr << myData << "\n";
		}

	}
	cerr <<endl<< "File check complete...Now converting to a heap " << endl;
	
	//now convert it to a heap..shudn't seg fault
	
	myFile.Convert (Heap, mySchema.GetAtts(mySchema.GetKey()));
	
	//check if GetInfo returns the right information
	int file_size, first_pos;
	FileType file_type;
	char *foo;
	myFile.GetInfo(foo, mySchema, file_size, first_pos, file_type, distinctString, numTuples);
	cerr << "File size: " << file_size << " First pos: " << first_pos <<endl;
	cerr << "Num Tuples: " << numTuples << " File Type: " << file_type << endl;
	myFile.Close();	

  	delete [] keys;
	delete [] distinctString;
	delete foo;
}

//test11: sorted-sorted conversion
//create a heap file, convert to sorted, then sort again on another attribute.
void Test11 (int numRecs, char* catName, char* fPath) { 
		
	howManyRecs = numRecs;
	keys = new int[howManyRecs];
	srand48(362346877);
	for (int i = 0; i < howManyRecs; i++) {
		keys[i] = lrand48 ();
	}
	
	// get the scehma for our file (I'll just ignore the file type
	// and the file information)
	int fSize, startPos, numTuples;
    FileType type;
    Schema mySchema ("");
	int numDistincts[NUMDIST];
    int *distinctString = numDistincts;
    dbCatalog.GetFileInfo (catName, mySchema, type, distinctString, numTuples, fSize, startPos);
		
	// create a database file
	cerr<<"Creating file\n";
	DBFile myFile;
	myFile.Create (fPath, mySchema, NUMALLOC, Heap);
	
	char **att0 = new char*[howManyRecs]; 
	
	// add all of the data into it
	cerr<<"Inserting records...\n";
	Record myData (mySchema);
	
	int att0ind = mySchema.GetIndex("att0");
	
	for (int i = 0; i < howManyRecs; i++) {
		// build up the record
		myData[0].InsertVal (&(keys[i]));
		for (int j = 0; j < 10; j++) {
				myData[j+1].InsertVal (strings[(keys[i] % randomizers[j]) % 607]);
		}
		//construct the array for attribute att0
		att0[i] = new char[100];
		
		strcpy(att0[i], strings[(keys[i] % randomizers[att0ind - 1]) % 607]); 
		
		// put him into the file
		myFile.Insert (myData);
		if (i % 10000 == 0) {
			cerr << i << " ";
		}

	}		
	cerr<<endl;

	//sort the file to a new file
	cerr<<"Now sorting the file on attribute 'keyatt'\n";
	myFile.Convert (Sorted, mySchema.GetAtts(mySchema.GetKey()));
	
	cerr << "Sort complete..verifying sort correctness " << endl;
	//sort the keys array
	qsort (keys, howManyRecs, sizeof (int), intcompar);
	
	// and see if the answer is correct;  first, find the first record
	myData[0].InsertVal (&keys[0]);
	myFile.Find (myData[0]);
	
	// now, scan the file and see if we were OK
	for (int i = 0; i < howManyRecs; i++) 
	{
		// build up the reocrd that we _schould_ have
		myData[0].InsertVal (&(keys[i]));
		for (int j = 0; j < 10; j++) {
			myData[j + 1].InsertVal (strings[(keys[i] % randomizers[j]) % 607]);
		}
		// get the record from the file
		Record fromFile (mySchema);
		myFile.GetNext (fromFile);
			
		// and compare all of the attributes
		int OK = 1;
		for (int j = 0; j < 11; j++) {
			if (fromFile[j].Compare (myData[j])) {
				OK = 0;
			}
		}

		if (numRecs > 10000 && i%10000 == 0)
			cerr<<i<<"*";

		// let 'em know if there was an error
		if (!OK) {
			cerr << "Error in record #" << i;
			cerr << " Found:";
			cerr << fromFile;
			cerr << " Expected:";
			cerr << myData << "\n";
		}

	}
	cerr<<endl;
		
	//check if GetInfo returns the right information
	int file_size, first_pos;
	FileType file_type;
	char *foo;
	myFile.GetInfo(foo, mySchema, file_size, first_pos, file_type, distinctString, numTuples);
	cerr << "File size: " << file_size << " First pos: " << first_pos <<endl;
	cerr << "Num Tuples: " << numTuples << " File Type: " << file_type << endl;
	
	//setup the new schema
	mySchema.SetKey("att0");
	
	cerr << "Do sorted-sorted conversion on attribute 'att0'..." << endl;
	myFile.Convert (Sorted, mySchema.GetAtts(mySchema.GetKey()));
	cerr << "Done conversion." << endl;
		
	//sort the new keys array
	qsort (att0, howManyRecs, sizeof(char *), strcompar);
	
	// and see if the answer is correct;  first, find the first record
	Record myData1 (mySchema);
	
	int keyindex =  mySchema.GetIndex ("att0");	

	myData1[keyindex].InsertVal (att0[0]);
	
	int ret = myFile.Find (myData1[keyindex]);
	cout<<"ret1284 = "<<ret<<endl;
	// now, scan the file and see if we were OK
	cerr<<"Scanning the sorted file..."<<endl;
	for (int i = 0; i < howManyRecs; i++) 
	{
		// build up the reocrd that we _schould_ have
		myData1[keyindex].InsertVal (att0[i]);
		
		// get the record from the file
		Record fromFile (mySchema);
		myFile.GetNext (fromFile);
			
		// and compare all of the attributes
		int OK = 1;
		
		if (fromFile[keyindex].Compare (myData1[keyindex])) {
			OK = 0;
		}
		if (numRecs > 10000 && i%10000 == 0)
			cerr<<i<<"*";

		// let 'em know if there was an error
		if (!OK) {
			cerr << "Error in record #" << i;
			cerr << " Found record with key:";
			cerr << fromFile[keyindex];
			cerr << " Expected:";
			cerr << myData1[keyindex] << "\n";
		}

	}
		
	//check if GetInfo returns the right information

	myFile.GetInfo(foo, mySchema, file_size, first_pos, file_type, distinctString, numTuples);
	cerr << "File size: " << file_size << " First pos: " << first_pos <<endl;
	cerr << "Num Tuples: " << numTuples << " File Type: " << file_type << endl;

	myFile.Close();	
  	delete [] keys;
  	
  	for (int i = 0; i < howManyRecs; i++) {
  		delete []att0[i];
  	}
   delete [] att0;
    	
  	delete [] distinctString;
	delete foo;
			
}

//test12: TPMMS test
//create a heap file with ascending ordered records or descending ordered records
//Test the convert function
void Test12 (int numRecs, char* catName, char* fPath, int tType) { 
		
	howManyRecs = numRecs;
	keys = new int[howManyRecs];
	srand48(2376234823);
	int p = howManyRecs/2;
	if (tType == 1)	{//ascending order first
		
		for (int i = 0; i < p; i++) {
			keys[i] = 11;
		}
		
		for (int i=p; i< howManyRecs; i++){
			keys[i] = i;
		}
	} else {//descending order
		for (int i = 0; i < p; i++) {
			keys[i] = 362346877 - i;
		}
		
		for (int i=p; i< howManyRecs; i++){
			keys[i] = 11;
		}
	}
	// get the scehma for our file (I'll just ignore the file type
	// and the file information)
	int fSize, startPos, numTuples;
    FileType type;
    Schema mySchema ("");
    int numDistincts[NUMDIST];
    int *distinctString = numDistincts;
    dbCatalog.GetFileInfo (catName, mySchema, type, distinctString, numTuples, fSize, startPos);

	// create a database 
	cerr<<"Creating file\n";
	DBFile myFile;
	myFile.Create (fPath, mySchema, NUMALLOC, Heap);

	// add all of the data into it
	cerr<<"Inserting records\n";
	Record myData (mySchema);
	for (int i = 0; i < howManyRecs; i++) {
		// build up the record
		myData[0].InsertVal (&(keys[i]));
		for (int j = 0; j < 10; j++) {
			myData[j + 1].InsertVal (strings[(keys[i] % randomizers[j]) % 607]);
		}

		// put him into the file
		myFile.Insert (myData);
		if (i % 5000 == 0) {
			cerr << i << " ";
		}
	}		
	cerr<<endl;

	// sort the file

	cerr<<"Now converting heap file\n";
	myFile.Convert (Sorted, mySchema.GetAtts(mySchema.GetKey()));
	cout << "Done sorting. Checking the sorted file for correctness.\n";
		
	// sort our own copy of the keys
	qsort (keys, howManyRecs, sizeof (int), intcompar);

	// and see if the answer is correct;  first, find the first record
	myData[0].InsertVal (&keys[0]);
	cout<<"MYDATA[0]"<<endl<<myData[0]<<endl;
	myFile.Find (myData[0]);
		
	// now, scan the file and see if we were OK
	for (int i = 0; i < howManyRecs; i++)
	{
		// build up the reocrd that we _schould_ have
		myData[0].InsertVal (&(keys[i]));
		for (int j = 0; j < 10; j++) {
			myData[j + 1].InsertVal (strings[(keys[i] % randomizers[j]) % 607]);
		}
		// get the record from the file
		Record fromFile (mySchema);
		myFile.GetNext (fromFile);
			
		// and compare all of the attributes
		int OK = 1;
		for (int j = 0; j < 11; j++) {
			if (fromFile[j].Compare (myData[j])) {
				OK = 0;
			}
		}

		if (numRecs > 10000 && i%10000 == 0)
			cerr<<i<<"*";

		// let 'em know if there was an error
		if (!OK) {
			cerr << "Error in record #" << i;
			cerr << " Found:";
			cerr << fromFile;
			cerr << " Expected:";
			cerr << myData << "\n";
		}

	}
	cerr <<endl<< "File check complete...Now converting to an heap " << endl;
	
	//now convert it to a heap..shudn't seg fault
	myFile.Convert (Heap, mySchema.GetAtts(mySchema.GetKey()));
	
	//check if GetInfo returns the right information
	int file_size, first_pos;
	FileType file_type;
	char *foo;
	myFile.GetInfo(foo, mySchema, file_size, first_pos, file_type, distinctString, numTuples);
	cerr << "File size: " << file_size << " First pos: " << first_pos <<endl;
	cerr << "Num Tuples: " << numTuples << " File Type: " << file_type << endl;
	myFile.Close();	

  	delete [] keys;
	delete [] distinctString;
	delete foo;
}

//***************************************
int main(int argc, char *argv[]) {
	
	if (argc < 2) {
		cerr << "You need to supply me the test-case to run as a command-line arg.." << endl;
		cerr << "Usage: <Executable-Name> <Test-case# b/w 1-10>" << endl;
		exit (1);
	}

	int test = atoi(argv[1]);
	cerr << "----------------------------------------\n";;;;
	cerr <<" Running test case " << test <<endl;
	cerr << "----------------------------------------\n";
	cerr << "Host: ";
	system ("echo $HOST");
	// initialize the buffer 
	myBuffer.Initialize ("cat1");

	// read in the 607 strings
	FILE *inFile = fopen ("defs", "r");
	for (int i = 0; i < 607; i++) {
		strings[i] = new char[100];
		GetLine (strings[i], inFile);
	}
  	fclose(inFile);
   	
	// create 10 different randomizers
	for (int i = 0; i < 10; i++) {
		randomizers[i] = lrand48 () % 200000 + 7;
		//cerr<<randomizers[i]<<endl;
	}
	
	
	time_t begintimer, endtimer;
	double time1, time2;
	//#######  Start testing ###########
	
	if(test == 1) {	
		Test1(1000,"test1","/var/tmp/test1file");
	}

	if(test == 2) {
		Test2(1000,"test2","/var/tmp/test2file");
	}
	
	if (test == 3) {
		Test3(1000,"test3","/var/tmp/test3file", "/var/tmp/test3sorted");
	}		
	
	if (test == 4) {
		Test4(1000,"test4","/var/tmp/test4file", "/var/tmp/test4sorted");
	}		
	
	if (test == 5) {
		Test5(30000,"test5","/var/tmp/test5file", "/var/tmp/test5sorted");
	}		
	
	if (test == 6) {
		Test6(50000,"test6","/var/tmp/test6file", "/var/tmp/test6sorted");
	}		
	
	if (test == 7) {
		Test7(300000,"test7","/var/tmp/test7file", "/var/tmp/test7sorted");
	}		
	
	if (test == 8) {
		Test1(20000,"test8","/var/tmp/test8file");
	}		
	
	if (test == 9) {
		Test2(30000,"test9","/var/tmp/test9file");
	}		
	
	if (test == 10) {
		Test4(20000,"test10","/var/tmp/test10file", "/var/tmp/test10sorted");
	}		
  	
  	if (test == 11) {
  		Test9(20000,"test10","/var/tmp/test11file", "/var/tmp/test11sorted");
  	}
  	
  	if (test == 12) {	
  		Test8(20000, "test7", "/var/tmp/test12file");
  	}

	if (test == 13) {
		Test11(20000, "test7", "/var/tmp/test13file");
	}
	
	if (test == 14) {
		Test10(20000, "test1", "/var/tmp/test14file");
	}
  	
  	if (test == 15) {
		Test12(200000, "test1", "/var/tmp/test15file", 1);
	}

	time(&begintimer); 
	if (test == 16) {
		Test12(200000, "test1", "/var/tmp/test16file", 2);
	}
	time(&endtimer); 
	time1 = difftime(endtimer, begintimer); 

  	if (test == 17) {
  		Test9(200000, "test10","/var/tmp/test17file", "/var/tmp/test17sorted");
  	}
  	
  	if (test == 18) {	
  		Test8(200000, "test7", "/var/tmp/test18file");
  	}

	if (test == 19) {
		Test11(200000, "test7", "/var/tmp/test19file");
	}
	
	time(&begintimer); 
	if (test == 20) {
		Test10(300000, "test1", "/var/tmp/test20file");
	}
	time(&endtimer); 
	time2 = difftime(endtimer, begintimer); 
	
	cerr << "Running time: " << time1+time2 << " seconds" << endl; 
	
	for (int i = 0; i < 607; i++)
    	delete strings[i];
  	delete [] strings;
  
	return 0;
}
