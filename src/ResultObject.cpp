#include "ResultObject.h"

ResultObject::ResultObject(){
	docid=0;
	valor=0.0;
}

ResultObject::ResultObject(unsigned int _docid, double _valor){
	docid=_docid;
	valor=_valor;
}

ResultObject::~ResultObject(){
}

//los siguientes codigos fueron movidos al .h para asegurar inline

//unsigned int ResultObject::getDocId(){
//	return docid;
//}

//double ResultObject::getScore(){
//	return valor;
//}

//void ResultObject::setDocId(unsigned int _docid){
//	docid=_docid;
//}

//void ResultObject::setScore(double _valor){
//	valor=_valor;
//}


