
#include "Wand.h"

//Los streams los recibire en el init
Wand::Wand(int _maxNumStreams){

//cout<<"Wand ("<<_maxNumStreams<<" max)\n";

	maxNumStreams = _maxNumStreams;
	numStreams = 0;
	
	streams = new InvertedList*[maxNumStreams];
	streamDocIds = new unsigned int[maxNumStreams];
	streamIndices = new int[maxNumStreams];
	streamIndicesAux = new int[maxNumStreams];
	
	arr_ubs = new double[maxNumStreams];
	
	streamComparator = StreamComparator(streamDocIds);
	
	currentDocId = END_OF_LIST;
	currentPayload = 0;
	
//	debug = false;
	
}

Wand::~Wand(){
//cout<<"~Wand - inicio\n";
	
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
	
	if(arr_ubs != NULL){
		delete [] arr_ubs;
	}
	
	currentDocId = END_OF_LIST;
	currentPayload = 0;
//cout<<"~Wand - fin\n";
}

void Wand::init(QueryObject *_query_terms){
//cout<<"Wand::init - inicio ("<<(_query_terms->size())<<" terms)\n";
	
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
	
	//Esto TIENE que ser definido en el init de cada wand
	for (int i=0; i<numStreams; ++i) {
		arr_ubs[i] = 0.0;
	}
	
//cout<<"Wand::init - fin\n";
}

unsigned int Wand::next(double threshold){
//if(debug) cout<<"Wand::next - inicio (threshold: "<<threshold<<")\n";
	
	// We do not check whether the stream is already at the end 
	// here based on the assumption that application won't call 
	// next() for streams that are already at the end, or atleast
	// won't do this frequently. 
	
	int pivotIdx=-1;
	unsigned int pivotDocId = END_OF_LIST;
	
	while (true) {
 		
 		if(numStreams==0){
//if(debug) cout<<"Wand::next - No quedan streams\n";
			currentPayload = 0;
			return currentDocId = END_OF_LIST;
 		}
 		
		if (!findPivotFeatureIdx(threshold, pivotIdx)) {
//if(debug) cout<<"Wand::next - ! findPivotFeatureIdx (EOS)\n";
			currentPayload = 0;
			return currentDocId = END_OF_LIST;
		}
		
//if(debug) cout<<"Wand::next - pivotIdx: "<<pivotIdx<<"\n";
		
		pivotDocId = streamDocIds[streamIndices[pivotIdx]];
		
//if(debug) cout<<"Wand::next - pivotDocId: "<<pivotDocId<<"\n";
		
		if (streamDocIds[streamIndices[0]] == streamDocIds[streamIndices[pivotIdx]]) {
//if(debug) cout<<"Wand::next - if 1... candidato encontrado, evaluando\n";
			
			// Found candidate. All cursors before streams[pivotIdx] point to
			// the same doc and this doc is the candidate for full evaluation.
			currentDocId = pivotDocId;
			
			// Advance pivotIdx sufficiently so that all instances of pivotDocId are included
			while (pivotIdx < numStreams-1 && streamDocIds[streamIndices[pivotIdx+1]] == pivotDocId) {
				++pivotIdx;
			}
			
			// scorer calculates the exact score for the docs [0, pivotIdx] 
			//scorer->scoreCurrentDoc(currentDocId, streamIndices, 0, pivotIdx, currentPayload);
			scoreCurrentDoc(0, pivotIdx);
			
//if(debug) cout<<"Wand::next - currentPayload: "<<currentPayload<<"\n";
			
			// advance all cursors preceding the pivot cursor and the pivot
			// cursor past the pivot docid
			moveStreamsAndSort(pivotIdx+1);
			
			// If the fully-evaluated score is higher then the threshold, then
			// currentDocId is a matching doc
			
			if (currentPayload > threshold) {
//				if (currentPayload >= threshold) 
//if(debug) cout<<"Wand::next - candidato encontrado (doc "<<currentDocId<<" por score "<<currentPayload<<")\n";
				return currentDocId;
			} //otherwise do nothing - let the while-loop continue
			else{
//if(debug) cout<<"Wand::next - Continuando busqueda\n";
			}
			
		}
		else { // not all cursors upto the pivot are aligned at the same doc yet
			// decreases pivotIdx to the first stream pointing at the pivotDocId
//if(debug) cout<<"Wand::next - else (no todos alineados)\n";
			while (pivotIdx && streamDocIds[streamIndices[pivotIdx-1]] == pivotDocId) {
				--pivotIdx;
			}
			
			moveStreamsToDocAndSort(pivotIdx, pivotDocId);
		}
		
	}  /* while (true) */
}

