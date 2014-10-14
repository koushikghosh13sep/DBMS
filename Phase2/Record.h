#ifndef _RECORD_H
#define _RECORD_H

#include <iostream>
#include "Schema.h"

using namespace std;
    
// This class is the in-memory representation of a data record.  It holds
// all of the records values, as well as the schema for the record so it
// knows how to interpret those values (for exmaple, it knows the type and
// size of each of the values that it holds)
class Record {
    
	private: 
		Value* atts;
		int numAtts;
		int key;
		
		friend ostream &operator << (ostream &input, Record &rec); 
    
	public:
    
		// Creates a record that can hold elements of the given
		// schema (a schema doesn't actually hold anything... it
		// only describes some data)
		Record (Schema &mySchema);
		
		~Record ();
    
		// Sets the key for the record to be the specified name
		void SetKey (char* attName);
    
		// Gets the name of the key
		char* GetKey ();
    
		// Print this record to the screen (stdout)
		void Print ();
    
		// Get the scpecified value using an array index... for 
		// example, if we have a record x, and we want the value
		// for the 5th attribute, we use x[4]
		Value &operator [] (int);
    
		// Find out which of our attributes has the given attribute
		// name... returns -1 if we don't have that attribute
		int GetIndex (char *attName);
    
		// Return how man bytes of binary storage it will take to
		// hold this record (for example, if we want to write him
		// to a page on disk)
		int GetNumBytes ();
    
		// Compares us with the parameter "me".  Returns a 
		// number less than, equal to, or greater than zero,
		// depending on whether we are less than, equal to,
		// or greater than the parameter.  Uses the key values
		// of the two records
		int Compare (Record &me);	
    
		// Load up this guy's values (which have been stored in 
		// binary) from the specified location in memory
		char* FromBinary (char* where);
    
		// Write this guy's values out in binary, to the specified
		// location in memory
		char* ToBinary (char* where);

		// Returns the pointer to the data value of a dataInd attribute
		void* GetDataVal (int dataInd);

};
    
#endif
