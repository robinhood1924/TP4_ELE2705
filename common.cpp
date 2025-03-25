/*
 * common.cpp
 *
 *  Created on: May 6, 2016
 *		Author: Kevin Leong
 */

/*
 * Ce fichier devrait être intégralement fourni aux étudiants.
 * Il contient la fonction nécessaire pour connaître la taille d'un fichier en bytes
 */

#include "common.h"

using namespace std;

long long int getFileSize(char* filename) {
	ifstream ifs(filename, ios::in | ios::ate | ios::binary);
	long long int filesize = (long long int) ifs.tellg();
	ifs.close();
	return filesize;
}