bool Wand::findPivotFeatureIdx(double threshold, int &pivotIdx){
//if(debug) cout<<"Wand::findPivotFeatureIdx - inicio (threshold "<<threshold<<")\n";
	sum_ub = 0;
	int idx;
	for (idx=0; sum_ub <= threshold && idx < numStreams; ++idx) {
		sum_ub += arr_ubs[ streamIndices[idx] ];
	}
	if( sum_ub > threshold ) {
		pivotIdx = idx - 1;
//if(debug) cout<<"Wand::findPivotFeatureIdx - encontrado (posicion "<<pivotIdx<<", sum_ub "<<sum_ub<<")\n";
		return true;
	}
//if(debug) cout<<"Wand::findPivotFeatureIdx - No encontrado (sum_ub "<<sum_ub<<")\n";
	return false; 
}

void Wand::moveStreamsAndSort(const int numStreamsToMove){
//cout<<"Wand::moveStreamsAndSort - inicio (numStreamsToMove "<<numStreamsToMove<<")\n";
	for (int i=0; i<numStreamsToMove; ++i) {
		streamDocIds[streamIndices[i]] = streams[streamIndices[i]]->next();
	}
	sortMerge(numStreamsToMove);
//cout<<"Wand::moveStreamsAndSort - fin\n";
}

void Wand::sortMerge(const int numStreamsToMove){
	for (int i=0; i<numStreamsToMove; ++i) {
		streamIndicesAux[i] = streamIndices[i];
	}
	sort(streamIndicesAux, streamIndicesAux+numStreamsToMove, streamComparator);
	
	//En aux solo se ordenaron desde 0 hasta numStreamsToMove
	//el codigo siuiente se asegura de dejar los indices finales en streamIndices
	int j=numStreamsToMove, k=0, i=0;
	while (i < numStreamsToMove && j < numStreams) {
		if (streamComparator(streamIndicesAux[i], streamIndices[j])) {
			streamIndices[k++] = streamIndicesAux[i++];
		}
		else {
			streamIndices[k++] = streamIndices[j++];
		}
	}
	if (j == numStreams) {
		while (i < numStreamsToMove) {
			streamIndices[k++] = streamIndicesAux[i++];
		}
	}
	//eliminar listas agotadas
	while (numStreams && streamDocIds[streamIndices[numStreams-1]] == END_OF_LIST) {
		--numStreams;
	}
}

void Wand::moveStreamsToDocAndSort(const int numStreamsToMove, const int desiredDocId){
	for(int i=0; i<numStreamsToMove; ++i) {
		streamDocIds[streamIndices[i]] = streams[streamIndices[i]]->next(desiredDocId);
	}
	sortMerge(numStreamsToMove);
}

void Wand::scoreCurrentDoc(int startIdx, int endIdx){
//cout<<"scoreCurrentDoc - doc "<<currentDocId<<": 0\n";
	currentPayload = 0;
	return;
}

unsigned int Wand::log2bits(unsigned int num){
	unsigned int ret=1;
	while( num>>=1 ){
		++ret;
	}
	return ret;
}

unsigned int Wand::getDocId(){
	return currentDocId;
}

double Wand::getScore(){
	return currentPayload;
}


