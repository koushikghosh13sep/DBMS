#ifndef _VALUE_H
#define _VALUE_H
  
#include <iostream>
#include "Attribute.h"

using namespace std;
    
// A value is an "instantiated" attribute that actually has a value.  For 
// example, if we have a record of type PERSON, then it's SSN is held in 
// this class
class Value : public Attribute {
    
	private: 
		void* data;

		friend class Record;

	public: 
    
		Value ();
		~Value ();

		// Prints an attribute to standard out
		void Print ();
    
		// Prints it to a stream
		friend ostream &operator << (ostream &input, Value &val); 
    
		// Copy the attribute "me"... since "me" is only an
		// attribute and has no associated value, our value is
		// then undefined until a value has been inserted	
		void Copy (Attribute &me);
    
		// Give ourselves a value.  This pointer is interpreted
		// depending on the type of the attribte/value that we
		// our supposed to hold.  For example, if we are an
		// Integer, then data have better point to an integer
		void InsertVal (void *data);
    	
		// Sets the attribute to have the given value
		void SetVal (Value v);
  
		// Compares us with the parameter "me".  Returns a 
		// number less than, equal to, or greater than zero,
		// depending on whether we are less than, equal to,
		// or greater than the parameter
		int Compare (Value &me);	
    
		// Returns the name
		char* GetName ();
    
		// Used in B Tree to set pair -> infinity
		void SetInfinity (); 
  
		// Directly get the data void pointer
		void* GetData ();
  
		// Copy Constructor
		Value (const Value & v);
		
		// Compares the value
		const Value &operator=( const Value & v);
  
};
       
#endif
