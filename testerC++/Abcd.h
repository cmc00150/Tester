//
// Created by cesar on 5/02/25.
//

#ifndef ABCD_H
#define ABCD_H
#include "Pregunta.h"
#include <iostream>
#include <list>
using namespace std;

class Abcd: public Pregunta{
	list<string> opciones;

public:
	explicit Abcd( string enun ): Pregunta( enun ) {}

	void añadirOpcion( const string& preg ) { opciones.push_back(preg); }
	string toString() override;
};



#endif //ABCD_H
