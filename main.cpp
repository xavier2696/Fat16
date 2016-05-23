#include <iostream> 
#include <fstream>
#include <ctime>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <math.h>   
#include <vector>

using namespace std;

unsigned short clusterDirectorioActual = 65467;
bool actualEsRoot = true;
//string rutaActual = "root";
vector<string> rutaActual;

vector<unsigned short> ClusterActual;


struct Directory{
	char primer_caracter;
	char nombre_archivo[10]; 
	unsigned char atributos;
	char fecha[8];
	unsigned short direccion;
	unsigned int tamano;
	char reservado[6];
};

struct Cluster{
	char espacio[4096]; //Pueden ser 128 directorios o bytes de un archivo
};

unsigned short fatTable[65536];
Directory root[512];
Cluster Data[65468];

void crearDirectorio(char, char*);
void Catredireccionamiento(char, char*);
void Cat(char *);
void crearParticionFat();
void ls();
void escribirFatFile();
void getInfoFile();
void ls_l();
void cambiarDirectorio(char*);
void rm(char* );
void rmdir(char* );

char* splitLinea(char * line){
	char * tokens = strtok (line," ");
	return tokens;
}

int main(){
	rutaActual.push_back("root");
	ClusterActual.push_back(65467);
	cout<<"Introducir 'formatear' para formatear el disco"<<endl;
	cout<<"Introducir 'usar' para usar disco actual"<<endl;
	while(true){
		cout<<"Mi_sh>";
		for(int o=0;o<rutaActual.size();o++){
			cout<<"/"<<rutaActual[o];
		}
		cout<<"$ ";
		char line[255];
		cin.getline(line,255);
		char * tokens = splitLinea(line);
		char cmd[10];
		char nombre[10];
			nombre[0]=0;
		char extra[10];
			extra[0]=0;
		int cont = 0;
		while (tokens != NULL) {
			if(cont==0){
				memcpy (cmd, tokens,10);
			}else if(cont==1){
				memcpy (nombre, tokens,10);
			}else if(cont==2){
				memcpy (extra, tokens,10);
			}
			cont++;
			tokens = strtok (NULL, " ");
		}



		if(strcmp(cmd, "formatear")==0){
			crearParticionFat();
		}else if(strcmp(cmd, "usar")==0){
			getInfoFile(); // copia la informacion del archivo binario en las diferentes estructuras.
		}else if(strcmp(cmd, "mkdir")==0){
			if(nombre[0]==0){
				cout<<"sin nombre"<<endl;
			}else{
				crearDirectorio(nombre[0],nombre);
			}
			
		}else if(strcmp(cmd, "cat")==0){
			if(nombre[0]=='>'){
				if(extra[0]==0){
					cout<<"sin nombre"<<endl;
				}else{
					Catredireccionamiento(extra[0], extra);
				}
			}else if(nombre[0]==0){
				cout<<"Sin nombre"<<endl;
			}else{
				Cat(nombre);
			}
		}else if(strcmp(cmd, "ls")==0){
			if(nombre[0]=='-' && nombre[1]=='l'){
				if(extra[0]!=0){
					cout<<"Comando incorrecto"<<endl;
				}else{
					ls_l();
				}
			}else if(nombre[0]==0){
				ls();
			}
		}else if(strcmp(cmd, "cd")==0){
			if(nombre[0]==0){
				cout<<"sin nombre"<<endl;
			}else{
				cambiarDirectorio(nombre);
			}
		}else if(strcmp(cmd, "rm")==0){
			if(nombre[0]==0){
				cout<<"sin nombre"<<endl;
			}else{
				rm(nombre);
			}
		}else if(strcmp(cmd, "rmdir")==0){
			if(nombre[0]==0){
				cout<<"sin nombre"<<endl;
			}else{
				rmdir(nombre);
			}
		}else if(strcmp(cmd, "exit")==0){
			escribirFatFile();
			break;
		}

		for(int i=0;i<10;i++){
			nombre[i] = '\0';
			cmd[i] = '\0';
			extra[i] = '\0';
		}

		
		
	}
	return 0;
}

