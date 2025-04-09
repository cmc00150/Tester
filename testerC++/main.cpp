#include <complex>
#include <iostream>
#include <filesystem>
#include "Tester.h"
using namespace std;
using namespace filesystem;

#define HELP R"(SINTAXIS:
	tester [opciones] [flag](obligatorio) [directorio](obligatorio)

	FLAGS:
		-vf		Para documentos de verdadero y falso.
		-ab		Para documentos de varias opciones.

	OPCIONES:
		--random		Muestra las preguntas de forma aleatoria (default: forma ordenada).

		--time			Establece un tiempo de delay entre pregunta y pregunta (default: 0), debe ser 0 o mayor que 0.

		--penalty		Establece cuantos fallos hay que tener para restar una buena (default: ninguna), debe ser mayor que 1.

		--history		Evita que se limpie la pantalla después de cada pregunta [true|false] (default: true).)"

void errorFatal( string motivo ) {
	cerr << motivo << endl;
	cerr << HELP;
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
try {
    if (argc < 2) throw runtime_error("[Error] debe pasarse al menos 2 parámetros [flag] [directorio]");

    string flag, ruta;
    float tiempo = 0;
    int penalización = 0;
    bool randomSolicitado = false, activarHistorial = true, modoVentana = false;

    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "-vf" or string(argv[i]) == "-ab") {
            if (!flag.empty()) {
                char respuesta;
                cout << "¡CUIDADO! Se han detectado varias flags ¿Deseas guardar sobreescirbir la anterior? [S/N]: ";
                cin >> respuesta;
                if (toupper(respuesta) != 'S') continue;
            }
            flag = argv[i];
        } 
        
        else if (string(argv[i]) == "--random")
            randomSolicitado = true;

        else if (string(argv[i]) == "--time") {
            if (i == (argc - 1)) throw runtime_error("[Error] Valor de la opcion --time está fuera de rango");
            
            else if (isdigit(*argv[i + 1]) and *argv[i + 1] >= 0)
                tiempo = stoi(string(argv[++i]));
            else
                throw runtime_error("[Error] valor " + string(argv[i + 1]) + " del parámetro --tiempo invalido");
        } 
        
        else if (string(argv[i]) == "--penalty") {
            if (i == (argc - 1)) throw runtime_error("[Error] Valor de la opcion --penalty está fuera de rango");
            
            if (isdigit(*argv[i + 1]) and *argv[i + 1] >= 0)
                penalización = stoi(string(argv[++i]));
            else
                throw runtime_error("[Error] valor " + string(argv[i + 1]) + " del parámetkro --penalty invalido");

        } 
        
        else if (string(argv[i]) == "--history") {
            if (i == (argc - 1)) throw runtime_error("[Error] Valor de la opcion --history está fuera de rango");
            
            if ((string(argv[i + 1]) == "true" or string(argv[i + 1]) == "false") and *argv[i + 1] >= 1)
                activarHistorial = (argv[++i][0] == 't');
            else
                throw runtime_error("[Error] valor: " + string(argv[i + 1]) + " del parámetro --history invalido");
        }

        else if (argv[i][0] != '-') {
            // Si NO es una flag o una opción (porque no comienza por -):
            if (!ruta.empty()) throw runtime_error("[Error] no puedes asignar dos rutas");

            ruta = argv[i];
            size_t pos = ruta.rfind('.'); // Buscamos su extensión
            if (pos == string::npos or ruta.substr(pos) !=
                                       ".txt") { // Si no se ha encontrado una extensión o es distinta a .txt se lanza una advertencia
                char respuesta;
                cout << "¡CUIDADO! El archivo no tiene la extensión .txt ¿Desea utilizarlo aun así? [S/N]: ";
                cin >> respuesta;
                if (toupper(respuesta) != 'S') exit(EXIT_FAILURE);
                                       }
        } // El resto de casos sería una opcion o una flag
        else
            throw runtime_error("[Error] el argumento " + string(argv[i]) + " no existe");
    }

    if (flag.empty()) throw runtime_error("[Error] no se ha indicado cual es el tipo de las preguntas");
    if (ruta.empty()) throw runtime_error("[Error] no se ha indicado donde se ecuentra el archivo");

    Tester cuestionario(ruta, flag);

    if (randomSolicitado) cuestionario.desordenar();
    if (penalización) cuestionario.setPenalizacion(penalización);

    while (!cuestionario.fin()) {
        if (activarHistorial) cuestionario.limpiarPantalla();
        cuestionario.barraProgreso();
        cuestionario.mostrarEnunciado();

        char respuesta;
        cout << "Respuesta: ";
        cin >> respuesta;

        cuestionario.comprobarRespuesta(respuesta);
        ++cuestionario;
        cuestionario.espera(tiempo);
    }

    cuestionario.resumen();

}catch(exception &e){
    cerr << e.what() << endl << endl;
    cerr << HELP << endl;
}

return 0;
}