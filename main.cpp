#include <iostream> 
#include <fstream>
#include <ctime>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <stdlib.h>

using namespace std;

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
	char espacio[4096]; //Pueden ser 128 directorios o un archivo
};

unsigned short fatTable[65536];
Directory root[512];
Cluster Data[65468];

void crearDirectorioRoot(char, char*, int);
void CrearArchivoRoot(char, char*, char*);
void crearParticionFat();
void ls();
void escribirFatFile();
void getInfoFile();



int main(){
	getInfoFile(); // copia la informacion del archivo binario en las diferentes estructuras.
	while(true){
		cout<<"1) Formatear particion"<<endl;
		cout<<"2) Agregar directorio a particion ya creada"<<endl;
		cout<<"3) Agregar archivo a particion ya creado(archivo debe estar en el mismo directorio)"<<endl;
		cout<<"4) ls"<<endl;
		cout<<"5) Exit"<<endl;
		int opcion;
		cout<<"Opcion: "; 
		cin>>opcion;
		if(opcion==5){
			escribirFatFile();
			break;
		}
		switch(opcion){
			case 1:
				crearParticionFat();
				break;
			case 2:	
				char letra;	
				cout<<"primera letra: ";
				cin>>letra;
				char nombre[10];	
				cout<<"nombre_archivo: ";
				cin>>nombre;
				crearDirectorioRoot(letra,nombre,0);
				break;
			case 3:
				cout<<"primera letra: ";
				cin>>letra;	
				cout<<"nombre_archivo: ";
				cin>>nombre;
				char archivo[10];
				cout<<"nombre archivo a copiar: ";
				cin>> archivo;
				CrearArchivoRoot(letra, nombre, archivo);
				break;
			case 4:
				ls();
				break;
			default:
				break;
		}
	}
	return 0;
}

void ls(){
	
	for(int rot =0; rot<512; rot++){
		 if(root[rot].primer_caracter!=0){
		 	cout<<"Name: "<<root[rot].nombre_archivo;
		 	cout<<" ";
		 	cout<<"Tipo: "<<root[rot].atributos<<endl;
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
  	/*
  	for(int i = 0; i<65468; i++){
  		outfile.write((char*)&Data[i].espacio, 4096);
  	}
  	*/
  	outfile.close();
}

void crearDirectorioRoot(char primera_letra, char *nombre ,int tamano){
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
			cout<<"hay algo"<<endl;
		}else{
			cout<<"vacio"<<endl;
			root[rot].primer_caracter = primera_letra;
			memcpy (root[rot].nombre_archivo,nombre,10);
			//directorio.nombre_archivo = nombre_archivo;
			root[rot].atributos = tipo;
			memcpy (root[rot].fecha, dt, 8);
			//directorio.fecha = fecha;
			short int cluesterdireccion; // buscar cluster disponible en la fattable
			for(int clus=69;clus<65536;clus++){
				cout<<fatTable[clus]<<endl;
				if(fatTable[clus]==0 ){
					cluesterdireccion =clus;
					fatTable[clus]= 63; // el cluster 63 es parte de la fattable(indica entonces que no apunta a otro cluster)
					break;
				}
			}
			root[rot].direccion = cluesterdireccion;
			root[rot].tamano = tamano;
			break;
		}
	}
}

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
	
	/*
  	for(int i = 0; i<65468; i++){
  		infile.read((char*)&Data[i], 4096);
  	}
  	*/
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
  	
  		/*
  	for(int i = 0; i<65468; i++){
  		outfile.write((char*)&Data[i], 4096);
  	}
  	*/
  	
  	outfile.close();
  	

}

