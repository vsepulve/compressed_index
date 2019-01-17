#include "VarByteList.h"

VarByteList::VarByteList(){
	n_docs = 0;
	data = NULL;
	//datos para el next
	cur_index = 0;
	cur_doc = 0;
	data_size = 0;
	//datos para la info de bloques
	num_blocks = 0;
	cur_block_pos = 0;
	bloks = NULL;
	cur_pos = -1;
}

VarByteList::VarByteList(list<unsigned int> *original_list){
	unsigned int *arr = new unsigned int [original_list->size()];
	unsigned int i = 0 ; 
	for(list<unsigned int>::iterator it = original_list->begin(); it !=original_list->end(); it++){
		arr[i++] = (*it);
	}
	this->compress(arr, original_list->size());
	delete [] arr;
}

VarByteList::VarByteList(unsigned int *arr, unsigned int arr_size){
	this->compress(arr, arr_size);
}

unsigned int VarByteList::nbytes(unsigned int num){
	//Forma eficiente de calcular el numero de bytes, por el numero de bits
	//Si usa menos de 8, 15, 22, o 29 bits (porque se permiten 7 de cada byte)
	if(num < 0x80) return 1;
	else if(num < 0x4000) return 2;
	else if(num < 0x200000) return 3;
	else if(num < 0x10000000) return 4;
	else return 5;
}
	
void VarByteList::compress(unsigned int *arr, unsigned int arr_size){
	cur_pos = -1;
	if(arr_size == 0){
		cout<<"VarByteList::compress - ERROR (sin documentos)\n";
		exit(-1);
	}
	//stores the numbers of docs
	n_docs = arr_size;
	data = NULL;
	data_size = 0;
	//data for next
	cur_index = 0;
	cur_doc = 0;
	//data for block structur
	num_blocks = 0;
	cur_block_pos = 0;
	bloks = NULL;
	//calculating num_block
	num_blocks = (n_docs/DOCS_PER_BLOCK)+1;
	bloks = new unsigned long long [num_blocks];
//	cout<<"num_blocks = "<<num_blocks<<"\n";
	
	//building compresed data stream
	
	//calculating the size of the compresed data stream
	int largo_total=0;
	for(unsigned int i=0; i<arr_size; ++i){
//		int largo_actual = ceil((log2((arr[i]+1)))/7);
//		largo_total += largo_actual;
		largo_total += nbytes(arr[i]);
	}
	//stores the size of the compresed data stream in bytes 
	data_size=largo_total;
	//reserves memory for the compresed data stream
	data=new unsigned char[data_size];
	//continuator mask
	unsigned char continuar=128;
	//end mask
	unsigned char end = 127;
	
	//comienzo el llenado
	
	unsigned int indice_global=0;
	unsigned int contador_num_leidos = 0;
	unsigned long long acumulado = 0 ;
	unsigned long long num_bloks_used=0;
	for(unsigned int i =0; i<arr_size; ++i){
		if(i==arr_size){
			break;
		}
//		int largo_bytes= ceil((log2((arr[i]+1)))/7);	
		int largo_bytes= nbytes(arr[i]);
		unsigned char a_colocar;
		acumulado += arr[i];
		contador_num_leidos++;
		switch (largo_bytes){
			case 1:	
				a_colocar=(arr[i]&end);
				data[indice_global++]=a_colocar;
				if((contador_num_leidos % DOCS_PER_BLOCK) == 0){
					//escribir en bloque [pos | numero]
					unsigned long long pos_block = indice_global-1;
					pos_block<<=32;
					unsigned int numero = acumulado;
					bloks[num_bloks_used] |= numero;
					bloks[num_bloks_used] |= pos_block;
					num_bloks_used++;
				}
				break;			
			case 2:
				a_colocar=((arr[i]&16256)>>7);
				a_colocar=a_colocar|continuar;
				data[indice_global++]=a_colocar;
				a_colocar=(arr[i]&end);
				data[indice_global++]=a_colocar;
				if((contador_num_leidos % DOCS_PER_BLOCK) == 0){
					//escribir en bloque [pos | numero]
					unsigned long long pos_block = indice_global-1;
					pos_block<<=32;
					unsigned int numero = acumulado;
					bloks[num_bloks_used] |= numero;
					bloks[num_bloks_used] |= pos_block;
					num_bloks_used++;
				}
				break;
			case 3:
				a_colocar=((arr[i]&2080768)>>14);
				a_colocar=a_colocar|continuar;
				data[indice_global++]=a_colocar;
				a_colocar=((arr[i]&16256)>>7);
				a_colocar=a_colocar|continuar;
				data[indice_global++]=a_colocar;
				a_colocar=(arr[i]&end);
				data[indice_global++]=a_colocar;
				if((contador_num_leidos % DOCS_PER_BLOCK) == 0){
					//escribir en bloque [pos | numero]
					unsigned long long pos_block = indice_global-1;
					pos_block<<=32;
					unsigned int numero = acumulado;
					bloks[num_bloks_used] |= numero;
					bloks[num_bloks_used] |= pos_block;
					num_bloks_used++;
				}
				break;
			case 4:
				a_colocar=((arr[i]&266338304)>>21);
				a_colocar=a_colocar|continuar;
				data[indice_global++]=a_colocar;
				a_colocar=((arr[i]&2080768)>>14);
				a_colocar=a_colocar|continuar;
				data[indice_global++]=a_colocar;
				a_colocar=((arr[i]&16256)>>7);
				a_colocar=a_colocar|continuar;
				data[indice_global++]=a_colocar;
				a_colocar=(arr[i]&end);
				data[indice_global++]=a_colocar;
				if((contador_num_leidos % DOCS_PER_BLOCK) == 0){
					//escribir en bloque [pos | numero]
					unsigned long long pos_block = indice_global-1;
					pos_block<<=32;
					unsigned int numero = acumulado;
					bloks[num_bloks_used] |= numero;
					bloks[num_bloks_used] |= pos_block;
					num_bloks_used++;
				}
				break;
		}
	}
	//building the last block
	unsigned long long pos_block = indice_global-1;
	pos_block<<=32;
	unsigned int numero = acumulado;
	bloks[num_bloks_used] |= numero;
	bloks[num_bloks_used] |= pos_block;

}
	