void Cat(char * nombre){
	bool seencuentra = false;
	if(actualEsRoot){
		for(int rot =0; rot<512; rot++){
			 if(root[rot].primer_caracter!=0 && root[rot].atributos=='a'){
			 	if(strcmp(root[rot].nombre_archivo, nombre)==0){
			 		//seencuentra= true;
			 		int nclusters = ceil(root[rot].tamano/4096);
			 		string line ="";
					if(nclusters==0)
						nclusters++;
			 		int contador=0;
			 		for(int clus=0 ;clus<nclusters;clus++){
						
						for(int a=0 ;a<4096;a++){
							if(Data[root[rot].direccion-69].espacio[a]!='^'){
								line+= Data[root[rot].direccion-69].espacio[a];
							}else{
								break;
							}
						}
					}
					cout<<line<<endl;

				}
			 		if(seencuentra == true);
			 			//break;
			}
			
		}
	}else{
		
		for(int i =0; i<128; i++){
			 if(Data[clusterDirectorioActual-69].espacio[i*32]!=0 && 
			 	Data[clusterDirectorioActual-69].espacio[(i*32)+11]=='a'){
			 	char nom[10];
			 	memcpy (nom, &Data[clusterDirectorioActual-69].espacio[(i*32)+1],10);
			 	if(strcmp(nom, nombre)==0){
			 		int nclusters = ceil(Data[clusterDirectorioActual-69].espacio[(i*32)+22]/4096);
			 		string line ="";
					if(nclusters==0)
						nclusters++;
			 		int contador=0;
			 		for(int clus=0 ;clus<nclusters;clus++){
						
						for(int a=0 ;a<4096;a++){
							if(Data[Data[clusterDirectorioActual-69].espacio[(i*32)+20]-69].espacio[a]!='^'){

								line+= Data[Data[clusterDirectorioActual-69].espacio[(i*32)+20]-69].espacio[a];
							}else{
								break;
							}
						}
					}
					cout<<line<<endl;

				}
			 		if(seencuentra == true);
			 			//break;
			}
			
		}
		
	}
	if(!seencuentra){
		cout<<"El archivo no existe"<<endl;
	}
}

void ls(){
	int contar = 0;
	if(actualEsRoot){
		for(int rot =0; rot<512; rot++){
			 if(root[rot].primer_caracter!=0){
			 	printf(" %s\t",root[rot].nombre_archivo);
			 	contar++;
			 }
		}
	}else{
		for(int i =0;i<128; i++){
			if(Data[clusterDirectorioActual-69].espacio[i*32] != 0){
				char nom[10];
				memcpy (nom , &Data[clusterDirectorioActual-69].espacio[(i*32)+1],10);
				printf(" %s\t",nom);
				contar++;
			}
		}
	}
	if(contar>0)
		cout<<endl;
}

void ls_l(){
	if(actualEsRoot){
		for(int rot =0; rot<512; rot++){
			 if(root[rot].primer_caracter!=0){
				cout<<root[rot].nombre_archivo<<"\t"<< root[rot].atributos<<"\t"<<root[rot].tamano<<"\t"<<root[rot].fecha<<endl;
     
			 }
		}
	}else{
		for(int i =0;i<128; i++){
			if(Data[clusterDirectorioActual-69].espacio[i*32] != 0){
				char nom[10];
				char fecha[9];
				memcpy (nom , &Data[clusterDirectorioActual-69].espacio[(i*32)+1],10);
				memcpy (fecha, &Data[clusterDirectorioActual-69].espacio[(i*32)+12], 9);
				int tamano = int(Data[clusterDirectorioActual-69].espacio[(i*32)+22]+Data[clusterDirectorioActual-69].espacio[(i*32)+23]
								+Data[clusterDirectorioActual-69].espacio[(i*32)+24]+Data[clusterDirectorioActual-69].espacio[(i*32)+25]);
				cout<<nom<<"\t"<< Data[clusterDirectorioActual-69].espacio[(i*32)+11]<<'\t'<<tamano<<"\t";
				for (int j=0;j<9;j++)
					cout<<fecha[j];
				cout<<endl;
			}
		}
	}
}

