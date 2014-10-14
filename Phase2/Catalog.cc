#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include "Catalog.h"

using namespace std;
  
//********************************************************   
int GetLine (char *inMe, FILE *fromMe) {
	for (int i = 0; 1; i++) {
		inMe[i] = getc (fromMe);
		if (inMe[i] == '\n') {
			inMe[i] = 0;
			return 1;
		}
		else if (inMe[i] == EOF) {
			inMe[i] = 0;
			return 0;
		}
	}
}
    
//********************************************************   
Catalog :: Catalog (char *fName) {
	strName = strdup (fName);
	FILE *catalog = fopen (strName, "r");
	if (catalog == NULL) {
		cerr << "Can't open catalog file.\n";
		return;
	}

	char space[200];
	while (GetLine (space, catalog)) {
		if (!strncmp ("PAGESIZE", space, strlen ("PAGESIZE"))) {
			sscanf (&(space[8]), "%d", &pageSize);
		} else if (!strncmp ("BUFFPAGES", space, strlen ("BUFFPAGES"))) {
			sscanf (&(space[9]), "%d", &numPages);
		}
	}
}
    
//********************************************************   
Catalog :: Catalog() {
}
  
//********************************************************   
Catalog :: ~Catalog () {
	delete strName;
}
    
//********************************************************   
void Catalog::init(char* fName) {
	strName = strdup (fName);
	FILE *catalog = fopen (strName, "r");
	if (catalog == NULL) {
		cerr << "Can't open catalog file.\n";
		return;
	}

	char space[200];
	while (GetLine (space, catalog)) {
		if (!strncmp ("PAGESIZE", space, strlen ("PAGESIZE"))) {
			sscanf (&(space[8]), "%d", &pageSize);
		} else if (!strncmp ("BUFFPAGES", space, strlen ("BUFFPAGES"))) {
			sscanf (&(space[9]), "%d", &numPages);
		}
	}
}
  
//********************************************************   
void Catalog :: GetFileInfo (char* fName, Schema &mySchema,
	FileType &fType, int *&numDistincts, int &numTuples,
	int &fSize, int &startPos) {

	// find the right info
	mySchema.Clear ();
	char space[200];
	FILE *catalog = fopen (strName, "r");
	while (GetLine (space, catalog)) {
		if (!strncmp ("BEGIN FILE", space, strlen ("BEGIN FILE"))) {
			GetLine (space, catalog);
			char space2[200];
			sscanf (space, "%s", space2);	
			if (!strcmp (fName, space2)) {
				break;
			}
		}
	}

	// now read it in	
	mySchema.Clear ();
	fscanf (catalog, "%s", space);
	mySchema.AddName (space);

	// read in the attributes
	int numAtts;
	fscanf (catalog, "%d", &numAtts);
	for (int i = 0; i < numAtts; i++) {
		fscanf (catalog, "%s", space);
		Attribute a;
		if (!strcmp (space, "INTEGER")) {
			fscanf (catalog, "%s", space);
			a.Init (space, Integer, sizeof (int));
		} else if (!strcmp (space, "REAL")) {
			fscanf (catalog, "%s", space);
			a.Init (space, Real, sizeof (double));
		} else {
			int numBytes;
			fscanf (catalog, "%d", &numBytes);
			fscanf (catalog, "%s", space);
			a.Init (space, CharString, numBytes);
		}
		mySchema.AddAtt (a);
	}

	// read in the key
	fscanf (catalog, "%s", space);
	fscanf (catalog, "%s", space);
	mySchema.key = mySchema.GetIndex (space);

	// read in the value counts
	numDistincts = new int[numAtts];
	for (int i = 0; i < numAtts; i++) {
		fscanf (catalog, "%d", &(numDistincts[i]));
	}

	// read in the number of tuples
	fscanf (catalog, "%d", &numTuples);

	// read in the file size
	fscanf (catalog, "%d", &fSize);

	// read in the starting block
	fscanf (catalog, "%d", &startPos);

	// read in the file type
	fscanf (catalog, "%s", space);
	if (!strcmp (space, "SORTED")) {
		fType = Sorted;
	} else if (!strcmp (space, "HEAP")) {
		fType = Heap;	
	} else {
		fType = BTree;
	}

	fclose (catalog);
}

