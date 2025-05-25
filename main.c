#include "extra.h"
#include "list.h"
#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Estructura para representar un ítem
typedef struct
{
    char *nombre;
    int valor;
    int peso;
} Item;

/* Estructura para representar un escenario/lugar del juego:
   - id: (string)
   - nombre: Título del escenario
   - descripcion: Texto descriptivo
   - items: Lista de ítems disponibles en el escenario
   - arriba/abajo/izquierda/derecha: IDs de escenarios adyacentes
   - esFinal: Bandera que indica si es el escenario final */
typedef struct
{
    char *id; // ID como string
    char *nombre;
    char *descripcion;
    List *items;
    char *arriba;
    char *abajo;
    char *izquierda;
    char *derecha;
    bool esFinal;
} Escenario;

/* Estructura para el estado del jugador:
   - escenario_actual: ID del escenario donde se encuentra
   - inventario: Lista de ítems recolectados
   - tiempo_restante: Unidades de tiempo disponibles
   - puntaje: Puntos acumulados */
typedef struct
{
    char *escenario_actual;
    List *inventario;
    int tiempo_restante;
    int puntaje;
} Jugador;

int is_equal_str(void *key1, void *key2)
{
    return strcmp((char *)key1, (char *)key2) == 0;
}

/* Crea un nuevo ítem con memoria dinámica:
   - nombre: String a copiar
   - valor: Puntos del ítem
   - peso: Costo del ítem
   Retorna puntero al ítem creado */

Item *crear_item(char *nombre, int valor, int peso)
{
    Item *item = (Item *)malloc(sizeof(Item));
    item->nombre = strdup(nombre);
    item->valor = valor;
    item->peso = peso;
    return item;
}
/* Libera la memoria de un ítem */
void liberar_item(void *item)
{
    Item *i = (Item *)item;
    free(i->nombre);
    free(i);
}
/* Crea un nuevo escenario con memoria dinámica:
   - id, nombre, descripcion: Strings a copiar
   Retorna puntero al escenario creado */
Escenario *crear_escenario(char *id, char *nombre, char *descripcion)
{
    Escenario *esc = (Escenario *)malloc(sizeof(Escenario));
    esc->id = strdup(id);
    esc->nombre = strdup(nombre);
    esc->descripcion = strdup(descripcion);
    esc->items = list_create();
    esc->arriba = NULL;
    esc->abajo = NULL;
    esc->izquierda = NULL;
    esc->derecha = NULL;
    esc->esFinal = false;
    return esc;
}

/* Libera completamente la memoria de un escenario:
   - Libera todos los strings y listas internas
   - Libera todos los ítems contenidos */

void liberar_escenario(void *escenario)
{
    Escenario *esc = (Escenario *)escenario;
    free(esc->id);
    free(esc->nombre);
    free(esc->descripcion);

    // Liberar direcciones si existen
    if (esc->arriba != NULL)
        free(esc->arriba);
    if (esc->abajo != NULL)
        free(esc->abajo);
    if (esc->izquierda != NULL)
        free(esc->izquierda);
    if (esc->derecha != NULL)
        free(esc->derecha);

    // Liberar todos los items del escenario
    void *item = list_first(esc->items);
    while (item != NULL)
    {
        liberar_item(item);
        item = list_next(esc->items);
    }
    list_clean(esc->items);
    free(esc->items);

    free(esc);
}

/* Carga los escenarios desde un archivo CSV:
   - filename: Ruta del archivo a cargar
   Retorna un Map con todos los escenarios cargados
   Formato CSV esperado:
   ID,Nombre,Descripción,Items,Arriba,Abajo,Izquierda,Derecha,EsFinal */

