
#include "WandTfidf.h"

//Los streams los recibire en el init
WandTfidf::WandTfidf(int _maxNumStreams){

//cout<<"WandTfidf ("<<_maxNumStreams<<" max)\n";

//	maxNumStreams=_maxNumStreams;
//	numStreams=0;
//	
//	streams=new InvertedList*[maxNumStreams];
//	streamDocIds=new unsigned int[maxNumStreams];
//	streamIndices=new int[maxNumStreams];
//	streamIndicesAux=new int[maxNumStreams];
//	
//	streamComparator=StreamComparator(streamDocIds);
//	
//	currentDocId=END_OF_LIST;
//	currentPayload=0;
}

WandTfidf::~WandTfidf(){
//cout<<"~WandTfidf - inicio\n";
	
	if(streams!=NULL){
		delete [] streams;
		streams=NULL;
	}
	
	if(streamDocIds!=NULL){
		delete [] streamDocIds;
		streamDocIds=NULL;
	}
	
	if(streamIndices!=NULL){
		delete [] streamIndices;
		streamIndices=NULL;
	}
	
	if(streamIndicesAux!=NULL){
		delete [] streamIndicesAux;
		streamIndicesAux=NULL;
	}
	
	currentDocId = END_OF_LIST;
	currentPayload = 0;
//cout<<"~WandTfidf - fin\n";
}

void WandTfidf::init(QueryObject *_query_terms){
	
//cout<<"WandTfidf::init - inicio ("<<(_query_terms->size())<<" terms)\n";
	
	numStreams = _query_terms->size();
	if (numStreams == 0) {
		currentDocId = END_OF_LIST;
		currentPayload = 0;
	}
	
	//los indices parten en su posicion natural (0.. n-1)
	//tambien preparo los streams
	query_terms = _query_terms;
	for (int i=0; i<numStreams; ++i) {
		streamIndices[i] = i;
		streams[i] = query_terms->getList(i);
		streams[i]->reset();
		streamDocIds[i] = streams[i]->getDocId();
	}

	sort(streamIndices, streamIndices+numStreams, streamComparator);
	
	for (int i=0; i<numStreams; ++i) {
		arr_ubs[i] = (double)( log2bits( streams[streamIndices[i]]->getMaxPayload() ) )
					* query_terms->getWeight(streamIndices[i])
					* query_terms->getIdf(streamIndices[i]);
	}
	
//cout<<"WandTfidf::init - fin\n";
	
}

void WandTfidf::scoreCurrentDoc(int startIdx, int endIdx){
//cout<<"WandTfidf::scoreCurrentDoc - doc "<<currentDocId<<": ";
	currentPayload = 0;
	for (int i = startIdx; i <= endIdx; ++i) {
		currentPayload += (float)( log2bits( streams[streamIndices[i]]->getPayload() ) ) 
						* query_terms->getWeight(streamIndices[i])
						* query_terms->getIdf(streamIndices[i]);
//cout<<""<<currentPayload<<" ";
	}
//cout<<"\n";
	return;
}
	
//double WandTfidf::getScore(){
//	return (double)currentPayload;
//}



