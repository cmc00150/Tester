#!/bin/bash

if [ ${#} == 0 ]; then
	echo "Error en los argumentos"
	exit
fi

sed -i 's/ - /|/g' ${1} # Cambiamos el formato de separación de ' - ' a ';' y lo SOBREESCRIBIMOS EN EL ARCHIVO

archivo=${1}
aciertos=0
posicion=0
total=$(wc -l $archivo | cut -d ' ' -f1)

while IFS= read -r linea # Cogemos una linea del archivo seleccionado
do
	if [[ -z "$linea" ]]; then # Si la linea está vacía pasa a la siguiente
		continue
	fi

	((posicion++)) # Aumentamos la posición donde nos encontramos
 
	pregunta=$(echo "$linea" | cut -d '|' -f1) # Almacenamos la pregunta (se puede observar que utilizamos como delimitador el ';'
	solucion=$(echo "$linea" | cut -d '|' -f2) # Ahora la solución
	
	awk -v a="$posicion" -v b="$total" 'BEGIN {printf "[%.0f%] - ",(a/b)*100}' # Muestra [x%] delante de la pregunta para mostrar el progreso
	echo $pregunta # Muestra la pregunta

  	read -p "${BLANCO}(V/F):" respuesta < /dev/stdout
	
	if [[ $(echo "${solucion:0:1}" | tr '[:lower:]' '[:upper:]' ) == $(echo "${respuesta}" | tr '[:lower:]' '[:upper:]' ) ]] # Pasamos ambos de minúscular a mayusculas para que tengan el mismo formato y lo comparamos
	then 
		echo -e "${VERDE}Respuesta correcta\n"
		((aciertos++))
	else
		echo -e "${ROJO}Respuesta incorrecta, la solución era: $solucion\n"
	fi
done < "${archivo}"

awk -v a="$aciertos" -v b="$total" 'BEGIN {printf "La puntuación final sobre 100 es: %.2f \n",(a/b)*100}' # Utilizamos awk porque tiene más precisión en la división
echo "------ Detalles ------"
echo "Aciertos: $aciertos" # Muestra los aciertos
echo "Fallos: "$(( $total-$aciertos )) # Muestra los fallos (total-aciertos)
echo "Aciertos si los fallos restan: "$(( $aciertos - ($total-$aciertos) / 4)) # Muestra los aciertos suponiendo que cada 4 fallos quitan un acierto


sed -i 's/|/ - /g' ${1} # Volmemos a cambiar el formato de las preguntas para dejarlo como estaba
