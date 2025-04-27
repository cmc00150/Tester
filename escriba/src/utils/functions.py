import platform
import os
from time import sleep

def cargando(evento_terminado, archivos):
    print("Procesando los archivos: " + str(archivos))
    while not evento_terminado.is_set():
        estados = ['Cargando   ', 'Cargando.  ', 'Cargando.. ', 'Cargando...']
        for estado in estados:
            print(estado, end='\r')
            sleep(0.5)

def guardar_api_key(api_key: str):
    # Guarda la API Key en una variable de entorno de forma persistente.
    
    sistema = platform.system()

    if sistema == "Windows":
        # En Windows, se usa setx para guardar de forma persistente
        os.system(f'setx GEMINI_API_KEY "{api_key}"')
        print("API Key guardada. Reinicia la terminal para aplicar los cambios.")
    else:
        # En Mac/Linux, se guarda en ~/.bashrc o ~/.zshrc (según el shell)
        shell_config = os.path.expanduser("~/.bashrc")  # Bash por defecto
        if os.path.exists(os.path.expanduser("~/.zshrc")):
            shell_config = os.path.expanduser("~/.zshrc")  # Zsh si existe

        with open(shell_config, "a") as file:
            file.write(f'\nexport GEMINI_API_KEY="{api_key}"\n')

        print(f"API Key guardada en {shell_config}.")
        # También aplicar la variable al entorno actual
        os.environ["GEMINI_API_KEY"] = api_key
        print("La API Key se ha configurado para esta sesión.")

def obtener_api_key():
    # Obtiene la API Key desde la variable de entorno
    api_key = os.getenv("GEMINI_API_KEY")
    return api_key