VarByteList::~VarByteList(){
	if(data != NULL){
		delete [] data;
		data = NULL;
	}
	if(bloks != NULL){
		delete [] bloks;
		data = NULL;
	}
	n_docs = 0;
	cur_index = 0;
	cur_doc = 0;
	data_size = 0;
	num_blocks = 0;
	cur_block_pos = 0;
	cur_pos = -1;
}

//deja la lista en el primer documento
//notar que una lista puede no tener next, y aun estar en una posicion valida (la ultima)
void VarByteList::reset(){
	cur_index = 0;
	cur_doc = 0;
	cur_block_pos = 0;
	cur_pos = -1;
	cur_doc = this->next();
	if(payload_list != NULL){
		payload_list->reset();
	}
}

bool VarByteList::hasNext(){
//	cout<<"\nvarByteList::hasNext() - \n";
	return (data_size > cur_index);
}

unsigned int VarByteList::next(){
//	cout<<"VarByteList::next - cur_index: "<<cur_index<<", data_size: "<<data_size<<" \n";
	if(cur_index==data_size){
		return cur_doc=END_OF_LIST;
	}
	unsigned int res = 0;
	
	for(; cur_index<data_size; ++cur_index){
		if(data[cur_index]>=128){
			res|=(data[cur_index]&127);
			res<<=7;
		}
		else{					
			res|=data[cur_index];
			break;
		}
	}
	++cur_index;
	
	cur_doc += res; 
	if(cur_doc > (unsigned int)(bloks[cur_block_pos])){
		++cur_block_pos;
	}
	++cur_pos;
//	cout<<"VarByteList::next - retornando "<<cur_doc<<", cur_pos: "<<cur_pos<<"\n";
	return cur_doc ;
}

