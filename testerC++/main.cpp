#include <iostream>
#include <filesystem>
#include "Tester.h"
using namespace std;
using namespace filesystem;

int main(int argc, char* argv[]) {
try {
#if defined(_WIN32) | defined(_WIN64)
    SetConsoleOutputCP(CP_UTF8); // Establecemos la salida de la consola para que interprete carácteres UTF-8
#endif

    if (argc < 2) throw runtime_error("[Error] debe pasarse al menos 2 parámetros [flag] [directorio]");

    string flag, ruta;
    float tiempo = 0;
    int penalizacion = 0;
    bool randomSolicitado = false, historial = false;

    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "-vf" or string(argv[i]) == "-ab") {
            if (!flag.empty()) {
                char respuesta;
                cout << "¡CUIDADO! Se han detectado varias flags ¿Deseas guardar sobreescribir la anterior? [S/N]: ";
                cin >> respuesta;
                if (toupper(respuesta) != 'S') continue;
            }
            flag = argv[i];
        } else if (string(argv[i]) == "--random")
            randomSolicitado = true;

        else if (string(argv[i]) == "--time") {
            if (i == (argc - 1)) throw runtime_error("[Error] --time no tiene un valor definido");

            else if (isdigit(*argv[i + 1]) and *argv[i + 1] >= 0)
                tiempo = stof(string(argv[++i]));
            else
                throw runtime_error("[Error] valor " + string(argv[i + 1]) + " del parámetro --tiempo invalido");
        } else if (string(argv[i]) == "--penalty") {
            if (i == (argc - 1)) throw runtime_error("[Error] --penalty no tiene un valor definido");

            if (isdigit(*argv[i + 1]) and *argv[i + 1] >= 0)
                penalizacion = stoi(string(argv[++i]));
            else
                throw runtime_error("[Error] valor " + string(argv[i + 1]) + " del parámetro --penalty invalido");

        } else if (string(argv[i]) == "--history") {
            if (i == (argc - 1)) throw runtime_error("[Error] --history no tiene un valor definido");

            if (string(argv[i + 1]) == "true" or string(argv[i + 1]) == "false")
                historial = (*argv[++i] == 't');
            else
                throw runtime_error("[Error] valor: " + string(argv[i + 1]) + " del parámetro --history invalido");
        } else if (argv[i][0] != '-') {
            // Si NO es una "flag" o una opción (porque no comienza por -):
            if (!ruta.empty())
                throw runtime_error("[Error] no puedes asignar dos rutas");
            ruta = argv[i];
            if (!exists(path(ruta)))
                throw runtime_error("[Error] La ruta '" + ruta + "' no existe");
            if (!is_regular_file(path(ruta)))
                throw runtime_error("[Error] La ruta '" + ruta + "' no es un archivo");

            size_t pos = ruta.rfind('.'); // Buscamos su extensión
            if (pos == string::npos or ruta.substr(pos) !=
                                       ".txt") { // Si no se ha encontrado una extensión o es distinta a .txt se lanza una advertencia
                char respuesta;
                cout << "¡CUIDADO! El archivo no tiene la extensión .txt ¿Desea utilizarlo aun así? [S/N]: ";
                cin >> respuesta;
                if (toupper(respuesta) != 'S') exit(EXIT_FAILURE);
            }
        } // El resto de casos sería una opción o una "flag"
        else {
            throw runtime_error("[Error] el argumento " + string(argv[i]) + " no existe");
        }
    }

    if (flag.empty()) throw runtime_error("[Error] no se ha indicado cual es el tipo de las preguntas");
    if (ruta.empty()) throw runtime_error("[Error] no se ha indicado donde se encuentra el archivo");

    Tester cuestionario(ruta, flag);

    if (randomSolicitado) cuestionario.desordenar();
    if (penalizacion) cuestionario.setPenalizacion(penalizacion);

    while (!cuestionario.fin()) {
        if (!historial) cuestionario.limpiarPantalla();
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
    cerr <<
    R"(SINTAXIS:
	tester [opciones] [flag](obligatorio) [directorio](obligatorio)

	FLAGS:
		-vf		Para documentos de verdadero y falso.
		-ab		Para documentos de varias opciones.

	OPCIONES:
		--random		Muestra las preguntas de forma aleatoria (default: forma ordenada).

		--time			Establece un tiempo de delay entre pregunta y pregunta (default: 0), debe ser 0 o mayor que 0.

		--penalty		Establece cuantos fallos hay que tener para restar una buena (default: ninguna), debe ser mayor que 1.

		--history		Evita que se limpie la pantalla después de cada pregunta [true|false] (default: false).)"
        << endl;
}

return 0;
}