#!/bin/bash
ROJO='\e[0;31m'
BLANCO='\e[1;37m'
VERDE='\e[0;32m'
FELIZ='\U1F604'
TRISTE='\U1F622'

if [ ${#} == 0 ]; then
	echo "Error en los argumentos"
	exit
fi

aciertos=0
posicion=0
total=$(wc -l ${1} | cut -d ' ' -f1)

ancho=$(tput cols) # Devuelve el ancho de la termial
printf -v barra "%$((${ancho}-2))s" # Creamos tantos espacios como ancho sea el terminal
barra=${barra// /#} # Sustituimos esos espacios por #

while IFS= read -r linea # Cogemos una linea del archivo seleccionado
do
	if [[ -z "$linea" ]]; then # Si la linea está vacía pasa a la siguiente
		continue
	fi

	((posicion++)) # Aumentamos la posición donde nos encontramos

	porcentaje=$(( ($posicion / ${#barra}) * $total )) # Calculamos el número de # para representar el porcentaje
 	printf -v barra "[%-${#barra}.${porcentaje}s]" # Alineamos a la izquierda y ponemos el resto a espacios
  	echo "${barra// /-}" # Sustituimos los espacios por los -
  
	pregunta=${linea% - *} # Almacenamos la pregunta (eliminamos la cadena desde el final hasta ' - ')
	printf -v solucion "%.1s" ${linea#* - } # Ahora la solución (desde el principio hasta el final ) y mediante printf quitamos todas las letras menos la primera
	
	printf "${BLANCO}$pregunta\n(V/F): "
  	read respuesta < /dev/stdout
	
	if [[ ${solucion^} == ${respuesta^} ]] # Pasamos ambos de minúscular a mayusculas para que tengan el mismo formato y lo comparamos
	then 
		echo -e "${VERDE}Respuesta correcta ${FELIZ} \n"
		((aciertos++))
	else
		echo -e "${ROJO}Respuesta incorrecta ${TRISTE}, la solución era: $solucion\n"
	fi

  	sleep "1.3"
   	clear
done < "${1}"

awk -v a="$aciertos" -v b="$total" 'BEGIN {printf "La puntuación final sobre 100 es: %.2f \n",(a/b)*100}' # Utilizamos awk porque tiene más precisión en la división
echo "------ Detalles ------"
echo "Aciertos: $aciertos" # Muestra los aciertos
echo "Fallos: "$(( $total-$aciertos )) # Muestra los fallos (total-aciertos)
echo "Aciertos si los fallos restan: "$(( $aciertos - ($total-$aciertos) / 4)) # Muestra los aciertos suponiendo que cada 4 fallos quitan un acierto
