#include "Tester.h"
/**
 * @brief Construye el objeto tester contendra las preguntas y la lógica.
 * @param arch Archivo donde extraer las preguntas.
 * @param flag Tipo de preguntas que contendrá el archivo.
 * @throw runtime_error Cuando no se ha podido abrir el archivo.
 */
Tester::Tester(string arch, string flag) {
	ifstream archivo( arch );

	if (!archivo.is_open()) throw runtime_error("[Tester::Tester] No se ha podido abrir el archivo");

	string fila;

	while ( getline(archivo >> ws, fila) ) {
		if (fila.empty()) continue;
		
		if (flag == "-vf")
			recogerPreguntaVerdaderoFalso(fila);
		else if (flag == "-ab")
			recogerPreguntaAbcd(fila);
		else {
			cerr << "Error con el tipo de preguntas " << flag;
			exit(EXIT_FAILURE);
		}
	}

	archivo.close();
	progreso= preguntas.begin();
}

/**
 * @brief Función privada para recoger las preguntas de tipo Abcd.
 * @param preg Cadena de donde extraer la pregunta.
 * @pre Los enunciados deben empezar con un NÚMERO y las opnciones con una LETRA.
 * @pre La letra de la solución correcta irá en MAYUSCULAS.
 * @details Si se encuentra como primer elemento de una línea un número, almacenará esta línea como un enunciado.
 * Si se encuentra con una letra lo almacena como opción DEL ÚLTIMO ENUNCIADO y si está en mayúsculas además la almacena como respuesta correcta.
 */
void Tester::recogerPreguntaAbcd( string preg ) {
	if ( isdigit(preg[0]) )
		preguntas.push_back( new Abcd( preg ));
	else if ( preg[0] >= 'a' and preg[0] <= 'z' ) {
		if (preguntas.empty())
			throw runtime_error("[Tester::recogerPreguntaAbcd] No hay enunciado para la opción: " + preg);

		dynamic_cast<Abcd*>(preguntas.back())->añadirOpcion(preg);
	}
	else if ( preg[0] >= 'A' and preg[0] <= 'Z' ) {
		if (preguntas.empty())
			throw runtime_error("[Tester::recogerPreguntaAbcd] No hay enunciado para la opción: " + preg);

		stringstream aux;
		aux << (char)(tolower(preg[0])) << preg.substr(1);
		dynamic_cast<Abcd*>(preguntas.back())->añadirOpcion(aux.str());
		(preguntas.back())->añadirOpcionCorrecta(preg[0]);
	}
	else
		throw runtime_error("[Tester::recogerPreguntaAbcd] Formato incorrecto: " + preg);
}

/**
 * @brief Función privada para recoger las preguntas de tipo VerdaderoFalso.
 * @param preg Cadena de donde extraer la pregunta.
 * @pre Se debe empezar con el enunciado y terminar con la respuesta, separados por la cadena " - ".
 * @details Buscamos en la linea la cadena " - " y la utilizamos como separación, de tal forma que lo que hay antes lo
 * guardamos como el enunciado y lo que viene después como la respuesta. Esta última solo se guardará la primera letra
 * en mayúsculas.
 */
void Tester::recogerPreguntaVerdaderoFalso( string preg ) {
	string divisor = " - ";
	size_t pos = preg.find( divisor );

	if (pos == string::npos)
		throw runtime_error("[Tester::recogerPreguntaVerdaderaFalso] Formato de pregunta incorrecta.");

	preguntas.push_back( new VerdaderoFalso( preg.substr(0, pos) ) );
	(preguntas.back())->añadirOpcionCorrecta(toupper( preg[pos + divisor.length()] ));
}

/**
 * @brief Establece la penalización.
 * @param pen Número entero entre [1,+inf)
 * @details Por defecto la penalización es +inf, lo que significa cada infinitos fallos te resta una buena.
 */