Map *cargar_escenarios(const char *filename)
{
    FILE *archivo = fopen(filename, "r");
    if (archivo == NULL)
    {
        perror("Error al abrir el archivo");
        return NULL;
    }

    Map *escenarios = map_create(is_equal_str);
    char **campos;

    // Leer encabezados
    campos = leer_linea_csv(archivo, ',');

    while ((campos = leer_linea_csv(archivo, ',')) != NULL)
    {
        char *id = campos[0];
        char *nombre = campos[1];
        char *descripcion = campos[2];

        Escenario *esc = crear_escenario(id, nombre, descripcion);

        // Procesar items
        if (strlen(campos[3]) > 0)
        {
            List *items_list = split_string(campos[3], ";");
            char *item_str = list_first(items_list);
            while (item_str != NULL)
            {
                List *item_data = split_string(item_str, ",");
                char *item_name = list_first(item_data);
                char *item_value_str = list_next(item_data);
                char *item_weight_str = list_next(item_data);

                int item_value = atoi(item_value_str);
                int item_weight = atoi(item_weight_str);

                Item *item = crear_item(item_name, item_value, item_weight);
                list_pushBack(esc->items, item);

                list_clean(item_data);
                free(item_data);
                item_str = list_next(items_list);
            }
            list_clean(items_list);
            free(items_list);
        }

        // Procesar direcciones (convertir "-1" a NULL)
        esc->arriba = (strcmp(campos[4], "-1") == 0) ? NULL : strdup(campos[4]);
        esc->abajo = (strcmp(campos[5], "-1") == 0) ? NULL : strdup(campos[5]);
        esc->izquierda = (strcmp(campos[6], "-1") == 0) ? NULL : strdup(campos[6]);
        esc->derecha = (strcmp(campos[7], "-1") == 0) ? NULL : strdup(campos[7]);

        // Procesar si es final
        esc->esFinal = (strcmp(campos[8], "Si") == 0);

        // Insertar en el mapa usando el ID como clave
        map_insert(escenarios, esc->id, esc);
    }

    fclose(archivo);
    return escenarios;
}

void mostrar_menu_principal()
{
    limpiarPantalla();
    puts("========================================");
    puts("           JUEGO GRAPHQUEST");
    puts("========================================");
    puts("1) Cargar laberinto");
    puts("2) Iniciar partida");
    puts("3) Salir");
}

void mostrar_menu_juego()
{
    puts("\nOpciones:");
    puts("1) Recoger Iem");
    puts("2) Descartar Item");
    puts("3) Moverse a otro escenario");
    puts("4) Reinciar partida");
    printf("Seleccione una opcion: ");
}

/* Permite al jugador recoger uno o más ítems del escenario actual:
   - escenarios: Mapa con todos los escenarios
   - jugador: Estado actual del jugador
   - Descuenta 1 unidad de tiempo por acción */

void recoger_item(Map *escenarios, Jugador *jugador)
{
    MapPair *pair = map_search(escenarios, jugador->escenario_actual);
    if (pair == NULL)
    {
        printf("Error: Escenario no encontrado.\n");
        return;
    }

    Escenario *esc_actual = (Escenario *)pair->value;

    if (list_size(esc_actual->items) == 0)
    {
        printf("No hay items aqui.\n");
        return;
    }

    printf("\nItems disponibles (Valor/Peso):\n");
    int index = 1;
    Item *item = list_first(esc_actual->items);
    while (item != NULL)
    {
        printf("%d) %s (%d/%d)\n", index, item->nombre, item->valor, item->peso);
        item = list_next(esc_actual->items);
        index++;
    }

    printf("\nIngrese numeros de Items separados por espacios (ej: 1 3): ");
    char input[100];
    fgets(input, sizeof(input), stdin);

    int seleccion;
    char *token = strtok(input, " ");
    bool items_recogidos = false;
    List *items_a_eliminar = list_create();

    while (token != NULL)
    {
        if (sscanf(token, "%d", &seleccion) == 1)
        {
            if (seleccion >= 1 && seleccion <= list_size(esc_actual->items))
            {
                // Reiniciamos el puntero al inicio para cada selección
                item = list_first(esc_actual->items);
                for (int i = 1; i < seleccion; i++)
                {
                    item = list_next(esc_actual->items);
                }

                if (item != NULL)
                {
                    Item *nuevo_item = crear_item(item->nombre, item->valor, item->peso);
                    list_pushBack(jugador->inventario, nuevo_item);
                    jugador->puntaje += item->valor;
                    printf("Recogiste: %s\n", item->nombre);

                    list_pushBack(items_a_eliminar, item);
                    items_recogidos = true;
                }
            }
        }
        token = strtok(NULL, " ");
    }

    item = list_first(items_a_eliminar);
    while (item != NULL)
    {

        Item *current = list_first(esc_actual->items);
        while (current != NULL)
        {
            if (current == item)
            {
                list_popCurrent(esc_actual->items);
                break;
            }
            current = list_next(esc_actual->items);
        }
        item = list_next(items_a_eliminar);
    }

    list_clean(items_a_eliminar);
    free(items_a_eliminar);

    if (items_recogidos)
    {
        jugador->tiempo_restante--;
    }
}

