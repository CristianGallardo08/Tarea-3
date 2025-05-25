GRAPHQUEST - Juego de Aventura por Escenarios

Descripción
GRAPHQUEST es un juego de aventura en C donde los jugadores exploran escenarios, recolectan ítems y resuelven desafíos para alcanzar la salida antes de que se agote el tiempo. El juego utiliza un sistema de mapas conectados y gestión de inventario.

Funcionalidades
Funcionando correctamente:
Cargar escenarios desde un archivo CSV con sus conexiones y ítems.

Moverse entre escenarios en 4 direcciones (arriba, abajo, izquierda, derecha).

Recolectar múltiples ítems de los escenarios.

Gestionar inventario (ver, descartar ítems).

Sistema de puntaje basado en valor de los ítems.

Límite de tiempo con cálculo dinámico según peso del inventario.

Mecánicas del Juego
Movimiento: Cada movimiento consume tiempo según fórmula (peso_inventario + 1) / 10 

Ítems: Añaden valor al puntaje pero aumentan peso

Tiempo: Comienza en 10 unidades y se reduce con cada acción

Victoria: Llegar al escenario final (ID 16) antes de agotar el tiempo

Tip: Prioriza ítems con alta relación valor/peso para optimizar tu puntaje final.

==MENU INICIAL===

Opcion 1 ; Cargar Laberinto

Opción seleccionada: 1) Cargar laberinto

¡Laberinto cargado correctamente!
Total de escenarios: 16

Opcion 2: Iniciar Partida:
=== ESTADO ACTUAL ===
Escenario actual: 1 (Entrada principal)
Tiempo restante: 10
Puntaje: 0

Opcion 3 : Salir del sistema 

===MENU PARTIDA==

Opciones:
1) Recoger Ítem
2) Descartar Ítem
3) Moverse a otro escenario
4) Reiniciar partida
Seleccione una opción: 

Opcion 1 : Recoger Item 

Ítems disponibles:
1) Cuchillo (Valor: 3, Peso: 1)
2) Pan (Valor: 2, Peso: 1)

Seleccione ítems a recoger: 1 2
Recogiste: Cuchillo
Recogiste: Pan

Opcion 2 : Descartar Item

Tu inventario (Valor/Peso):
1) Cuchillo (3/1)
2) Pan (2/1)

Ingrese numeros de items a descartar separados por espacios: (1 2)
Descartaste: Cuchillo
Descartaste: Pan

Opcion 3 : moverse a otro escenario 

Direcciones disponibles:        
1) Arriba
2) Abajo
3) Izquierda
4) Derecha

Seleccione una direccion (1-4): 4

Te has movido al escenario 7. Tiempo usado: 1

Opcion 4 : Reiniciar Partida 

Saliendo al menu principal...
ATENCION!! ; Al salir debe cargar el laberinto nuevamente para iniciar una nueva partida

Objetivo ; Alcanzar el Final

¡FELICIDADES! Has llegado al escenario final.
Puntaje final: ?

Ítems recolectados:
????

Compilación y Ejecución(BASH)

#COMPILAR
gcc main.c list.c map.c extra.c -o graphquest
#EJECUTAR
./graphquest


Estructura de archivos
main.c: Lógica principal del juego y menús

list.c/h: Implementación de listas enlazadas

map.c/h: Implementación de mapa/tabla hash

extra.c/h: Funciones auxiliares para CSV y UI

graphquest.csv: Datos de escenarios e ítems


