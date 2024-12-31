# Tester
Se trata de un script en bash para estudiar con preguntas de verdadero o falso desde el terminal de bash en una distribución linux.
## Uso
#### 1. ⬇️ Instalacion
Descarga el repositorio mediante `git clone https://github.com/cmc00150/Tester.git` en el directorio deseado o descargandolo como un **.zip** directamente al final del boton verde de _"code"_
#### 2. ⚠️ Preparación del documento
- Deberan tener la forma "PREGUNTA - SOLUCIÓN", muy importante que el '-' **vaya separado** por dos espacios antes y después del mismo ( - ).
- Toda la pregunta deberá estar en la misma linea. _(si es totalmente necesario que el enunciado tenga saltos de linea, deberá hacerse mediante carácteres de escape \n)_
- Debe dejarse una última linea en blanco al final del documento debajo de la última pregunta, si el documento termina con la última pregunta entonces esta no se mostrará.
- Al introducir operaciones matemáticas hay que asegurarse de que tengan un formato plano, para ello lo mejor es copiarla a mano con el teclado (en vez de copiar y pegar).
#### 3. ✅ Ejecución
Llamamos a la función _tester.sh_ (si estamos dentro de la carpeta descargada será `./tester.sh`) y le pasamos como argumento el documento. Por ejemplo: `./tester.sh PreguntasSTI.txt`
## Opciones
Actualmente hay 3 opciones a elegir:
```sh
./tester --random --penalty 3 --time 4 PreguntasSTI.txt
```
> Tip: Puedes poner './tester.sh' para ver las opciones en cualquier momento:
```
OPTIONS:
  --random      Muestra las preguntas de forma aleatoria (default: ordenado)
  --penalty     Establece cuantos fallos restan un acierto (default: 1)
  --time        Configura el tiempo entre pregunta y pregunta (default: 1)
```