void crearParticionFat(){	
	for(int i = 0; i<65536; i++)
		fatTable[i] = 0;
    ofstream outfile;
  	outfile.open("particionfat16", ios::binary | ios::out);
  	outfile.write((char*)&fatTable, sizeof(short int)*65536);
  	outfile.write((char*)&fatTable, sizeof(short int)*65536);
  	for(int i = 0; i<512; i++){
  		outfile.write((char*)&root, 32);
  	}
  	//65468
  	for(int i = 0; i<65468; i++){
  		outfile.write((char*)&Data[i].espacio, 4096);
  	}
  
  	outfile.close();
}

void crearDirectorio(char primera_letra, char *nombre ){ // CREAR DIRECTORIO
	if(actualEsRoot){
		char tipo = 'd';
		time_t now = time(0);
		char* dt = ctime(&now);
		char fecha[8];
		memcpy ( fecha, dt, 8);
		char nombre_archivo[10];
		memcpy (nombre_archivo,nombre,10);
		char reservado[6];
		for(int rot =0; rot<512; rot++){ //ahora revisa en la region root
			if(root[rot].primer_caracter != 0){
				if(strcmp(root[rot].nombre_archivo, nombre_archivo)==0){
					cout<<"El directorio ya existe"<<endl;
					break;
				}
			}else{
				//cout<<"vacio"<<endl;
				short int cluesterdireccion; // buscar cluster disponible en la fattable
				bool hayespaciofattable;
				for(int clus=69;clus<65536;clus++){
					if(fatTable[clus]==0 ){
						cluesterdireccion =clus;
						fatTable[clus]= 63; // el cluster 63 es parte de la fattable(indica entonces que no apunta a otro cluster)
						hayespaciofattable= true;
						break;
					}
				}
				if(hayespaciofattable){
					root[rot].primer_caracter = primera_letra;
					memcpy (root[rot].nombre_archivo,nombre,10);
					root[rot].atributos = tipo;
					memcpy (root[rot].fecha, dt, 8);
					root[rot].direccion = cluesterdireccion;
					root[rot].tamano = 4096;
				}else{
					cout<<"El disco esta lleno"<<endl;
				}
				break;
			}
		}
	}else{
		char tipo = 'd';
		time_t now = time(0);
		char* dt = ctime(&now);
		char fecha[8];
		memcpy ( fecha, dt, 8);
		char nombre_archivo[10];
		memcpy (nombre_archivo,nombre,10);
		char reservado[6];
		for(int i=0;i<128*32; i+=32){
			if(Data[clusterDirectorioActual-69].espacio[i] != 0){
				char noma[10];
				memcpy(noma,&Data[clusterDirectorioActual-69].espacio[(i*32)+1],10);
				if(strcmp(noma, nombre_archivo)==0){
					cout<<"El directorio ya existe"<<endl;
					break;
				}
			}else{
				//cout<<"vacio"<<endl;
				short int cluesterdireccion; // buscar cluster disponible en la fattable
				bool hayespaciofattable;
				for(int clus=69;clus<65536;clus++){
					if(fatTable[clus]==0 ){
						cluesterdireccion =clus;
						fatTable[clus]= 63; // el cluster 63 es parte de la fattable(indica entonces que no apunta a otro cluster)
						hayespaciofattable= true;
						break;
					}
				}
				if(hayespaciofattable){


					Data[clusterDirectorioActual-69].espacio[i] = primera_letra;
					for(int j =0;j<10;j++){
						Data[clusterDirectorioActual-69].espacio[i+j+1] = nombre[j];
					}				
					Data[clusterDirectorioActual-69].espacio[i+11] = tipo;
					for(int j=0;j<8;j++){
						Data[clusterDirectorioActual-69].espacio[i+12+j] = fecha[j];
					}
					char direccionTemp[2];
	    			*((unsigned short int *) direccionTemp) = cluesterdireccion;
					/*char* direccionTemp = (char*)cluesterdireccion;
					//memcpy (direccionTemp, (char*)cluesterdireccion, 2);*/
					for(int j =0;j<2;j++){
						Data[clusterDirectorioActual-69].espacio[i+20+j] = direccionTemp[j];
					}
					char tamanoTemp[4];
	    			*((int *) tamanoTemp) = 4096;
					/*char* tamanoTemp;
					int tamano = 4096;
					tamanoTemp = (char*)tamano;
					//memcpy (tamanoTemp, (char*)tamano, 4);*/
					for(int j =0;j<4;j++){
						Data[clusterDirectorioActual-69].espacio[i+22+j] = tamanoTemp[j];
					}
				}else{
					cout<<"El disco esta lleno"<<endl;
				}
				break;
			}
		}
	}
}


