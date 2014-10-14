#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include "Attribute.h"
  
using namespace std;
  
//********************************************************   
Attribute :: Attribute () {
	name = 0;
}

//********************************************************   
Attribute :: Attribute(char* n, AttributeType t, int l) {
	name = strdup(n);
	myType = t;
	len = l;
}

//********************************************************   
Attribute :: ~Attribute () {
	delete (char *) name;
	name = 0;
}
  
//********************************************************   
void Attribute :: Init (char *attName, AttributeType attType, int myLen) {
    	
	if (attType == Real && myLen != sizeof (double)) {
		cerr << "Warning!! Len for type Real should be ";
		cerr << sizeof (double) << " bytes\n";
	} else if (attType == Integer && myLen != sizeof (int)) {
		cerr << "Warning!! Len for type Integer should be ";
		cerr << sizeof (int) << " bytes\n";
	}
	delete name;
	name = new char[strlen(attName) + 1];
	strcpy (name, attName);
	myType = attType;
	len = myLen;	
}

//********************************************************   
int Attribute::GetLength() {
	return len;
}

//********************************************************   
char *Attribute :: GetName () {
	return name;
}
  
//********************************************************   
void Attribute :: Copy (Attribute &me) {
	Init (me.name, me.myType, me.len);	
}
    
//********************************************************   
AttributeType Attribute::GetType() {
	return myType;
}
  
//********************************************************   
Attribute :: Attribute (const Attribute & a) {
	name = strdup(a.name);
	myType = a.myType;
	len = a.len;
}
  
//********************************************************   
int Attribute::operator=(const Attribute & newAttr) {
	if (myType != newAttr.myType || len != newAttr.len)
		return 2;
	else if (strcasecmp(name, newAttr.name) != 0) 
		// Case insensitive comparision because SQL is case insensitive
		return 1;
	else
		return 0;
}
