#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include "Value.h"

using namespace std;
  
//********************************************************   
Value :: Value () {
	data = 0;
	len = 0;
	name = 0;
}
    
//********************************************************   
Value :: ~Value () {
	if (data != NULL) {
		switch (myType) {
			case Integer : 
				delete (int*)data;
				break;
			case Real : 
				delete (double*)data;
				break;
			case CharString: 
				delete [] (char*)data;
				break;
		}
	}
	
	delete [] name;
	name = 0;
	data = 0;
}
 
//********************************************************   
void Value :: Print () {
	cout << *this;
}
    
//********************************************************   
ostream &operator << (ostream &input, Value &rec) {
    
	input << rec.name << ", ";
	if (rec.data == 0) {
		input << "<NULL>";
		return input;
	}

	if (rec.myType == Integer) {
		input << "type Integer: <";
		input << *((int *) rec.data) << ">";
		return input;
	}

	if (rec.myType == Real) {
		input << "type Real: <";
		input << *((double *) rec.data) << ">";
		return input;
	}

	if (rec.myType == CharString) {
		input << "type CharString [" << rec.len << "]: <";
		input << (char *) rec.data << ">";
		return input;
	}

	return input;
}
    
//********************************************************   
void Value :: Copy (Attribute &me) {
	Init (me.name, me.myType, me.len);	
}
    
//********************************************************   
void Value :: InsertVal (void *newData) {
	delete (char *) data;
	data = (void *) (new char[len]);
	bcopy (newData, data, len); 
}
    
//********************************************************   
void Value :: SetVal (Value newVal) {

	if (newVal.myType == Integer) {
		int ptr = *((int*) newVal.data);
		InsertVal (&ptr);
	}

	if (newVal.myType == Real) {
		double ptr = *((double*) newVal.data);
		InsertVal (&ptr);
	}

	if (newVal.myType == CharString) {
		InsertVal ((char*) newVal.data);
	}

	return;
}    
  
//********************************************************   
int Value :: Compare (Value &me) {
    
	if (myType != me.myType) {
		cout << "Warning!  Trying to compare apples and oranges.\n";
		cout << "Warning!  Trying to compare " << "myType " << myType << " and me.myType" <<  me.myType << endl;
	}

	if (myType == Integer) {
		if (*((int *) data) < *((int *) me.data))
			return -1;
		if (*((int *) data) > *((int *) me.data)) 
			return 1;
		return 0;
	}	

	if (myType == Real) {
		if (*((double *) data) < *((double *) me.data))
			return -1;
		if (*((double *) data) > *((double *) me.data)) 
			return 1;
		return 0;
	}

	if (myType == CharString) {
		int min = me.len;
		if (len < me.len)
			min = len;
		return strncmp ((const char *) data, (const char *) me.data, min); 
	}

	return -2;
}
    
//********************************************************   
char *Value :: GetName () {
	return name;
}
    
//********************************************************   
void Value :: SetInfinity () {
	if (data == 0) {
		cerr << "Can't set nothing to infinity.\n";
	} else {
		if (myType == Real) {
			double max = 9e99;
			InsertVal (&max);
		} else if (myType == Integer) {
			int max = 2147483647;
			InsertVal (&max);
		} else if (myType == CharString) {
			for (int i = 0; i < len; i++) {
				((char *) data)[i] = 255;
			}
			((char *) data)[len - 1] = 0;
		}
	}
}

//********************************************************   
void* Value :: GetData () {
	return data;
}
 
//********************************************************   
Value::Value( const Value &v) {
	myType = v.myType;
	len = v.len;
	name = strdup(v.name);
	data = (void*)(new char[len]);
	bcopy(v.data, data, len);
}
  
//********************************************************   
const Value &Value::operator=( const Value &v) {
	if (&v != this) {
		if (data != NULL) {
			switch (myType) {
				case Integer : 
					delete (int*)data;
					break;
				case Real : 
					delete (double*)data;
					break;
				case CharString: 
					delete [] (char*)data;
					break;
			}
		}
		
		delete [] name;

		myType = v.myType;
		len = v.len;  
		name = strdup(v.name);
		data = (void*)(new char[len]);
		bcopy(v.data, data, len);
	}
	
	return *this;
}
  
