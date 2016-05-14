#include <iostream> 
#include <fstream>
#include <ctime>
#include <stdio.h>
#include <string.h>
#include <vector>

using namespace std;


unsigned short int fatTable[65536];
char cluster[4096];

void crearDirectorioRoot(char, char*, int);
void CrearArchivoRoot(char, char*, char*);
void crearParticionFat();
void ls();

struct Directory{
	char primer_caracter;
	char nombre_archivo[10]; 
	unsigned char atributos;
	char fecha[8];
	unsigned short int direccion;
	unsigned int tamano;
	char reservado[6];
};



int main(){
	cout<<"1) Crear archivo particion"<<endl;
	cout<<"2) Agregar directorio a particion ya creada"<<endl;
	cout<<"3) Agregar archivo a particion ya creado(archivo debe estar en el mismo directorio)"<<endl;
	cout<<"4) ls"<<endl;
	int opcion;
	cout<<"Opcion: "; 
	cin>>opcion;
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
	return 0;
}

void ls(){
	char line[32];
	char nombre[11];
	ifstream fentrada;
	fentrada.open("particionfat32",ios::binary|ios::out|ios::in);
			
	for(int i = 0; i<65536; i++){
		 fentrada.read(line, 32);
		 if(line[0]!=0){
		 	memcpy( nombre, line + 1, 10 );
		 	cout<<"Name: "<<nombre;
		 	cout<<" ";
		 	cout<<"Tipo: "<<line[11]<<endl;
		 }
	}
	fentrada.close();
}

void crearParticionFat(){	
	for(int i = 0; i<65536; i++)
		fatTable[i] = 0;
    ofstream outfile;
  	outfile.open("particionfat32", ios::binary | ios::out);
  	outfile.write((char*)&fatTable, sizeof(short int)*65536);
  	outfile.write((char*)&fatTable, sizeof(short int)*65536);
  	for(int i = 0; i<4096; i++){
  		cluster[i] = NULL;
  	}
  	for(int i = 0; i<65472; i++){
  		outfile.write((char*)&cluster, 4096);
  	}
  	outfile.close();
}

void crearDirectorioRoot(char primera_letra, char *nombre ,int tamano){
	short int offset = 262144;
	char tipo = 'd';
	time_t now = time(0);
	char* dt = ctime(&now);
	char fecha[8];
	memcpy ( fecha, dt, 8);
	char nombre_archivo[10];
	memcpy (nombre_archivo,nombre,10);
	short int direccion = offset;
	ifstream infile;
	infile.open("particionfat32", ios::binary | ios::in);
	infile.seekg(offset);
	char reservado[6];
	while(direccion<278528){
		char temp[32];
		infile.read (temp, 32);
		if(temp[0] != NULL){
			cout<<"hay algo"<<endl;
			direccion += 32;
		}else{
			cout<<"vacio"<<endl;
			Directory directorio ;
			directorio.primer_caracter = primera_letra;
			memcpy (directorio.nombre_archivo,nombre,10);
			//directorio.nombre_archivo = nombre_archivo;
			directorio.atributos = tipo;
			memcpy (directorio.fecha, dt, 8);
			//directorio.fecha = fecha;
			directorio.direccion = offset;
			directorio.tamano = tamano;
			ofstream outfile;
			infile.close();
			outfile.open("particionfat32",ios::binary|ios::out|ios::in);
			outfile.seekp(direccion);
			outfile.write((char*)&directorio,32);
			outfile.close();
			break;
		}
	}
	if(direccion>278528)
		infile.close();
}

void CrearArchivoRoot(char primera_letra, char *nombre ,char* nombre_archivo_a_copiar){
	short int offset = 262144;
	char tipo = 'a';
	time_t now = time(0);
	char* dt = ctime(&now);
	char fecha[8];
	memcpy ( fecha, dt, 8);
	char nombre_archivo[10];
	memcpy (nombre_archivo,nombre,10);
	short int direccion = offset;
	ifstream infile;
	infile.open("particionfat32", ios::binary | ios::in);
	infile.seekg(offset);
	char reservado[6];
	while(direccion<278528){
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
			outfile.open("particionfat32",ios::binary|ios::out|ios::in);

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