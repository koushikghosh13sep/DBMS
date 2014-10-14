#include "RecordInfo.h"

RecordInfo :: RecordInfo() { /*R = new Record();*/ }
RecordInfo :: ~RecordInfo() {
    delete R;
}

RecordInfo :: RecordInfo(Schema &sc, Record &me, int pos) {
	
	char* where;
	where = new char [me.GetNumBytes()];
	
	R = new Record(sc); 
	position = pos;

        cout<<"\natt : "<<sc.NumAtts()<<"GetNum Bytes = me :"<<me.GetNumBytes();
	cout<<"R :"<<R->GetNumBytes()<<endl;
        me.ToBinary(where);
        cout<<"\nB R FrmBin\n";
        R->FromBinary(where);
        cout<<"\nA R FrmBin\n";
        delete where;
	cout << endl;
	cout << "\nIN CONST\n";	
}

RecordInfo :: RecordInfo(Schema &sc, char *where, int pos) {
	R = new Record(sc); 

        position = pos;
        
        R->FromBinary(where);
}

Record& RecordInfo :: GetRecord(){return *R;}

int RecordInfo :: GetPosition() {return position; }

bool RecordInfo :: operator>(const RecordInfo* me) const {

// JHAR KHETE PARE.......... BEWARE........!!!!!!!!!!!!

//	cout<<endl<<"Record :: Operator> B r1"<<endl; 
	RecordInfo *r1 = new RecordInfo();
//	cout<<endl<<"Record :: Operator> B r2"<<endl;
	RecordInfo *r2 = new RecordInfo();
//	cout<<endl<<"Record :: Operator> A r2"<<endl;
	r1 = const_cast <RecordInfo *> (this);
	r2 = const_cast <RecordInfo *> (me);
	cout << "Key of r1 : " << (r1 -> R)->operator[](0) << endl; 
	cout << "Key of r2 : " << (r2 -> R)->operator[](0) << endl; 
	cout << "Comp : " << (r1->R)->Compare (*(r2->R));
	
	if ((r1->R)->Compare (*(r2->R)) > 0) {
		cout<<endl<<"Record :: Return True"<<endl;
		return true;
	}
	else {
		cout<<endl<<"Record :: Return False"<<endl;
		return false;
	}

			
}


