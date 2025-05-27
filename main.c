#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define M 64  // Cantidad de baldes
#define R 4   // Ranuras por balde
#define ARCHIVO "vendedores.dat"    //nombre del archivo

// Estructura para un vendedor
typedef struct {
    int dni;
    char nombre[51];
    char telefono[16];
    float valor;
    int cantVendida;
    char tipoVenta[21];
    int estado;  // 0 = libre, 1 = ocupado
} Vendedor;

// Estructura para un balde con R ranuras
typedef struct {
    Vendedor ranuras[R];
} Balde;

// Estructura auxiliar para manipulacion en memoria
typedef struct {
    int posicionBalde;
    int posicionRanura;
} Posicion;

int altaEnDisco();
int bajaEnDisco();
Vendedor evocarEnDisco();
void consultaBaldeContenedor();
void mostrarEstructura();
void memorizarDesdeArchivo();
void analisisCostos();

void menu() {
    int opcion, dni, pos;
    Vendedor vendedorAux;
    do {
        printf("\n===== MENU PRINCIPAL - SISTEMA DE VENDEDORES =====\n");
        printf("1. Ingreso de nuevos vendedores\n");
        printf("2. Eliminacion de vendedores existentes\n");
        printf("3. Consulta de vendedores\n");
        printf("4. Consulta de balde contenedor\n");
        printf("5. Mostrar estructura completa\n");
        printf("6. Memorizacion previa desde archivo 'Vendedores.txt'\n");
        printf("7. Analisis de costos (usando 'Operaciones.txt')\n");
        printf("0. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        getchar(); // limpiar buffer

        switch (opcion) {
            case 1:
                altaEnDisco(vendedorAux);
                break;
            case 2:
                bajaEnDisco(vendedorAux);
                break;
            case 3:
                evocarEnDisco(dni);
                break;
            case 4:
                consultaBaldeContenedor();
                break;
            case 5:
                mostrarEstructura();
                break;
            case 6:
                memorizarDesdeArchivo();
                break;
            case 7:
                analisisCostos();
                break;
            case 0:
                printf("Saliendo del programa...\n");
                break;
            default:
                printf("Opcion invalida. Intente nuevamente.\n");
        }
    } while (opcion != 0);
}

int main()
{
    menu();
    return 0;
}

void inicializarArchivoRebalse() {
    FILE* archivo = fopen(ARCHIVO, "wb");
    if (archivo == NULL) {
        printf("Error al crear el archivo de rebalse.\n");
        exit(1);
    }

    Vendedor vendedorNulo = {0, "", "", 0.0f, 0, "", -1};  // estado = -1 (virgen)

    for (int i = 0; i < M * R; i++) {
        fwrite(&vendedorNulo, sizeof(Vendedor), 1, archivo);
    }

    fclose(archivo);
    printf("Archivo de rebalse inicializado correctamente.\n");
}


// -------------------- LOCALIZAR ----------------------
// Retorna: 1 si existe, 0 si no existe, -1 si lleno. Pos retorna la posicion
int localizarEnDisco(int dni, int *pos) {
    FILE *f = fopen(ARCHIVO, "rb");
    if (!f) return -1;

    int h = hashing(dni);
    int i = 0, MAux = -1;
    Vendedor v;

    while (i < M * R) {
        fseek(f, sizeof(Vendedor) * h, SEEK_SET);
        fread(&v, sizeof(Vendedor), 1, f);

        if (v.estado == -1) break; // celda virgen
        if (v.estado == 0 && MAux == -1) MAux = h;
        if (v.estado == 1 && v.dni == dni) {
            *pos = h;
            fclose(f);
            return 1; // Encontrado
        }

        h = (h + 1) % (M * R);
        i++;
    }

    if (i >= M * R && MAux == -1) {
        fclose(f);
        return -1; // lleno
    }

    *pos = (MAux != -1) ? MAux : h;
    fclose(f);
    return 0;
}

// -------------------- ALTA ----------------------
int altaEnDisco(Vendedor nuevo) {
    int pos;
    int res = localizarEnDisco(nuevo.dni, &pos);
    if (res != 0) return 0;

    FILE *f = fopen(ARCHIVO, "rb+");
    if (!f) f = fopen(ARCHIVO, "wb+");
    if (!f) return 0;

    nuevo.estado = 1;
    fseek(f, sizeof(Vendedor) * pos, SEEK_SET);
    fwrite(&nuevo, sizeof(Vendedor), 1, f);

    fclose(f);
    return 1;
}

// -------------------- BAJA ----------------------
int bajaEnDisco(Vendedor aBorrar) {
    int pos;
    int res = localizarEnDisco(aBorrar.dni, &pos);
    if (res != 1) return 0;

    FILE *f = fopen(ARCHIVO, "rb+");
    if (!f) return 0;

    Vendedor v;
    fseek(f, sizeof(Vendedor) * pos, SEEK_SET);
    fread(&v, sizeof(Vendedor), 1, f);

    if (memcmp(&v, &aBorrar, sizeof(Vendedor) - sizeof(int)) == 0) { // Compara sin el estado
        v.estado = 0; // libre
        fseek(f, sizeof(Vendedor) * pos, SEEK_SET);
        fwrite(&v, sizeof(Vendedor), 1, f);
        fclose(f);
        return 1;
    }
    fclose(f);
    return 0;
}

// -------------------- EVOCAR ----------------------
Vendedor evocarEnDisco(int dni) {
    Vendedor vacio = {0};
    int pos;
    if (localizarEnDisco(dni, &pos) != 1) return vacio;

    FILE *f = fopen(ARCHIVO, "rb");
    if (!f) return vacio;

    Vendedor v;
    fseek(f, sizeof(Vendedor) * pos, SEEK_SET);
    fread(&v, sizeof(Vendedor), 1, f);
    fclose(f);
    return v;
}

// Hashing provisto
int hashing(int dni){
    char x[8];
    int longitud, i;
    int contador = 0;
    sprintf(x, "%d", dni);
    longitud = strlen(x);
    for (i = 0; i < longitud; i++)
        contador += ((int)x[i]) * (i + 1);
    return (contador % M);
}