void Tester::setPenalizacion(unsigned int pen) {
	if (pen == 0) throw runtime_error("[Tester::setPenalizacion] La penalización debe ser mayora 1");
	penalizacion = pen;
}

/**
 * @brief Genera una espera que puede utilizarse después de cada pregunta.
 * @param tiempo Tiempo entre [0, +inf)
 */
void Tester::espera(float tiempo) {
	if (tiempo<0) throw runtime_error("[Tester::espera] " + to_string(tiempo) + " no está entre [0,+inf)");

	cout << ESCONDERCURSOR;
	if (tiempo == 0) {
		cout << "Pulsa ENTER tecla para pasar a la siguiente pregunta...";
		cin.get();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	else {
		while (tiempo>0) {
			cout << LIMPIARLINEA;
			// Muestra el formato decimal en vez de científico y ajusta la precisión a dos. Por último indicamos que el
			// texto no se almacene en el buffer sino que se muestre directamente para evitar que el limpiado de línea lo alcance
			cout << "\rSiguiente pregunta en " << fixed << setprecision(2) << tiempo << " segundos" << flush;
			tiempo-=0.01;
			this_thread::sleep_for(chrono::milliseconds(10)); // Duerme por 1 milisegundo
		}
	}
	cout << MOSTRARCURSOR;
}

/**
 * @brief Comprueba si la respuesta pasada es la correcta.
 * @param respuesta Carácter a comparar
 * @details Comprueba la respuesta de la última pregunta por lo que deberá combinarse con
 * la función @b siguiente() si se quiere cambiar de pergunta.
 */
void Tester::comprobarRespuesta(char respuesta) {
	char opcionCorrecta = (*progreso)->verOpcionCorrecta();

	bool ok = toupper(respuesta) == opcionCorrecta
	if (ok) aciertos++;

    cout << (ok ? VERDE << "Respuesta correcta :D" :
				   ROJO << "Respuesta incorrecta :C, la solución era: " << opcionCorrecta) 
	<< endl << endl;

	cout << RESETEARFORMATO;
}

/**
 * @brief Muestra el progreso.
 * @param tam Tamaño de la barra de progreso, si no se indica se usa el tamaño de la terminal.
 * @return Devuelve una cadena que representa una barra de progreso.
 */
void Tester::barraProgreso(int tam = 0) {
#if defined(_WIN32) || defined(_WIN64)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
        throw runtime_error("[Tester::barraProgreso] Error al obtener el tamaño de la terminal.");

    int cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
    struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	int cols = w.ws_col;
#endif
	cols = (tam and tam <= preguntas.size()) ? tam : cols; // Si se ha pasado un tamaño, lo usamos, sino el de la terminal.
    int tamTerminal = cols - 2; // Le restamos dos por los [] del inicio y del final.

	int porcentaje = ((float)pregMostradas/numPreguntas) * 100;
	string porcentajeStr = " " + to_string(porcentaje) + "% ";
	size_t posPorcentaje = tamTerminal/2 - 2; // 2 menos porque queremos que este centrado.

	string inicio = "[";
	string barra((tamTerminal*pregMostradas)/numPreguntas, '#');
	string resto(tamTerminal - barra.length(),'.');

	inicio = inicio + barra + resto + "]";

	if (porcentaje <= 33.333)
		cout << FONDOROJO;
	else if (porcentaje > 33.333 and porcentaje <= 66.666)
		cout << FONDOAMARILLO;
	else
		cout << FONDOVERDE;

	cout << inicio.replace(posPorcentaje, porcentajeStr.length(), porcentajeStr) << RESETEARFORMATO << endl << endl;
}

/**
 * @brief Muestra el enunciado de la ÚLTIMA PREGUNTA.
 * @param activarHistorial Opción para mantener las preguntas o limpiar la pantalla después de cada una
 * @details Muestra el enunciado de la última pregunta. Además si es la primera, guarda el tiempo de inicio.
 */
void Tester::mostrarEnunciado() {
	// ESCRIBIR chcp 65001 EN TERMINAL DE WINDOW PARA QUE INTERPRETE UTF-8
    if (progreso == preguntas.begin())
    	tiempoInicio = chrono::high_resolution_clock::now();

	cout << (*progreso)->toString() << endl;

	pregMostradas++;
}

/**
 * @brief Pasa a la siguiente pregunta
 */
void Tester::operator++() {
    if (!fin())
        ++progreso;
    else
        throw out_of_range("[Tester::operator++] No hay más preguntas");
}

/**
 * @brief Desordena la lista de preguntas.
 */
void Tester::desordenar() {
	random_device rd;
	mt19937 g(rd());

	vector<Pregunta*> temp(preguntas.begin(), preguntas.end());
	ranges::shuffle(temp, g);
	preguntas.assign(temp.begin(), temp.end());
}

/**
 * @brief Muestra por pantalla un resumen enmarcado.
 * @param tam Tamaño de las preguntas, si no se indica se usa el tamaño de la lista de preguntas.
 */
void Tester::resumen(int tam = 0) {
    string lineas[] = {
			"╭────────────── Puntuación ──────────────╮",
            "├───────────────          ───────────────┤",
            "├─ Aciertos:                             │",
            "├─ Fallos:                               │",
            "├─                                       │",
            "╰────────────────────────────────────────╯",
            "✘ restan 1✔ : "
    };
	int numPreguntas = (tam and tam <= preguntas.size()) ? tam : preguntas.size();

    lineas[6] = format("{}{}", (penalizacion == INT_MAX)? "NO" : to_string(penalizacion), lineas[6]);

	auto tiempoFin = std::chrono::high_resolution_clock::now();
	float duracion = chrono::duration_cast<chrono::seconds>(tiempoFin - tiempoInicio).count();
	stringstream cronometro;
	cronometro <<   setw(2) << setfill('0') << (int)(duracion/3600) % 60 << ":" <<
			        setw(2) << setfill('0') << (int)(duracion/60) % 60 << ":" <<
			        setw(2) << setfill('0') << (int)duracion % 60;

	stringstream aciertosFinal;
	aciertosFinal << aciertos << " - "
		<< setprecision(4) << ((float)aciertos/numPreguntas)*100 << "%";

	stringstream fallosFinal;
	fallosFinal << (int)(numPreguntas - aciertos) << " - "
		<< setprecision(4) << (((float)numPreguntas - aciertos)/numPreguntas)*100 << "%";

	stringstream aciertosFallosFinal;
	aciertosFallosFinal << setprecision(2) << (aciertos - ((penalizacion != INT_MAX)? ((float)(numPreguntas - aciertos)/penalizacion) : 0)) << " - "
		<< setprecision(4) << ((aciertos - ((penalizacion != INT_MAX)? ((float)(numPreguntas - aciertos)/penalizacion) : 0))  /numPreguntas)*100 << "%";

	stringstream ventana;
	ventana << lineas[0] << endl;
	ventana << lineas[1].replace(lineas[1].find(' ')+1, cronometro.str().size(), cronometro.str()) << endl;
	ventana << lineas[2].replace(17, aciertosFinal.str().size(), aciertosFinal.str()) << endl;
	ventana << lineas[3].replace(15, fallosFinal.str().size(), fallosFinal.str()) << endl;
	ventana << lineas[4].replace(7, lineas[6].size()+aciertosFallosFinal.str().size()-4, lineas[6] + aciertosFallosFinal.str()) << endl;
	ventana << lineas[5] << endl;

    cout << LIMPIARPANTALLA << endl;
	cout << ventana.str() << endl;
}

/**
 * @brief Destructor de la clase.
 */
Tester::~Tester() {
	for (auto pregunta : preguntas)
		delete pregunta;
}