void cambiarDirectorio(char * nombre){
	int direccionAsignada = -1;
	if(strcmp(nombre, "..")==0){
		if(rutaActual.back() !="root"){
			rutaActual.pop_back();
			ClusterActual.pop_back();
			if(ClusterActual.back() == 65467)
				actualEsRoot = true;
			clusterDirectorioActual = ClusterActual.back();
			
		}
	}else{
		if(clusterDirectorioActual == 65467){//se encuentra en la root		
			for(int rot =0; rot<512; rot++){
			 if(root[rot].primer_caracter!=0){
			 	char* name = root[rot].nombre_archivo;
			 	if(strcmp(nombre,name)==0){
			 		direccionAsignada = root[rot].direccion;
			 		break;
			 	}
			 }
			}
		}else{
			for(int i =0;i<128*32; i+=32){
				if(Data[clusterDirectorioActual-69].espacio[i] != 0){
					char name[10];
					for(int j=0;j<10;j++){
						name[j] = Data[clusterDirectorioActual-69].espacio[i+j+1];
					}
					//memcpy(name,(char*)Data[clusterDirectorioActual].espacio[i+1],10);
					if(strcmp(name,nombre)==0){
						char direccionTemp[2];
						for(int j =0;j<2;j++){
							direccionTemp[j] = Data[clusterDirectorioActual-69].espacio[i+j+20];
						}
						//memcpy(direccionTemp,(char*)Data[clusterDirectorioActual].espacio[i+20],2);
						direccionAsignada = (int)*direccionTemp;
			 			break;
					}
				}
			}
		}
		if(direccionAsignada != -1){
			rutaActual.push_back(nombre);
			clusterDirectorioActual = direccionAsignada;
			ClusterActual.push_back(clusterDirectorioActual);
			actualEsRoot = false;
		}else
			cout<<"No existe esa subcarpeta"<<endl;
		//cout<<"direccion actual: "<<clusterDirectorioActual<<endl;
	}
	
}