/* Permite al jugador descartar ítems de su inventario:
   - jugador: Estado actual del jugador
   - Descuenta 1 unidad de tiempo por acción */

void descartar_item(Jugador *jugador)
{
    if (list_size(jugador->inventario) == 0)
    {
        printf("Tu inventario esta vacio.\n");
        return;
    }

    printf("\nTu inventario (Valor/Peso):\n");
    int index = 1;
    Item *item = list_first(jugador->inventario);
    while (item != NULL)
    {
        printf("%d) %s (%d/%d)\n", index, item->nombre, item->valor, item->peso);
        item = list_next(jugador->inventario);
        index++;
    }

    printf("\nIngrese numeros de items a descartar separados por espacios: ");
    char input[100];
    fgets(input, sizeof(input), stdin);

    int seleccion;
    char *token = strtok(input, " ");
    bool items_descartados = false;
    List *items_a_eliminar = list_create();

    while (token != NULL)
    {
        if (sscanf(token, "%d", &seleccion) == 1)
        {
            if (seleccion >= 1 && seleccion <= list_size(jugador->inventario))
            {
                item = list_first(jugador->inventario);
                for (int i = 1; i < seleccion; i++)
                {
                    item = list_next(jugador->inventario);
                }

                if (item != NULL)
                {
                    printf("Descartaste: %s\n", item->nombre);
                    jugador->puntaje -= item->valor;
                    list_pushBack(items_a_eliminar, item);
                    items_descartados = true;
                }
            }
        }
        token = strtok(NULL, " ");
    }

    item = list_first(items_a_eliminar);
    while (item != NULL)
    {
        Item *current = list_first(jugador->inventario);
        while (current != NULL)
        {
            if (current == item)
            {
                list_popCurrent(jugador->inventario);
                liberar_item(current);
                break;
            }
            current = list_next(jugador->inventario);
        }
        item = list_next(items_a_eliminar);
    }

    list_clean(items_a_eliminar);
    free(items_a_eliminar);

    if (items_descartados)
    {
        jugador->tiempo_restante--;
    }
}

/* Calcula el peso total del inventario del jugador */

int calcular_peso_total(List *inventario)
{
    int peso_total = 0;
    Item *item = list_first(inventario);
    while (item != NULL)
    {
        peso_total += item->peso;
        item = list_next(inventario);
    }
    return peso_total;
}

/* Mueve al jugador a otro escenario:
   - escenarios: Mapa con todos los escenarios
   - jugador: Estado actual del jugador
   - El tiempo usado se calcula como: (peso_total + 1) / 10 */

void mover_jugador(Map *escenarios, Jugador *jugador)
{

    MapPair *pair = map_search(escenarios, jugador->escenario_actual);
    if (pair == NULL)
    {
        printf("Error: No se encontro el escenario actual.\n");
        return;
    }
    Escenario *actual = (Escenario *)pair->value;

    printf("\nDirecciones disponibles:\n");
    if (actual->arriba)
        printf("1) Arriba\n");
    if (actual->abajo)
        printf("2) Abajo\n");
    if (actual->izquierda)
        printf("3) Izquierda\n");
    if (actual->derecha)
        printf("4) Derecha\n");

    printf("\nSeleccione una direccion (1-4): ");
    char direccion;
    scanf(" %c", &direccion);
    getchar();

    char *id_destino = NULL;
    switch (direccion)
    {
    case '1':
        id_destino = actual->arriba;
        break;
    case '2':
        id_destino = actual->abajo;
        break;
    case '3':
        id_destino = actual->izquierda;
        break;
    case '4':
        id_destino = actual->derecha;
        break;
    default:
        printf("Direccion no valida.\n");
        return;
    }

    if (id_destino == NULL)
    {
        printf("No hay camino en esa direccion.\n");
        return;
    }

    int peso_total = 0;
    Item *item = list_first(jugador->inventario);
    while (item != NULL)
    {
        peso_total += item->peso;
        item = list_next(jugador->inventario);
    }

    int tiempo_usado = (peso_total + 1) / 10;
    if (tiempo_usado < 1)
        tiempo_usado = 1;

    if (jugador->tiempo_restante < tiempo_usado)
    {
        printf("No tienes suficiente tiempo para moverte.\n");
        return;
    }

    free(jugador->escenario_actual);
    jugador->escenario_actual = strdup(id_destino);
    jugador->tiempo_restante -= tiempo_usado;

    printf("\nTe has movido al escenario %s. Tiempo usado: %d\n", id_destino, tiempo_usado);

    pair = map_search(escenarios, id_destino);
    if (pair != NULL)
    {
        Escenario *destino = (Escenario *)pair->value;
        if (destino->esFinal)
        {
            printf("\n¡FELICIDADES! Has llegado al escenario final.\n");
            printf("Puntaje final: %d\n", jugador->puntaje);

            if (list_size(jugador->inventario) > 0)
            {
                printf("\nItems recolectados:\n");
                item = list_first(jugador->inventario);
                while (item != NULL)
                {
                    printf("- %s (Valor: %d, Peso: %d)\n",
                           item->nombre, item->valor, item->peso);
                    item = list_next(jugador->inventario);
                }
            }
            else
            {
                printf("\nNo recolectaste ningun item.\n");
            }

            jugador->tiempo_restante = 0;
        }
    }
}

