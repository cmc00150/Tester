#ifndef PREGUNTA_H
#define PREGUNTA_H
#include <iostream>
using namespace std;

class Pregunta {
protected:
	string enunciado;
	char opCorrecta;

public:
	Pregunta( string enun ): enunciado(enun) {};

	void añadirOpcionCorrecta ( const char op ) { opCorrecta = op; }
	char verOpcionCorrecta() const { return opCorrecta; }
	virtual string toString()=0;

    virtual ~Pregunta() = default;
};

#endif //PREGUNTA_H
