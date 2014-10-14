#ifndef _ATTRIBUTE_H
#define _ATTRIBUTE_H
  
#include <iostream>
using namespace std;
class Value;
class Record;
class Schema;

   
// These are the three different data types that our DB supports
enum AttributeType {
	CharString,
	Real,
	Integer
};
    
// These are the three different file types that our DB suports
enum FileType {
	Sorted,
	Heap,
	BTree
};
    
// This class is used to hold an attribute for a schema.  For example,
// I have a PERSON relation with the attribute "SSN".  This class is
// used to hold an SSN
class Attribute {
    	
	private: 
		char* name;
		AttributeType myType;
		int len;
    		
		friend class Value;
		friend class Record;
		friend class Schema;
		friend ostream &operator << (ostream &input, Value &val); 
    
	public:
		Attribute ();
		Attribute(char* n, AttributeType t, int l); 
		~Attribute ();
		
		// Give the attribute a name, a type, and a size (in number
		// of bytes).  If the attribute is a Real then len should
		// be sizeof (double), if it's Integer, then sizeof (int)
		void Init (char* attName, AttributeType attType, int len);
    
		// Returns the length
		int GetLength ();
	  
		// Returns the name
		char* GetName ();
    
		// Make a copy of the parameter into this attribute
		void Copy (Attribute &me);

		// Returns the type 
		AttributeType GetType ();
  		
		// Copy Constructor
		Attribute (const Attribute & a);		
  		
		// Compares this attribute with newAttr
		// Return:
		//	0 the same
		//	1 only the names are different
		//	2 different types OR lengths
		int operator=(const Attribute & newAttr);

};
    
#endif
