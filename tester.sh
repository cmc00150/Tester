#!/bin/bash
ROJO='\e[0;31m'
BLANCO='\e[1;37m'
VERDE='\e[0;32m'
FELIZ='\U1F604'
TRISTE='\U1F622'

function help(){
	cat <<'EOF'
		SINTAXIS:
			tester.sh [OPTION]... Documento
		
		OPCIONES:
			--random
					Muestra las preguntas de forma aleatoria (default: forma ordenada).
			
			--time
					Establece el tiempo de delay entre pregunta y pregunta (default: 1), debe ser mayor que 0.
			
			--penalty
					Establece cada cuantos fallos restan una buena (default: 1), debe ser mayor que 1.
EOF
}

function almacenarPreguntas()
{
	while IFS= read -r linea; do
		if [ -z "$linea" ]; then continue; fi # Si la línea está vacía pasa a la siguiente
	
		preguntas+=("$linea")  # Almacenamos las preguntas en el vector
	done < "${1}"
}

function orden() 
{
	if [ -z ${1} ]; then # Si el argumento no es una cadena vacía
		seq 0 $(($total-1)) # Devuelve una lista ordenada
	else
		seq 0 $((${1}-1)) | shuf # Devuelve una lista desordenada
	fi
}

function mostrarBarraPorcentaje()
{
	local colorLetra=$(tput setaf 7)
	local pocoRecorrido=$(tput setab 1)
	local medioRecorrido=$(tput setab 3)
	local altoRecorrido=$(tput setab 2)

	local ancho=$(tput cols) # Devuelve el ancho de la termial
	printf -v barra "%$((${ancho}-2))s" # Creamos tantos espacios como ancho sea el terminal (menos 2 para poner los [])
	local barra=${barra// /#} # Sustituimos esos espacios por #

	local porcentaje=$(( (${#barra} / ${total} ) * $posicion )) # Calculamos el número de # para representar el porcentaje
	printf -v tramo "[%-${#barra}.$((${porcentaje}+1))s]" $barra # Alineamos a la izquierda y ponemos el resto a espacios
	
	if [ $porcentaje -le $(($ancho/3)) ]; then
 	  	echo -n $pocoRecorrido$colorLetra
 	fi
 	if [ $porcentaje -gt $(($ancho/3)) ] && [ $porcentaje -le $(($ancho/3*2)) ]; then
 	  	echo -n $medioRecorrido$colorLetra
 	fi
 	if [ $porcentaje -gt $(($ancho/3*2)) ]; then
 	  	echo -n $altoRecorrido$colorLetra
 	fi
 	
	printf "${tramo// /-}" # Sustituimos los espacios por los -
	tput sgr0 # Limpiamos el estilo
}

function mostrarPregunta()
{
	local pregunta=${1% - *} # Almacenamos la pregunta (eliminamos la cadena desde el final hasta ' - ')
	printf -v solucion "%.1s" ${1#* - } # Ahora la solución (desde el principio hasta el final ) y mediante printf quitamos todas las letras menos la primera
	
	echo -ne "${BLANCO}$pregunta\n(V/F): "
	read respuesta
	
	if [[ ${solucion^} == ${respuesta^} ]] # Pasamos ambos de minúscular a mayusculas para que tengan el mismo formato y lo comparamos
	then 
		echo -e "${VERDE}Respuesta correcta ${FELIZ}\n"
		((aciertos++))
	else
		echo -e "${ROJO}Respuesta incorrecta ${TRISTE}, la solución era: ${1#* - }\n"
	fi
}

function cooldown()
{
	tput civis # Escondemos el cursor
	
	local delayL=$delay

	while [ $delayL != 0 ]; do
		delayL=$(awk -v t="$delayL" 'BEGIN {print t - 0.01}')
		echo -ne "\rSiguiente pregunta ${delayL} segundos."
		sleep 0.0045 # Rellenamos el tiempo que nos sobra para que llegue a 0.01 cada iteración más o menos
	done

	tput cnorm # Volvemos a mostrar el cursor
}

function puntuacion()
{	
	echo -e "${BLANCO}"
	echo "------ Puntuación ------"
	awk -v a="$aciertos" -v b="$total" 'BEGIN {printf "Aciertos: %d - %.2f\n", a,  (a/b)*100}' # Aciertos (utilizamos awk porque tiene más precisión en la división)
	awk -v a="$aciertos" -v b="$total" 'BEGIN {printf "Fallos: %d - %.2f\n", b-a,  ((b-a)/b)*100}' # Fallos
	awk -v p="$penalizacion" -v a="$aciertos" -v b="$total" 'BEGIN {printf "Aciertos si los fallos restan: %.1f - %.2f\n", a-((b-a)/p), ((a-((b-a)/p))/b)*100}' # Mostramos la punt. teniendo en cuenta la penalización
}

if [[ ! -r ${!#} || ${1} =~ [0-9] || $# -eq 0 ]]; then # Si el primer argumento es un número o el ultimo no es un fichero da error
	help
	exit
fi

aciertos=0
posicion=0
total=$(wc -l < "${!#}")

penalizacion=1
delay=1
ordenPreguntas=($( orden )) # Por defecto lo ponemos ordenada

preguntas=()
almacenarPreguntas "${!#}" # Le pasamos la pregunta (el último argumento)

for((arg=1; arg<$#; arg++)); do # Miramos las opciones que deben de estar antes del último documento
	if [ -r ${!arg} ]; then # Si es un directorio pero no está en la última posición da un error
		help
		exit
	fi

	siguiente=$(( $arg + 1))

	case ${!arg} in

		--random)
			if [[ ${!siguiente} =~ [0-9] ]]; then help; exit; fi # Random no acepta argumentos
			ordenPreguntas=($( orden $total ))
		;;
		--time)
			if [[ ${!siguiente} =~ [^0-9] || ${!siguiente} < 0 ]]; then help; exit; fi # Debe pasarse un número y que sea mayor que 0
			delay=${!siguiente}
		;;
		--penalty)
			if [[ ${!siguiente} =~ [^0-9] || ${!siguiente} < 1 ]]; then help; exit; fi # Debe pasarse un número y que sea mayor que 1
			penalizacion=${!siguiente}
		;;
		[0-9]) # Si es un número no hacemos nada
		;;
		*)
			help
			exit
		;;
	esac
done

for indice in ${ordenPreguntas[@]}
do
	((posicion++)) # Aumentamos la posición donde nos encontramos
  
	mostrarBarraPorcentaje
	
	mostrarPregunta "${preguntas[$indice]}" # Le pasamos la pregunta
	
	cooldown
	
	tput clear # Limpiamos la pantalla
done

puntuacion