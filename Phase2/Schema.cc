#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include "Schema.h"
  
using namespace std;

//********************************************************
Schema :: Schema(){
	atts = 0;
	numAtts = 0;
	key = -1;
	relName = 0;
}
//********************************************************   
Schema :: Schema (char *relationName) {
	atts = 0;
	numAtts = 0;
	key = -1;
	relName = strdup (relationName);
}
    
//********************************************************   
Schema :: ~Schema () {
	delete [] atts;
	delete relName;
	relName = 0;
	atts = 0;
}
    
//********************************************************   
void Schema :: Clear () {
	delete [] atts;
	delete relName;
	relName = 0;
	atts = 0;
	numAtts = 0;
	key = -1;
}
    
//********************************************************   
void Schema :: SetKey (char *attName) {
	key = GetIndex (attName);
}
    
//********************************************************   
char *Schema :: GetKey () {
	if (key != -1) {
		return atts[key].GetName ();
	} else {
		cerr << "Error... no key for this schema.\n";
		return 0;
	}
}
    
//********************************************************   
void Schema :: AddAtt (Attribute &me) {
	Attribute *newList = new Attribute[numAtts + 1];
	for (int i = 0; i < numAtts; i++) {
		newList[i].Copy (atts[i]);
	}
	newList[numAtts].Copy (me);
	numAtts++;
	delete [] atts;
	atts = newList;	
}
 
//********************************************************   
int Schema :: GetIndex (char *attName) {
	for (int i = 0; i < numAtts; i++) {
		if (!strcmp (attName, atts[i].name))
			return i;
		}
		return -1;
	}
    
//********************************************************   
char *Schema :: GetName () {
	return relName;
}
    
//********************************************************   
int Schema :: NumAtts () {
	return numAtts;
}
    
//********************************************************   
Attribute& Schema::GetAtts (char *attName) {
	int i = 0;
	for (; i < numAtts; i++) {
		if (!strcmp (attName, atts[i].name))
			break;
	}
	return atts[i];
}
  
//********************************************************   
Attribute& Schema::GetAtts (int attInd) {
	if (attInd > numAtts) {
		cerr<<"Not a valid attribute index\n";
		exit(-1);
	}

	return atts[attInd];
}
     
//********************************************************   
void Schema :: AddName (char *name) {
	delete relName;
	relName = strdup (name);
}

//********************************************************   
int Schema::GetRecordSize () {
	int returnVal = 0;
	for (int i = 0; i < numAtts; i++) {
		returnVal += atts[i].len;
	}
	return returnVal;
}
 
//********************************************************   
void Schema :: Copy (Schema &me) {
          
	Clear ();
          
	if (me.relName != 0)
	relName = strdup (me.relName);

	for (int i = 0; i < me.numAtts; i++) {
		Attribute a;
		a.Copy (me.atts[i]);
		AddAtt (a);
	}

	key = me.key;
}
 
//********************************************************   
Attribute &Schema :: operator [] (int which) {

	if (which > numAtts) {
	cerr << "Error, trying to go out of record bounds.\n";
	}

	return atts[which];
}
                          
//********************************************************   
void Schema :: FPrintf (FILE *toMe) {

	for (int i = 0; i < numAtts; i++) {
		if (atts[i].myType == Real) {
			fprintf (toMe, "REAL %s\n", atts[i].name);
		} else if (atts[i].myType == Integer) {
			fprintf (toMe, "INTEGER %s\n", atts[i].name);
		} else {
			fprintf (toMe, "STRING %d %s\n", atts[i].len, atts[i].name);
		}
	}
	
	if (key != -1)
	fprintf (toMe, "KEY %s\n", atts[key].name);
}

//********************************************************   
int Schema::operator=(const Schema & newSchema) {
	if (numAtts != newSchema.numAtts) 
		return 3;
	int highest = 0;
	for (int i = 0; i < numAtts; i++) {
		switch (atts[i] = newSchema.atts[i]) {
			case 2: 
				return 3;
			case 1:
				highest = 1;
		}
	}
	if (key != newSchema.key) 
		return 2;
	else
	return highest;
}
  
//********************************************************   
Schema :: Schema (const Schema & s) {
	numAtts = s.numAtts;
	key = s.key;
	relName = strdup (s.relName);

	atts = new Attribute[numAtts];
	for (int i = 0; i < numAtts; i++) {
		atts[i].Copy (s.atts[i]);
	}
}

