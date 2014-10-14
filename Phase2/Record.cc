#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include "Record.h"

using namespace std;
  
//********************************************************   
Record :: Record (Schema &mySchema) {

	// copy all of the attributes
	atts = new Value[mySchema.numAtts];
	numAtts = mySchema.numAtts;
	key = mySchema.key;
	for (int i = 0; i < mySchema.numAtts; i++) {
		atts[i].Copy (mySchema.atts[i]);
		atts[i].data = new char[mySchema.atts[i].len];
		((char *) (atts[i].data))[0] = 0;
	}
}
    
//********************************************************   
Record :: ~Record () {
	delete [] atts;
	atts = 0;
}
 
//********************************************************   
void Record :: SetKey (char *attName) {
	key = GetIndex (attName);
}
    
//********************************************************   
char *Record:: GetKey () {
	if (key != -1) {
		return atts[key].GetName ();
	} else {
		cerr << "Error... no key for this record.\n";
		return 0;
	}
}
  
//********************************************************   
void Record :: Print () {
	cout << *this;
}
    
//********************************************************   
Value &Record :: operator [] (int which) {
	if (which > numAtts) {
		cerr << "Error, trying to go out of record bounds.\n";
	}
	return atts[which];
}
    
//********************************************************   
int Record :: GetIndex (char *attName) {
	for (int i = 0; i < numAtts; i++) {
		if (!strcmp (attName, atts[i].name))
			return i;
	}
	return -1;
}

//********************************************************   
int Record :: GetNumBytes () {
	int returnVal = 0;
	for (int i = 0; i < numAtts; i++) {
		returnVal += atts[i].len;
	}
	return returnVal;
}
 
//********************************************************   
int Record :: Compare (Record &me) {
	if (key != -1 && me.key != -1) {
		return (*this)[key].Compare (me[me.key]);
	} else {
		cerr << "Trying to compare records without 2 keys.\n";
		return 0;
	}
}
    
//********************************************************   
char *Record :: FromBinary (char *where) {
	for (int i = 0; i < numAtts; i++) {
		if (atts[i].data == 0){
			atts[i].data = new char[atts[i].len];
		}
		bcopy (where, atts[i].data, atts[i].len);
		where += atts[i].len;
	}
	return where;
}

 //********************************************************   
char *Record :: ToBinary (char *where) {
	for (int i = 0; i < numAtts; i++) {
		if (atts[i].data != 0) {
//			for(int j = 0 ; j < atts[i].len; j++)
//				while
//				cout<<(char*)atts[i].data[j];
//			cout<<endl;
			bcopy (atts[i].data, where, atts[i].len);
		} else {
			bzero (where, atts[i].len);
		}
		where += atts[i].len;
	}

	return where;
}
    
//********************************************************   
void *Record :: GetDataVal (int dataInd) {
	if (atts[dataInd].data != 0) {
		return atts[dataInd].data;
	} 

	return NULL;
}

//********************************************************   
ostream &operator << (ostream &input, Record &val) {
	input << "[";
	for (int i = 0; i < val.numAtts; i++) {
		if (i != 0)
			input << " ";
		input << val.atts[i] << "\n";
	}
	
	input << " KEY: " << val.atts[val.key];
	input << "]\n";
	return input;
} 

