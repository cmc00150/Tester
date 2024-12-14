#!/bin/bash
ROJO='\e[0;31m'
BLANCO='\e[1;37m'
VERDE='\e[0;32m'
FELIZ='\U1F604'
TRISTE='\U1F622'

function almacenarPreguntas()
{
	while IFS= read -r linea; do
		if [ -z "$linea" ]; then continue; fi # Si la linea está vacía pasa a la siguiente
	
		preguntas+=("$linea")  # Almacenamos las preguntas en el vector
	done < ${1}
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
	
	local tiempo=1	
	while [ $tiempo != 0 ]; do
		tiempo=$(awk -v t="$tiempo" 'BEGIN {print t - 0.01}')
		echo -ne "\rSiguiente pregunta ${tiempo} segundos."
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
	awk -v a="$aciertos" -v b="$total" 'BEGIN {printf "Aciertos si los fallos restan: %.1f - %.2f\n", a-((b-a)/4), ((a-((b-a)/4))/b)*100}' # Mostramos la punt. teniendo en cuenta la penalización
}

if [ ${#} -eq 0 ]; then
	echo "Error en los argumentos"
	exit
fi

aciertos=0
posicion=0
total=$(wc -l < "${1}")
preguntas=()

almacenarPreguntas ${1} # Le pasamos la pregunta

for pregunta in "${preguntas[@]}"; do

	tput sc # Guardamos la posición inicial del cursor antes de que se muestre la pregunta

	((posicion++)) # Aumentamos la posición donde nos encontramos
  
	mostrarBarraPorcentaje
	
	mostrarPregunta "$pregunta" # Le pasamos la pregunta
	
	cooldown
	
	tput clear # Limpiamos la pantalla
	
done

puntuacion