unsigned int VarByteList::next(unsigned int desired){
	
	//preguntar si es menor que current
//	cout<<"["<<term_id<<"] VarByteList::next - desired: "<<desired<<", cur_doc: "<<cur_doc<<"\n";
	if(desired <= cur_doc){
//		cout<<"VarByteList::next - desired <= cur_doc\n";
		//si es si retornar cur_doc y no hacer nada más;
		return cur_doc;
	}
	else{
//		cout<<"VarByteList::next - desired > cur_doc\n";
		//si no buscar en el bloque donde debiera estar
		if(this->block_pos(desired) > num_blocks){
//			cout<<"VarByteList::next - desired > ultimo doc";
			cur_index = data_size;
			return cur_doc = END_OF_LIST;
		}
		//pregunto si el desired es el ultimo elemento del bloque
		if(desired == (unsigned int)(bloks[cur_block_pos])){
//			cout<<"VarByteList::next - desired = ultimo de bloque\n";
			cur_index = (unsigned int)(bloks[cur_block_pos]>>32)+1;
			
//			cur_pos = ((cur_block_pos+1)*DOCS_PER_BLOCK );
			if(cur_block_pos == num_blocks-1){
				cur_pos = size()-1;
			}
			else{
				cur_pos = ((cur_block_pos+1)*DOCS_PER_BLOCK );
			}
			
			return cur_doc = desired;
		}
		else{
			//si no descomprimir el bloque hasta que se encuentre el número o se mayor
			for(this->next(); cur_doc < desired; this->next());
//			cout<<"VarByteList::next - ["<<term_id<<"] normal (cur_doc: "<<cur_doc<<", cur_pos: "<<cur_pos<<")\n";
			return cur_doc;
		}
	}
	
}

unsigned int VarByteList::block_pos(unsigned int desired){
//	cout<<"VarByteList::block_pos - desired: "<<desired<<", cur_block_pos = "<<cur_block_pos<<"\n";
	if(desired <= (unsigned int)(bloks[cur_block_pos])){
//		cout<<"\tVarByteList::block_pos - desired menor que max de bloque\n";
		return cur_block_pos;
	}
	else if(desired > (unsigned int)(bloks[num_blocks-1])){
		return num_blocks+1;
	}
	else{
		//busque el bloque
		//this search could be done in complexity O(log(n)), but for practical experiencie we gonna do it in O(n).
		for(unsigned int i = cur_block_pos +1 ; cur_block_pos < num_blocks; i++){
			if(desired <= (unsigned int)(bloks[i])){
				cur_block_pos = i;
				//setear cur_index para que quede correctamente  
				//para esto vamos a ocupar una casualidad que ocurre aquí, para que la pasicion de cur_index esta al comienzo del bloque actual
				//en bloks se guarda [pos | numero], la pos es la posicion de donde esta el último char del último numero del bloque, por lo que pos+1
				//es la posicion del primer char del primer numero del siguinete bloque, 
				//por lo que cur_index es pos del bloque anterior +1, esto podria fallar si estamos en el primer bloque, pero eso jamas puede pasar
				//pues si eso pasase nunca deberia entrar en este if
//				cout<<"VarByteList::block_pos("<<desired<<") : retornando: cur_block_pos = "<<cur_block_pos<<"\n";
				cur_index = (unsigned int)(bloks[cur_block_pos-1]>>32)+1;
				cur_doc = (unsigned int)(bloks[cur_block_pos-1]);
				cur_pos = ((cur_block_pos)*DOCS_PER_BLOCK )-1;
				
				return i;
			}
		}		
	}
//	cout<<"\tVarByteList::block_pos - retornando cur_block_pos: "<<cur_block_pos<<"\n";
	return cur_block_pos;
}


unsigned int VarByteList::size() const {
	return n_docs;
}

unsigned int VarByteList::getDocId() const {
	return cur_doc ;
}