/* Función principal que controla una partida:
   - escenarios: Mapa con los escenarios cargados
   - Maneja el bucle principal del juego
   - Controla el flujo de acciones del jugador */

void iniciar_partida(Map *escenarios)
{
    if (escenarios == NULL)
    {
        printf("Error: No se ha cargado ningún laberinto.\n");
        presioneTeclaParaContinuar();
        return;
    }

    Jugador *jugador = (Jugador *)malloc(sizeof(Jugador));
    jugador->escenario_actual = strdup("1");
    jugador->inventario = list_create();
    jugador->tiempo_restante = 10;
    jugador->puntaje = 0;

    char opcion;
    bool en_partida = true;

    while (en_partida && jugador->tiempo_restante > 0)
    {
        limpiarPantalla();

        printf("=== ESTADO ACTUAL ===\n");
        printf("Escenario actual: %s\n", jugador->escenario_actual);
        printf("Tiempo restante: %d\n", jugador->tiempo_restante);
        printf("Puntaje: %d\n", jugador->puntaje);

        mostrar_menu_juego();

        scanf(" %c", &opcion);
        getchar();

        switch (opcion)
        {
        case '1':
            recoger_item(escenarios, jugador);
            break;
        case '2':
            descartar_item(jugador);
            break;
        case '3':
            mover_jugador(escenarios, jugador);
            break;
        case '4':
            en_partida = false;
            printf("\nSaliendo al menu principal...\n");
            printf("ATENCION!! ; Al salir debe cargar el laberinto nuevamente para iniciar una nueva partida\n");
            break;
        default:
            printf("\nOpcion no valida.\n");
        }

        if (jugador->tiempo_restante > 0)
        {
            presioneTeclaParaContinuar();
        }
        else
        {
            printf("\n¡Se te acabo el tiempo! Juego terminado.\n");
        }
    }

    // Liberar recursos del jugador
    free(jugador->escenario_actual);
    list_clean(jugador->inventario);
    free(jugador->inventario);
    free(jugador);

    if (jugador->tiempo_restante <= 0)
    {
        presioneTeclaParaContinuar();
    }
}

int main()
{
    Map *escenarios = NULL;
    char opcion;

    do
    {
        mostrar_menu_principal();
        printf("Ingrese su opcion: ");
        scanf(" %c", &opcion);
        getchar();

        switch (opcion)
        {
        case '1':
            if (escenarios != NULL)
            {
                map_clean(escenarios);
                free(escenarios);
            }
            escenarios = cargar_escenarios("graphquest.csv");
            if (escenarios != NULL)
            {
                printf("Laberinto cargado correctamente.\n");
            }
            presioneTeclaParaContinuar();
            break;
        case '2':
            if (escenarios == NULL)
            {
                printf("Primero debe cargar un laberinto.\n");
                presioneTeclaParaContinuar();
            }
            else
            {
                iniciar_partida(escenarios);
            }
            break;
        case '3':
            printf("Saliendo del sistema...");
            break;
        default:
            printf("Opcion no valida.\n");
            presioneTeclaParaContinuar();
        }
    } while (opcion != '3');

    return 0;
}