void Catredireccionamiento(char primera_letra, char *nombre ){ // cREAR UN ARCHIVOO
	char tipo = 'a';
	time_t now = time(0);
	char* dt = ctime(&now);
	char fecha[8];
	memcpy ( fecha, dt, 8);
	char nombre_archivo[10];
	memcpy (nombre_archivo,nombre,10);
	char reservado[6];
	if(actualEsRoot){
		bool yaexiste = false;
		for(int rot =0; rot<512; rot++){ //ahora revisa en la region root
			if(root[rot].primer_caracter != 0){
				if(strcmp(root[rot].nombre_archivo, nombre_archivo)==0){
					cout<<"El archivo se va a sobre escribir"<<endl;
					yaexiste = true;
					rm(nombre);
					Catredireccionamiento(primera_letra,nombre);
					break;
				}
			}else{
			}
		}
		if(!yaexiste){
			for(int rot =0; rot<512; rot++){ //ahora revisa en la region root
				if(root[rot].primer_caracter != 0){
					//
				}else{
					//cout<<"vacio"<<endl;
					string line;
					string lineas[2000]; //longitud al chilaso
					cin.ignore();
					int contador=0;
					int nbyteslineas=0;
					int nclusters =0;
					while(line!="exit"){
		  				getline (cin,line);
		  				lineas[contador] = line;
		  				contador++;
					}
					for(int h= 0;h<contador-2;h++){
						lineas[h]+= "\n";
					}
					lineas[contador-1] = "^"; 
					lineas[contador] = "final"; /// indicando que es la ultima linea, no la incluye
					contador=0;
					while(lineas[contador]!="final" && contador<2000){
						nbyteslineas += lineas[contador].size();
		  				contador++;
					}

					nclusters = ceil(nbyteslineas/4096);
					if(nclusters==0)
						nclusters++;
					short nclustersencontrados[nclusters];
					short int cluesterdireccion; // buscar cluster disponible en la fattable
					bool hayespaciofattable;
					contador =0;
					for(int clus=69;clus<65536;clus++){
						if(fatTable[clus]==0 ){
							nclustersencontrados[contador] =clus;
							contador++;
						}
						if(contador==nclusters){
							hayespaciofattable =true;
							break;
						}

					}
					if(hayespaciofattable){
						cluesterdireccion = nclustersencontrados[0];
						root[rot].primer_caracter = primera_letra;
						memcpy (root[rot].nombre_archivo,nombre,10);
						root[rot].atributos = tipo;
						memcpy (root[rot].fecha, dt, 8);
						root[rot].direccion = cluesterdireccion;
						root[rot].tamano= nbyteslineas;
						
						for(int asig =0; asig< nclusters-1;asig++){ // lista de los clusters
							fatTable[nclustersencontrados[asig]] = nclustersencontrados[asig+1];
						}
						fatTable[nclustersencontrados[nclusters-1]]=63; // el cluster 63 es parte de la fattable(indica entonces que no apunta a otro cluster)

						if(nbyteslineas>0){
							char cadenadivida[nbyteslineas];
							contador=0;
							for(int l =0; l<sizeof(lineas); l++){ // dividir los bytes de las cadenas y meterlas en un array de chars
								if(lineas[l]=="final")
									break;
								for(int a =0; a<lineas[l].size(); a++){
									cadenadivida[contador] = lineas[l][a];
									contador++;
								}
							}
						
							contador =0;
							for(int asig =0; asig< nclusters;asig++){ // Ingresar los bytes del archivo... si los hay
								for(int w=0;w<4096;w++){
									if((w+contador-1)==nbyteslineas)
										break;
									Data[nclustersencontrados[asig]-69].espacio[w] = cadenadivida[w+contador]; //-69 porque Data solo tiene los cluster de data
								}
								contador+=4096;
							}
						}


					}else{
						cout<<"El disco esta lleno"<<endl;
					}
					
					break;
				}
			}
		}
	}else{
		bool yaexiste = false;
		for(int sub =0; sub<128; sub++){ 
			if(Data[clusterDirectorioActual-69].espacio[(sub*32)]!= 0){
				char noma[10];
				memcpy(noma,&Data[clusterDirectorioActual-69].espacio[((sub*32)+1)],10 );
				if(strcmp(noma, nombre_archivo)==0){
					cout<<"El archivo se va a sobre escribir"<<endl;
					yaexiste = true;
					rm(nombre);
					Catredireccionamiento(primera_letra,nombre);
					break;
				}
			}else{
			}
		}
		if(!yaexiste){
			for(int sub =0; sub<128; sub++){ 
				if(Data[clusterDirectorioActual-69].espacio[(sub*32)]!= 0){
					//cout<<"hay algo"<<endl;
				}else{
					//cout<<"vacio"<<endl;
					string line;
					string lineas[255]; //longitud al chilaso
					cin.ignore();
					int contador=0;
					int nbyteslineas=0;
					int nclusters =0;
					while(line!="exit"){
		  				getline (cin,line);
		  				lineas[contador] = line;
		  				contador++;
					}
					for(int h= 0;h<contador-2;h++){
						lineas[h]+= "\n";
					}
					lineas[contador-1] = '^'; 
					lineas[contador] = "final"; /// indicando que es la ultima linea, no la incluye
					contador=0;
					while(lineas[contador]!="final" && contador<255){
						nbyteslineas += lineas[contador].size();
		  				contador++;
					}

					nclusters = ceil(nbyteslineas/4096);
					if(nclusters==0)
						nclusters++;
					short nclustersencontrados[nclusters];
					short int cluesterdireccion; // buscar cluster disponible en la fattable
					bool hayespaciofattable;
					contador =0;
					for(int clus=69;clus<65536;clus++){
						if(fatTable[clus]==0 ){
							nclustersencontrados[contador] =clus;
							contador++;
						}
						if(contador==nclusters){
							hayespaciofattable =true;
							break;
						}

					}
					if(hayespaciofattable){
						cluesterdireccion = nclustersencontrados[0];
						Data[clusterDirectorioActual-69].espacio[(sub*32)] = primera_letra;
						memcpy (&Data[clusterDirectorioActual-69].espacio[(sub*32)+1],nombre,10);
						Data[clusterDirectorioActual-69].espacio[(sub*32)+11]= tipo;
						memcpy (&Data[clusterDirectorioActual-69].espacio[(sub*32)+12], dt, 8);
						Data[clusterDirectorioActual-69].espacio[(sub*32)+20]= cluesterdireccion;
						Data[clusterDirectorioActual-69].espacio[(sub*32)+22]= nbyteslineas;
						
						for(int asig =0; asig< nclusters-1;asig++){ // lista de los clusters
							fatTable[nclustersencontrados[asig]] = nclustersencontrados[asig+1];
						}
						fatTable[nclustersencontrados[nclusters-1]]=63; // el cluster 63 es parte de la fattable(indica entonces que no apunta a otro cluster)

						if(nbyteslineas>0){
							char cadenadivida[nbyteslineas];
							contador=0;
							for(int l =0; l<sizeof(lineas); l++){ // dividir los bytes de las cadenas y meterlas en un array de chars
								if(lineas[l]=="final")
									break;
								for(int a =0; a<lineas[l].size(); a++){
									cadenadivida[contador] = lineas[l][a];
									contador++;
								}
							}
						
							contador =0;
							for(int asig =0; asig< nclusters;asig++){ // Ingresar los bytes del archivo... si los hay
								for(int w=0;w<4096;w++){
									if((w+contador-1)==nbyteslineas)
										break;
									Data[nclustersencontrados[asig]-69].espacio[w] = cadenadivida[w+contador]; //-69 porque Data solo tiene los cluster de data
								}
								contador+=4096;
							}
						}


					}else{
						cout<<"El disco esta lleno"<<endl;
					}
					
					break;
				}
			}
		}
	}
}