unsigned int VarByteList::uncompress(unsigned int *buff){
	
//	memset(buff, 0, n_docs*sizeof(int));
	
//	//Implementacion original
//	unsigned int indice = 0;
//	for(unsigned int i=0; i<data_size; ++i){
//		if(data[i]>=128){
//			buff[indice] |= (data[i]&127);
//			buff[indice] <<=7;
//		}
//		else{
//			buff[indice] |= data[i];
//			++indice;
//		}
//	}
//	return indice;
	
	unsigned int indice = 0;
	
	if(data_size<4){
		for(unsigned int i=0 ; i<data_size; ++i){
			if(data[i]>=128){
				buff[indice] |= (data[i]&127);
				buff[indice] <<=7;
			}
			else{
				buff[indice] |= data[i];
				++indice;
			}
		}
	}
	else{
		unsigned int i;
		for(i=0; i<data_size-3; ++i){
			if( !buff[indice] && (((unsigned int*)(&(data[i])))[0] & 0x80808080) == 0 ){
				buff[indice] = data[i];
				buff[indice+1] = data[i+1];
				buff[indice+2] = data[i+2];
				buff[indice+3] = data[i+3];
				indice += 4;
				i += 3;
			}
			else if(data[i]<128){
				buff[indice] |= data[i];
				++indice;
			}
			else{
				buff[indice] |= (data[i]&127);
				buff[indice] <<=7;
			}
		
		}
		for( ; i<data_size; ++i){
			if(data[i]>=128){
				buff[indice] |= (data[i]&127);
				buff[indice] <<=7;
			}
			else{
				buff[indice] |= data[i];
				++indice;
			}
		}
	}
	
//	for(unsigned int i=1; i<n_docs; ++i){
//		buff[i] += buff[i-1];
//	}
	
	return indice;
}

void VarByteList::save(fstream *writer){

	if (!(writer->is_open())){
		cerr<<"Error al escribir la Lista, el archivo no esta abierto\n";
		exit(-1);
	}
	else{
		//write n_docs;
		writer->write((char*)(&n_docs),sizeof(int));
		//write data_size
		writer->write((char*)(&data_size),sizeof(int));
		//write data
		writer->write((char*)(data),(sizeof(char)*data_size));
		
		//writing blocks data
		//write num_blocks
		writer->write((char*)(&num_blocks),sizeof(int));
		//write blocks data
		writer->write((char*)(bloks),(sizeof(long long)*num_blocks));
		
		//we do not write the next insformation , runs insformation or blocks insformation because this insformation is reseted in the load function

	}
}

void VarByteList::load(fstream *reader){
	if (!(reader->is_open())){
		cerr<<"\nError al escribir la Lista, el archivo no esta abierto\n";
		exit(-1);
	}	
	else{
		//reading number of docs
		n_docs = 0;
		reader->read ((char*)(&n_docs),sizeof(int));
		//reading data size
		data_size = 0 ;
		reader->read((char*)(&data_size),sizeof(int));
		
		//reserving memory for the compressed data
		if(data != NULL){
			delete [] data;
		}
		data = new unsigned char[data_size];
		//reading compressed data
		reader->read((char *)data,sizeof(char)*data_size);
		
		
		//reading block information
		num_blocks = 0;
		reader->read ((char*)(&num_blocks),sizeof(int));
		//reserving the memory for the blocks data
		if(bloks != NULL){
			delete [] bloks;
		}
		bloks = new unsigned long long[num_blocks];
		//reading the blocks data
		reader->read((char*)(bloks),sizeof(long long)*num_blocks);
		
		//setting the rest of the variables
		cur_index = 0 ;
		cur_doc = 0;
		//data for blocks
		cur_block_pos = 0;
		//starting
		cur_doc = this->next();			
	}	
}

//retorna el numero de bytes para almacenar los datos
//No se incluye el numero de docs, ni el tipo de compresion, ni la estructura de bloques
unsigned int VarByteList::getSizeData() const {
	return data_size + sizeof(int);
}

//retorna el numero de bytes para almacenar la estructura de bloques
//se incluye todo lo necesario para la estructura de bloques (incluido el tamaño de la estructura)
unsigned int VarByteList::getSizeBlock() const {
	return (num_blocks*sizeof(long long))+sizeof(int) ;
}

	
	
	