//********************************************************   
void Catalog :: EraseFileInfo (char *fName) {

	FILE *catalog = fopen (strName, "r");
	FILE *output = fopen ("asdasd", "w");
	char space[200];

	while (GetLine (space, catalog)) {
		if (!strncmp ("BEGIN FILE", space, strlen ("BEGIN FILE"))) {
			GetLine (space, catalog);
			if (!strcmp (fName, space)) {
				while (GetLine (space, catalog)) {
					if (!strncmp ("END FILE", space, strlen ("END FILE"))) {
						break;
					}
				}
			} else {
				fprintf (output, "BEGIN FILE\n%s\n", space);
			}
		} else {
			fprintf (output, "%s\n", space);
		}
	}
	
	fclose (catalog);
	fclose (output);
	sprintf (space, "cp asdasd %s", strName);
	system (space);
	sprintf (space, "rm  %s", "asdasd");
	system (space);
}

//********************************************************   
void Catalog :: AddFileInfo (char *fName, Schema &mySchema,
	FileType &fType, int *&numDistincts, int &numTuples, 
	int &fSize, int &startPos) {

	FILE *catalog = fopen (strName, "a");
	fprintf (catalog, "\nBEGIN FILE\n%s\n%s\n%d\n", fName, 
	mySchema.GetName (), mySchema.NumAtts ());
	mySchema.FPrintf (catalog);
	for (int i = 0; i < mySchema.numAtts; i++) {
		fprintf (catalog, "%d\n", numDistincts[i]);
	}
	fprintf (catalog, "%d\n%d\n%d\n", numTuples, fSize, startPos);
	if (fType == Heap) {
		fprintf (catalog, "HEAP\nEND FILE\n");
	} else if (fType == Sorted) {
		fprintf (catalog, "SORTED\nEND FILE\n");
	} else {
		fprintf (catalog, "BTREE\nEND FILE\n");
	}
	fclose (catalog);
}
    
//********************************************************   
int Catalog :: GetPageSize () {
	return pageSize;
}
     
//********************************************************   
int Catalog :: GetNumBufPages () {
	return numPages;
}
    
//********************************************************   
char ** Catalog :: GetAllFiles (int &num) {
    
	num = 0;
    
	// find the right info
	FILE *catalog = fopen (strName, "r");
	char space[200];
	while (GetLine (space, catalog)) {
		if (!strncmp ("BEGIN FILE", space, strlen ("BEGIN FILE"))) {
			num++;
		}
	}

	char **returnVal = new char *[num];

	fclose (catalog);
	catalog = fopen (strName, "r");
	num = 0;
	while (GetLine (space, catalog)) {
		if (!strncmp ("BEGIN FILE", space, strlen ("BEGIN FILE"))) {
			GetLine (space, catalog);
			char space2[200];
			sscanf (space, "%s", space2);	
			returnVal[num] = strdup (space2);	
			num++;
		}
	}

	fclose (catalog);
	return returnVal;
}

//********************************************************   
int Catalog :: IsInThere (char *fName) {

	// find the right info
	FILE *catalog = fopen (strName, "r");
	char space[200];
	while (GetLine (space, catalog)) {
		if (!strncmp ("BEGIN FILE", space, strlen ("BEGIN FILE"))) {
			GetLine (space, catalog);
			char space2[200];
			sscanf (space, "%s", space2);	
			if (!strcmp (fName, space2)) {
				fclose (catalog);
				return 1;
			}
		}
	}

	fclose (catalog);
	return 0;
}

