// Sample driver 

#include <iostream.h>
#include "Catalog.h"

//******************
//Change this def to switch the test case
//******************
#define TEST1

//******************

#ifdef TEST1

int main () {

	// create a new relation schema
	Attribute a1, a2, a3;
	a1.Init ("FirstAtt", Integer, sizeof (int));
	a2.Init ("SecAtt", Real, sizeof (double));
	a3.Init ("ThirdAtt", CharString, 50);
	Schema firstSchema ("MyFirstSchema");
	firstSchema.AddAtt (a1);
	firstSchema.AddAtt (a2);
	firstSchema.AddAtt (a3);
	firstSchema.SetKey("FirstAtt");
	// create a record that uses that schema
	Record myData1 (firstSchema);

	// add some values into him
	int num = 12;
	double dub = 9e99;
	char *string = "This is fun!!";
	myData1[0].InsertVal (&num);		
	myData1[1].InsertVal (&dub);		
	myData1[2].InsertVal (string);		
	
	// print him out 
	cout << myData1 << "\n";
}

#endif

#ifdef TEST2

int main () {

	// read in two schemas from the catalog
	Catalog myCat ("catalog");
	Schema one (""), two ("");
	FileType type1, type2;
	int fSize1, fSize2;
	int startPos1, startPos2;
	int* numDist1;
	int* numDist2;	
	int numTups1, numTups2;
	myCat.GetFileInfo ("myFile1", one, type1, numDist1, numTups1, fSize1, startPos1);
	myCat.GetFileInfo ("myFile2", two, type2, numDist2, numTups2, fSize2, startPos2);

	if (type1 == Heap) 
		cout << "myFile1 is a heap file.\n";
	
	if (type2 == Heap)
		cout << "myFile2 is a heap file.\n";

	Record recOne (one), recTwo (two);
	int val = 2;
	double dub = 4e23;
	char *str = "this is not my KEY!!";
	recOne[0].InsertVal (&val);
	recOne[1].InsertVal (&dub);
	recOne[2].InsertVal (str);
	recTwo[0].InsertVal (&val);
	recTwo[1].InsertVal (&dub);
	cout << recOne << "\n";
	cout << recTwo << "\n";

	if (recOne.Compare (recTwo) == 0)
		cout << "The two records have the same key value.\n";
}

#endif

