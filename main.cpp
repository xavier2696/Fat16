#include <iostream> 
#include <fstream>
#include <ctime>
#include <stdio.h>
#include <string.h>

using namespace std;


unsigned short int fatTable[65536];
char cluster[4096];

void crearDirectorioRoot(char, char*, int);
void crearParticionFat();

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
	
	int opcion;
	cin>>opcion;
	switch(opcion){
		case 1:
			crearParticionFat();
			break;
		case 2:
			
			crearDirectorioRoot('a',"nombre",0);
			break;
		default:
			break;
	}
	return 0;
}

void crearParticionFat(){	
	for(int i = 0; i<65536; i++)
		fatTable[i] = 0;
    ofstream outfile;
  	outfile.open("particionfat32", ios::binary | ios::out);
  	outfile.write((char*)&fatTable, sizeof(short int)*65536);
  	outfile.write((char*)&fatTable, sizeof(short int)*65536);
  	for(int i = 0; i<4096; i++){
  		cluster[i] = '\0';
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
		if(temp[0] != '\0'){
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
	//Directory directorio = {primera_letra,nombre_archivo,tipo, fecha,direccion, tamano};
}