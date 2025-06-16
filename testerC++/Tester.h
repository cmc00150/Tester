#ifndef TESTER_H
#define TESTER_H

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <io.h>
    #include <fcntl.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
    #include <cstdio>
#endif
#include <list>
#include <iostream>
#include <climits>
#include "Pregunta.h"
#include "Tester.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <random>
#include "Abcd.h"
#include "VerdaderoFalso.h"
#include <thread>
#include <algorithm>
#include <format>

#define VERDE "\033[0;32m"
#define ROJO "\033[0;31m"
#define FONDOROJO "\033[0;41m"
#define FONDOVERDE "\033[0;42m"
#define FONDOAMARILLO "\033[0;43m"
#define RESETEARFORMATO "\033[0m"
#define LIMPIARPANTALLA "\033[2J\033[H"
#define LIMPIARLINEA "\033[2K"
#define ESCONDERCURSOR "\033[?25l"
#define MOSTRARCURSOR "\033[?25h"


class Tester {
	list<Pregunta*> preguntas;
	list<Pregunta*>::iterator progreso;
	int pregMostradas = 1;
	int aciertos = 0;
	unsigned int penalizacion = INT_MAX;
	chrono::time_point<chrono::high_resolution_clock> tiempoInicio;

	void recogerPreguntaVerdaderoFalso(string preg);
	void recogerPreguntaAbcd(string preg);

public:
	Tester ( string archivo, string flag );

	void setPenalizacion(unsigned int pen);

	void limpiarPantalla() { cout << LIMPIARPANTALLA; }
	void barraProgreso( int tam = 0 );
	void espera( float tiempo );
	void comprobarRespuesta( char respuesta );
	void mostrarEnunciado();
    void operator++();
	void resumen( int tam = 0 );
	void desordenar();

    bool fin() { return progreso == preguntas.end(); }

	~Tester();
};

#endif //TESTER_H