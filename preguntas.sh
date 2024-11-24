#!/bin/bash

if [ ${#} == 0 ]; then
	echo "Error en los argumentos"
	exit
fi

sed -i 's/ - /;/g' ${1} # Cambiamos el formato de separación de ' - ' a ';' y lo SOBREESCRIBIMOS EN EL ARCHIVO

archivo=${1}
aciertos=0
total=$(wc -l $archivo | cut -d ' ' -f1)

while IFS= read -r linea # Cogemos una linea del archivo seleccionado
do
	if [[ -z "$linea" ]]; then # Si la linea está vacía pasa a la siguiente
		continue
	fi
	
	pregunta=$(echo "$linea" | cut -d ';' -f1) # Almacenamos la pregunta (se puede observar que utilizamos como delimitador el ';'
	solucion=$(echo "$linea" | cut -d ';' -f2) # Ahora la solución
	
	echo "${BLANCA}$pregunta"
	
  	read -p "${BLANCO}(V/F):" respuesta < /dev/stdout
	
	if [[ "${solucion:0:1}" == "${respuesta}" ]] #"$respuesta" ]]
	then 
		echo -e "${VERDE}Respuesta correcta\n"
		((aciertos++))
	else
		echo -e "${ROJO}Respuesta incorrecta, la solución era: $solucion\n"
	fi
done < "${archivo}"

echo "La puntuación es: "$(( ($aciertos / $total) * 100 ))
echo "------ Puntación ------"
echo "Aciertos: $aciertos"
echo "Fallos: "$(( $total-$aciertos ))
echo "Aciertos si los fallos restan: "$(( $aciertos - ($total-$aciertos) / 4))


sed -i 's/;/ - /g' ${1} # Volmemos a cambiar el formato de las preguntas para dejarlo como estaba