void getInfoFile(){


	ifstream infile;
	infile.open("particionfat16", ios::binary | ios::in);
	infile.seekg(131072);
	for(int i = 0; i<65536; i++){
		char temp[1];
		infile.read (temp, 1);
		char temp2[1];
		infile.read (temp2, 1);
			fatTable[i]= (unsigned short)(temp[0]+temp2[0]);
	}
  	
  	for(int i = 0; i<512; i++){
  		infile.read((char*)&root[i], 32);
  	}
  
  	for(int i = 0; i<65468; i++){
  		infile.read((char*)&Data[i], 4096);
  	}
  	
  	infile.close();

}

void escribirFatFile(){
	ofstream outfile;
  	outfile.open("particionfat16", ios::binary | ios::out);
  	outfile.write((char*)&fatTable, sizeof(short int)*65536);
  	outfile.write((char*)&fatTable, sizeof(short int)*65536);
  	
  	for(int i = 0; i<512; i++){
  		outfile.write((char*)&root[i], 32);
  	}
  	
  	for(int i = 0; i<65468; i++){
  		outfile.write((char*)&Data[i], 4096);
  	}
  	
  	
  	outfile.close();
  	

}

void rm(char* nombre){
	int direccionAsignada = -1;
	if(clusterDirectorioActual == 65467){//se encuentra en la root		
			for(int rot =0; rot<512; rot++){
			 if(root[rot].primer_caracter!=0){
			 	char* name = root[rot].nombre_archivo;
			 	if(strcmp(nombre,name)==0 && root[rot].atributos == 'a'){
			 		direccionAsignada = root[rot].direccion;
			 		root[rot].primer_caracter = 0;
			 		break;
			 	}
			 }
			}
			
		}else{
			for(int i =0;i<128*32; i+=32){
				if(Data[clusterDirectorioActual-69].espacio[i] != 0){
					char name[10];
					for(int j=0;j<10;j++){
						name[j] = Data[clusterDirectorioActual-69].espacio[i+j+1];
					}
					//memcpy(name,(char*)Data[clusterDirectorioActual].espacio[i+1],10);
					if(strcmp(name,nombre)==0 && Data[clusterDirectorioActual-69].espacio[i+11]=='a'){
						char direccionTemp[2];
						for(int j =0;j<2;j++){
							direccionTemp[j] = Data[clusterDirectorioActual-69].espacio[i+j+20];
						}
						//memcpy(direccionTemp,(char*)Data[clusterDirectorioActual].espacio[i+20],2);
						direccionAsignada = (int)*direccionTemp;
						Data[clusterDirectorioActual-69].espacio[i] = 0;
			 			break;
					}
				}
			}
		}
		if(direccionAsignada == -1){
				cout<<"No existe el archivo"<<endl;
			}else{
				while(fatTable[direccionAsignada]!=63){
					//cout<<"dir: "<<direccionAsignada<<endl;
					int temporal = fatTable[direccionAsignada];
					fatTable[direccionAsignada] = 0;
					direccionAsignada = temporal;
				}
				//cout<<"dir: "<<direccionAsignada<<endl;
				fatTable[direccionAsignada] = 0;
			}
}

