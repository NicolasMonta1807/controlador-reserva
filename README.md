# Controlador de reservas

## Autores
* Juanita Franco Sánchez
* Gabriel Espitia Romero
* Nicolás Montañez Velasco
* Andŕes Felipe Ruge Passito

## Compilación
El proyecto cuenta con un `makefile` que contiene todas las instrucciones de compilación, por lo que una vez dentro de la carpeta raíz del proyecto, solo basta con ejecutar:

```bash
make
```

## Ejecución
Luego de compilar ambos programas, puede ejecutarlos de la siguiente forma:

* Controlador:
```bash
./bin/controller -i horaInicio -f horaFinal -s segundosXhora -t aforoMaximo -p nombrePipe
```
  Como ejemplo de esta ejecución puede usar:
```bash
./bin/controller -i 7 -f 19 -s 8 -t 20 -p nominal
```

* Agente:
```bash
./bin/agent -s nombreAgente -a archivoSolicitudes -p nombrePipe
```
  Como ejemplo de esta ejecución puede usar:
```bash
./bin/agent -a solicitudes.txt -s agente420 -p hola
```

Recuerde que el nombre del pipe debe coincidir entre ambos programas.

