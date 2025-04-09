#include "Abcd.h"
#include <sstream>

string Abcd::toString() {
	stringstream aux;

	aux << enunciado << endl;
	for ( const auto& opcion : opciones )
		aux << opcion << endl;

	return aux.str();
}
