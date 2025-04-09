//
// Created by cesar on 5/02/25.
//

#ifndef VERDADEROFALSO_H
#define VERDADEROFALSO_H
#include "Pregunta.h"
#include <iostream>
using namespace std;

class VerdaderoFalso: public Pregunta {
public:
	explicit  VerdaderoFalso( string enun ): Pregunta( enun ) {}

	string toString() override { return enunciado + " - (V/F)"; }
};



#endif //VERDADEROFALSO_H
