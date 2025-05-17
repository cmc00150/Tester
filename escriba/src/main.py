from google import genai
from google.genai import types
from src.utils.functions import *
import sys
import threading
import pathlib
import httpx

def main():

    if len(sys.argv) < 2:
        print("Por favor, proporciona al menos una imagen como argumento.")
        print("Uso del escritor:\n" \
                "python3 escritor.py [opción] [imagen1] [imagen2] ...\n\n" \
                "[opciones]:\n"
                "-h   -  Muestra la ayuda\n" \
                "-vf  -  Para archivos de verdadero/falso\n" \
                "-ab  -  Para archivos de preguntas con opciones\n")
        exit(1)
    elif len(sys.argv) >= 2:
        if sys.argv[1] == "-vf":
            prompt = '''Te voy a pasar una imagen con una serie de preguntas de tipo verdadero o falso. Necesito que 
                        extraigas las preguntas de la imagen. De forma que cada pregunta vaya en una fila. Primero 
                        debe de ir el enunciado y luego la respuesta correcta (que deberás resolverla tu para luego 
                        poder compararla con la del usuario más tarde), donde el enunciado y la respuesta correcta 
                        está separado por la cadena ' - ' (incluidos los espacios). Aparte, si se incluyen cosas 
                        como una tabla deberán utilizar carácteres como "\n" para que luego se vea bien sin hacer 
                        saltos de linea. '''
        elif sys.argv[1] == "-ab":
            prompt = '''Te voy a pasar una imagen con una serie de preguntas con varias opciones. Necesito que me las 
                        extraigas con el formato como en el siguiente ejemplo:

                        3) ¿Cuál de las siguientes afirmaciones referentes al planificador a largo plazo es FALSA?
                        a) El planificador a largo plazo se encarga de controlar el grado de multiprogramación.
                        b) Es el planificador típico de los sistemas de proceso por lotes.
                        c) El planificador a largo plazo se encarga de decidir qué procesos de los que están en disco son llevados a memoria principal.
                        D) El planificador a largo plazo se encarga de decidir a qué proceso se le asignará la CPU.

                        Como puedes ver, el enunciado que aparecería en la imagen comienza por el número de la pregunta, seguido 
                        de un paréntesis y un espacio. Luego, cada opción comienza por una letra seguida de un paréntesis y un 
                        espacio (la secuenca a, b, c... debe reiniciarse al terminar cada pregunta). Además, fijate que la 
                        respuesta correcta he sustituido su índice por su mayúscula (en este caso era la d pero podria ser cualquiera
                        de las otras), indicando que esa es la respuesta correta (deberás resolverlos tú). Por último, si se incluyen 
                        cosas como una tabla deberán utilizar carácteres como "\n" para que todo quede en la misma linea pero luego 
                        se imprima bien.'''
            
        else:
            print("Uso del escritor:\n" \
                    "python3 escritor.py [opción] [imagen1] [imagen2] ...\n\n" \
                    "[opciones]:\n"
                    "-h   -  Muestra la ayuda\n" \
                    "-vf  -  Para archivos de verdadero/falso\n" \
                    "-ab  -  Para archivos de preguntas con opciones\n")
            exit(1)

    API_KEY = obtener_api_key()

    if not API_KEY:
        print("Parece que eres nuevo, por favor introduce tu API Key para poder continuar.")
        nueva_api_key = input("Introduce tu API Key: ")
        guardar_api_key(nueva_api_key)
        API_KEY = nueva_api_key
        exit(0)

    cliente = genai.Client(api_key=API_KEY)

    imagenes = []
    tipo = ""
    for i in range(2, len(sys.argv)):
        tipo = sys.argv[i].split('.')[-1] 
        if tipo != "pdf": 
            with open(sys.argv[i], 'rb') as f:
                bytesImagen = f.read()
                imagenes.append(types.Part.from_bytes(
                                    data=bytesImagen, # Le pasamos la imagen en bytes
                                    mime_type=f"image/{tipo}") # Dividimos el nombre por el punto y cogemos la extensión al final
                                ) 
        else:
            docpath = pathlib.Path(sys.argv[i])
            docpath.write_bytes(httpx.get(doc_url).content)
            types.Part.from_bytes(
                data=docpath.read_bytes(),
                mime_type='application/pdf',
            )

    if not imagenes:
        print("No se han pasado imágenes, por favor, introduce al menos una imagen.")
        exit(1)

    evento_terminado = threading.Event()
    hilo = threading.Thread(target=cargando, args=(evento_terminado, [sys.argv[i] for i in range(2, len(sys.argv))]))
    hilo.start()

    try:
        respuesta = cliente.models.generate_content(model='gemini-2.5-pro-exp-03-25',
                                                    contents = [prompt] + imagenes)
    finally:
        evento_terminado.set()
        hilo.join()

    nombreArchivo = input("\nIndica el nombre del nuevo archivo: ")
    if not nombreArchivo:
        nombreArchivo = "PreguntasAbcd.txt"
    with open(nombreArchivo+".txt", mode="x", encoding="utf-8") as fichero:
        fichero.write(respuesta.text)

    print("Proceso completado, el archivo se ha guardado con el nombre: " + nombreArchivo)

if __name__ == "__main__":
    main()