void rmdir(char* nombre){
	int direccionAsignada = -1;
	bool directoriovacio = true;
	if(clusterDirectorioActual == 65467){//se encuentra en la root		
			for(int rot =0; rot<512; rot++){
			 if(root[rot].primer_caracter!=0){
			 	char* name = root[rot].nombre_archivo;
			 	if(strcmp(nombre,name)==0 && root[rot].atributos == 'd'){
			 		direccionAsignada = root[rot].direccion;
			 		for(int y=0;y<128;y++){
			 			if(Data[direccionAsignada-69].espacio[y*32]!=0){
			 				directoriovacio=false;
			 				break;
			 			}
			 		}
			 		if(directoriovacio)
			 			root[rot].primer_caracter = 0;
			 		
			 		break;
			 	}
			 }
			}
			
		}else{
			for(int i =0;i<128*32; i+=32){
				if(Data[clusterDirectorioActual-69].espacio[i] != 0){
					char name[10];
					for(int j=0;j<10;j++){
						name[j] = Data[clusterDirectorioActual-69].espacio[i+j+1];
					}
					//memcpy(name,(char*)Data[clusterDirectorioActual].espacio[i+1],10);
					if(strcmp(name,nombre)==0 && Data[clusterDirectorioActual-69].espacio[i+11]=='d'){
						char direccionTemp[2];
						for(int j =0;j<2;j++){
							direccionTemp[j] = Data[clusterDirectorioActual-69].espacio[i+j+20];
						}
						//memcpy(direccionTemp,(char*)Data[clusterDirectorioActual].espacio[i+20],2);
						direccionAsignada = (int)*direccionTemp;
						for(int y=0;y<128;y++){
				 			if(Data[direccionAsignada-69].espacio[y*32]!=0){
				 				directoriovacio=false;
				 				break;
				 			}
			 			}
			 			if(directoriovacio)
							Data[clusterDirectorioActual-69].espacio[i] = 0;
			 			break;
					}
				}
			}
		}
		if(direccionAsignada == -1){
				cout<<"No existe el directorio"<<endl;
			}else{
				if(directoriovacio){
					while(fatTable[direccionAsignada]!=63){
						//cout<<"dir: "<<direccionAsignada<<endl;
						int temporal = fatTable[direccionAsignada];
						fatTable[direccionAsignada] = 0;
						direccionAsignada = temporal;
					}
					//cout<<"dir: "<<direccionAsignada<<endl;
					fatTable[direccionAsignada] = 0;
				}else{
					cout<<"EL directorio no esta vacio"<<endl;
				}
			}
}
/* ESTE COPIA UN ARCHIVO--- por cualquier cosa
void CrearArchivoRoot(char primera_letra, char *nombre ,char* nombre_archivo_a_copiar){
	int offset = 131073;
	char tipo = 'a';
	time_t now = time(0);
	char* dt = ctime(&now);
	char fecha[8];
	memcpy ( fecha, dt, 8);
	char nombre_archivo[10];
	memcpy (nombre_archivo,nombre,10);
	int direccion = offset;
	ifstream infile;
	infile.open("particionfat32", ios::binary | ios::in);
	infile.seekg(offset);
	char reservado[6];
	while(direccion<139264){
		char temp[32];
		infile.read (temp, 32);
		if(temp[0] != NULL){
			cout<<"hay algo"<<endl;
			direccion += 32;
		}else{
			infile.close();
			cout<<"vacio"<<endl;
			Directory directorio ;
			directorio.primer_caracter = primera_letra;
			memcpy (directorio.nombre_archivo,nombre,10);
			//directorio.nombre_archivo = nombre_archivo;
			directorio.atributos = tipo;
			memcpy (directorio.fecha, dt, 8);
			//directorio.fecha = fecha;
			ifstream infile2;
			infile2.open(nombre_archivo_a_copiar, ios::binary|ios::in);
			infile2.seekg (0, ios::end);
			int tamano = infile2.tellg();
			infile2.close();
			int cantidad_clusters = tamano/4096 +1;
			//cout<<cantidad_clusters<<endl;
			int contador = 68;
			ifstream infile3;
			infile3.open("particionfat32", ios::binary|ios::in|ios::out);
			infile3.seekg(136, ios::beg);
			vector< unsigned short int> lista_enlazada;
			cout<<"entra primer while"<<endl;
			while(cantidad_clusters > 0 && contador<=65536){
				unsigned short int* posicion = new unsigned short int[1]; //= static_cast<short int>(temporal);
				infile3.read(reinterpret_cast<char *>(posicion),sizeof(short int));
				contador+= 1;
				//cout<<*posicion<<endl;
				if(*posicion == 0){
					lista_enlazada.push_back(infile.tellg()-sizeof(short int));
					cantidad_clusters--;
					//cout<<cantidad_clusters;
				}
			}
			infile3.close();
			cout<<"pasa primer while"<<endl;
			
			ofstream outfile;
			outfile.open("particionfat16",ios::binary|ios::out|ios::in);

			for(int i = 0;i<lista_enlazada.size()-1; i++){
				outfile.seekp(lista_enlazada[i], ios::beg);
				outfile.write((char*)&lista_enlazada[i+1],2);
			}
			cout<<"pasa segundo for"<<endl;
			outfile.seekp(lista_enlazada[lista_enlazada.size()-1], ios::beg);
			short int* eof = new short int[1];
			eof[0]=65535;
			outfile.write((char*)eof,2);

			
			infile2.open(nombre_archivo_a_copiar, ios::binary|ios::in);
			cout<<"entra segundo for"<<endl;
			for(int i = 0; i<lista_enlazada.size(); i++){
				infile2.seekg(i*4096, ios::beg);
				char buffer[4096];
				infile2.read(buffer,4096);
				outfile.seekp(lista_enlazada[i]/2*4096, ios::beg);
				outfile.write(buffer,4096);
			}
			cout<<"pasa tercer for"<<endl;
			directorio.direccion = lista_enlazada[0];
			directorio.tamano = tamano;
					
			
			outfile.seekp(direccion, ios::beg);
			outfile.write((char*)&directorio,32);
			outfile.close();

			break;
		}
	}
	if(direccion>278528)
		infile.close();
}
*/

