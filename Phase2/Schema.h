#ifndef _SCHEMA_H
#define _SCHEMA_H
  
#include <iostream>
#include "Value.h"
#include <stdio.h>
using namespace std;
    
// This holds the schema for a relation
class Schema {
    
	private: 
		Attribute* atts;
		int numAtts;
		int key;
		
		friend class Record;
		friend class Catalog;
    
	public:
		char* relName;	
		
		// Create a new relation schema, for a relation of the
		// given name
		Schema (char *relationName);
		Schema ();
		
		~Schema ();

		// Clear us out
		void Clear ();
    
		// Sets the key for the schema to be the specified name
		void SetKey (char *attName);
    
		// Gets the name of the key
		char *GetKey ();
    
		// Add an attribute to this schema (goes on the end, so
		// it's the last attribute)
		void AddAtt (Attribute &me);
    
		// Find out which of our attributes has the given attribute
		// name... returns -1 if we don't have that attribute
		int GetIndex (char *attName);
    
		// Gets the name of this relation
		char *GetName ();
    
		// Returns the number of attributes in this relation schema
		int NumAtts ();
    
		// Returns the number of attributes in this relation schema
		Attribute& GetAtts (char *attName);
		Attribute& GetAtts (int attInd);
  
		// Changes or adds a new name to this relation schema
		void AddName (char *relationName);
    
		//Get the record size
		int GetRecordSize ();
  
		// Copy the schema
		void Copy (Schema &me);
      
		// Get the scpecified value using an array index... for 
		// get the scpecified attribute using an array index... for 
		// example, if we have a record x, and we want the value
		// for the 5th attribute, we use x[4]
		Attribute &operator [] (int);

		//Print Schema to the File 
		void FPrintf (FILE* toMe);
		
		//**Copy Constructor
		Schema (const Schema &s); 
		
		//**Compares this schema with newSchema.
		//return:
		//	0   if the two schemas are the same
		//	1   if there are only differences in attribute names
		//	2   the sorting key is changed
		//	3   more changes
		int operator=(const Schema & newSchema);
};
    
#endif
