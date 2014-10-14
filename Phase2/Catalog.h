#ifndef _CATALOG_H
#define _CATALOG_H

#include <iostream>
#include "Schema.h"
#include "Record.h"

using namespace std;
    
// This class serves as an interface to the database catalog
class Catalog {
    
	private: 

		char* strName;
		int pageSize;
		int numPages;
    
	public:
    
		// Open up the specified catalog file... it should contain
		// the BUFFPAGES and PAGESIZE lines, as described in the
		// first assignment!!
		Catalog (char* fName);
		Catalog();
		~Catalog ();

		void init(char* fName);
  
		// Go to the catalog in order to get information about the
		// specified database file (the DB file name is given by
		// fName).  This function reads in the schema for the file,
		// the type of the file (BTree, Sorted, etc.), the size of
		// the file, and the "starting position of the file"...
		// this will ususally be 0 for a heap or a sorted file, but
		// can be used to locate the root node in a BTree file
		void GetFileInfo (char *fName, Schema &mySchema,
					FileType &fType, int *&numDistincts, 
					int &numTuples, int &fSize, int &startPos);

		// Remove all traces of the specified file from the catalog
		void EraseFileInfo (char *fName);
    
		// Add information about the given file to the catalog
		void AddFileInfo (char *fName, Schema &mySchema,
					FileType &fType, int *&numDistincts,
					int &numTuples, int &fSize, int &startPos);
    
		// Returns the system page size
		int GetPageSize ();
    
		// Returns the number of buffer pages that the system has
		int GetNumBufPages ();
    
		// Get a pointer to all of the files described in the catalog
		// num will tell how many file names are returned
		char** GetAllFiles (int &num);

		// Tell whether or not the given file is in the catalog
		int IsInThere (char* fName);

};
    
#endif
