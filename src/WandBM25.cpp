#include "WandBM25.h"

//Los streams los recibire en el init
WandBM25::WandBM25(map<unsigned int, unsigned int> *mapa_docs, map<unsigned int, float> *_mapa_ubs, float k1, float b, int _maxNumStreams){

//cout<<"WandBM25 - inicio ("<<_maxNumStreams<<" max, k1: "<<k1<<", b: "<<b<<", "<<mapa_docs->size()<<" docs)\n";
	
	mapa_ubs = _mapa_ubs;
//	bm25_docs_map = new unordered_map<unsigned int, float>();
	
	bm25_k1 = k1+1;
	map<unsigned int, unsigned int>::iterator it_mapa_docs;
	long double largo_medio=0;
	unsigned int docid, largo;
	unsigned int max_docid = 0;
	for(it_mapa_docs=mapa_docs->begin(); it_mapa_docs!=mapa_docs->end(); it_mapa_docs++){
		docid = it_mapa_docs->first;
		largo = it_mapa_docs->second;
		largo_medio += largo;
		if(docid > max_docid){
			max_docid = docid;
		}
	}
	largo_medio /= mapa_docs->size();
	bm25_docs_map = new float[max_docid + 1];
	memset(bm25_docs_map, 0, (max_docid + 1)*sizeof(float));
//cout<<"WandBM25 - largo_medio: "<<largo_medio<<"\n";
	
	for(it_mapa_docs=mapa_docs->begin(); it_mapa_docs!=mapa_docs->end(); it_mapa_docs++){
		docid = it_mapa_docs->first;
		largo = it_mapa_docs->second;
		//k1*(1-b+b*(largo_doc/largo_medio))
		bm25_docs_map[docid] = k1 * ( 1.0-b + b*((float)largo/largo_medio) );
	}
	
//cout<<"WandBM25 - fin\n";
	
}

//WandBM25::WandBM25(unordered_map<unsigned int, float> *_bm25_docs_map, map<unsigned int, unsigned int> *mapa_docs, map<unsigned int, float> *_mapa_ubs){
WandBM25::WandBM25(float *_bm25_docs_map, map<unsigned int, unsigned int> *mapa_docs, map<unsigned int, float> *_mapa_ubs){
	
//	float k1 = 1.2, float b = 0.75, int _maxNumStreams = 128

	float k1 = 1.2;
//	float b = 0.75;
//	int _maxNumStreams = 128;
	
//cout<<"WandBM25 - inicio ("<<_maxNumStreams<<" max, k1: "<<k1<<", b: "<<b<<", "<<mapa_docs->size()<<" docs)\n";
	
	mapa_ubs = _mapa_ubs;
	bm25_docs_map = _bm25_docs_map;
	
	bm25_k1 = k1+1;
//	map<unsigned int, unsigned int>::iterator it_mapa_docs;
//	long double largo_medio=0;
//	unsigned int docid, largo;
//	for(it_mapa_docs=mapa_docs->begin(); it_mapa_docs!=mapa_docs->end(); it_mapa_docs++){
//		docid = it_mapa_docs->first;
//		largo = it_mapa_docs->second;
//		largo_medio += largo;
//	}
//	largo_medio /= mapa_docs->size();
//cout<<"WandBM25 - largo_medio: "<<largo_medio<<"\n";
	
//	for(it_mapa_docs=mapa_docs->begin(); it_mapa_docs!=mapa_docs->end(); it_mapa_docs++){
//		docid = it_mapa_docs->first;
//		largo = it_mapa_docs->second;
//		//k1*(1-b+b*(largo_doc/largo_medio))
//		bm25_docs_map[docid] = k1 * ( 1.0-b + b*((float)largo/largo_medio) );
//	}
	
//cout<<"WandBM25 - fin\n";
	
}

WandBM25::~WandBM25(){
//cout<<"~WandBM25 - inicio\n";
	
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
	
	currentDocId=END_OF_LIST;
	currentPayload=0;
//cout<<"~WandBM25 - fin\n";
}

void WandBM25::init(QueryObject *_query_terms){
	
//cout<<"WandBM25::init - inicio ("<<(_query_terms->size())<<" terms)\n";
	
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
		
//cout<<"WandBM25::init - Lista "<<streams[i]->getDocId()<<" de largo "<<streams[i]->size()<<"\n";

	}
	
	sort(streamIndices, streamIndices+numStreams, streamComparator);
	
	//calcular los ubs
	//En el caso de BM25 se necesitan datos adicionales a lo usual calcular los ubs
	//por lo que se tiene un mapa de ubs que se deberia recibir en el constructor
	if(mapa_ubs == NULL){
		cerr<<"WandBM25::init - Mapa de UBs nulo\n";
	}
	else{
		for (int i=0; i<numStreams; ++i) {
			arr_ubs[i] = (*mapa_ubs)[ query_terms->getTermId(i) ];
		}
	}
	
//cout<<"WandBM25::init - fin\n";
	
}

void WandBM25::scoreCurrentDoc(int startIdx, int endIdx){
//if(debug) cout<<"WandBM25::scoreCurrentDoc - doc "<<currentDocId<<": ";
//(*escritor)<<"doc "<<currentDocId<<": ";

	currentPayload = 0;
//	double s = 0;
//	double p = 0;
	unsigned int log2payload;
	
//	double f1, f2, f3, f4;
	
	for (int i = startIdx; i <= endIdx; ++i) {
		
		//Okapi-BM25
		//idf * (frec * (k1+1) ) / (frec + k1*(1-b+b*(largo_doc/largo_medio)) )
		//              |-pre-|           |---------------pre---------------|
		//idf * (log2_payload * bm25_k1) / ( log2_payload + bm25_docs_map[currentDocId] )
		
		log2payload = log2bits( streams[streamIndices[i]]->getPayload() );

		currentPayload += query_terms->getWeight(streamIndices[i])
			* query_terms->getIdf(streamIndices[i])
			* ( log2payload * bm25_k1 )
			/ ( log2payload + bm25_docs_map[currentDocId]) ;
			
//		f1 = query_terms->getWeight(streamIndices[i]);
//		f2 = query_terms->getIdf(streamIndices[i]);
//		f3 = ( log2payload * bm25_k1 );
//		f4 = ( log2payload + bm25_docs_map[currentDocId]) ;
//		p = f1 * f2 * f3 / f4 ;
		
//		s += p;
		
//cout<<" + "<<p<<" ";
//(*escritor)<<" + "<<p<<" ";
//(*escritor)<<" + [ "<<f1<<" * "<<f2<<" * "<<f3<<" / "<<f4<<" payload "<<streams[streamIndices[i]]->getPayload()<<" term "<<streams[streamIndices[i]]->getTermId()<<" parte "<<(streams[streamIndices[i]])<<" ] ";
//(*escritor)<<dec<<"";
		
//if(debug) cout<<" + "<<((*query_terms)[streamIndices[i]]->getWeight()
//			* (*query_terms)[streamIndices[i]]->getIdf()
//			* ( log2payload * bm25_k1 )
//			/ ( log2payload + bm25_docs_map[currentDocId]))<<" ";

	}
	
//	currentPayload = s;
//if(debug) cout<<" = "<<currentPayload<<"\n";
//cout<<" = "<<currentPayload<<"\n";
//(*escritor)<<" = "<<currentPayload<<"\n";

	return;
}

