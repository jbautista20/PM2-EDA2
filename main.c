#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

// 210 / (6*39)  =  0,897...
// Desperdicio: 40 bytes (4096−39×104 = 4096−4056 = 40 bytes)

#define M 6  // Cantidad de baldes
#define R 39   // Ranuras por balde
#define ARCHIVO "Vendedores.txt"   //nombre del archivo
#define ARCHIVO_BIN "vendedores.dat"  //archivo binario

// Estructura para un vendedor
typedef struct {
    int dni;
    char nombre[51];
    char telefono[16];
    float valor;
    int cantVendida;
    char tipoVenta[21];
} VendedorSinEstado;

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

int hashing();
int altaEnDisco();
int bajaEnDisco();
Vendedor evocarEnDisco();
void convertirTxtABinario();
void consultaBaldeContenedor();
void mostrarEstructura();
void memorizarDesdeArchivo();
void mostrarVendedor();
void analisisCostos();

void menu() {
    int opcion, dni, pos;
    Vendedor vendedorAux = {0,"","",0,0,"",0};
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
                printf("\n-------ALTA DE VENDEDOR-------");
                printf("\nDni de vendedor: ");
                scanf("%d", &vendedorAux.dni);
                printf("\nNombre y apellido: ");
                scanf(" %[^\n]", vendedorAux.nombre);
                printf("\nTelefono: ");
                scanf("%d", &vendedorAux.telefono);
                printf("\nTipo de venta: ");
                scanf(" %[^\n]", vendedorAux.tipoVenta);
                printf("\nCantidad vendida: ");
                scanf("%d", &vendedorAux.cantVendida);
                printf("\nValor de venta: ");
                scanf("%f", &vendedorAux.valor);
                altaEnDisco(vendedorAux);
                break;
            case 2:
                printf("\n-------BAJA DE VENDEDOR-------\n");
                printf("Ingrese DNI del vendedor a eliminar: ");
                scanf("%d", &dni);

                vendedorAux = evocarEnDisco(dni);
                if (vendedorAux.estado != 1) {
                    printf("No se encontró un vendedor activo con ese DNI.\n");
                    break;
                }

                int confirmacion;
                printf("\nVENDEDOR ENCONTRADO CON EXITO\n");
                //mostrarVendedor(vendedorAux);
                printf("Si desea eliminarlo presione 1: ");
                scanf("%d", &confirmacion);
                if(confirmacion==1){
                    bajaEnDisco(vendedorAux);
                }
                break;
            case 3:
                printf("\n-------CONSULTAR VENDEDOR-------");
                printf("\nIngrese DNI de vendedor: ");
                scanf("%d", &dni);
                vendedorAux = evocarEnDisco(dni);
                if (vendedorAux.estado == 1) {
                    //mostrarVendedor(vendedorAux);
                } else {
                    printf("No se encontró un vendedor activo con DNI %d.\n", dni);
                }
                break;
            case 4:
                //consultaBaldeContenedor();
                break;
            case 5:
                mostrarEstructura();
                break;
            case 6:
                //memorizarDesdeArchivo();
                break;
            case 7:
                //analisisCostos();
                break;
            case 0:
                printf("Saliendo del programa...\n");
                break;
            default:
                printf("Opcion invalida. Intente nuevamente.\n");
        }
    } while (opcion != 0);
}

int main(){   
    convertirTxtABinario();
    menu();
    return 0;
}

void convertirTxtABinario() {
    FILE *ftxt = fopen(ARCHIVO, "r");
    FILE *fbin = fopen(ARCHIVO_BIN, "wb");

    if (!ftxt || !fbin) {
        printf("Error al abrir archivos.\n");
        if (ftxt) fclose(ftxt);
        if (fbin) fclose(fbin);
        return;
    }

    VendedorSinEstado v;
    char buffer[128];
    int contador = 0;

    while (fscanf(ftxt, "%d\n", &v.dni) == 1) {
        fgets(v.nombre, sizeof(v.nombre), ftxt);
        fgets(v.telefono, sizeof(v.telefono), ftxt);
        fscanf(ftxt, "%f\n", &v.valor);
        fscanf(ftxt, "%d\n", &v.cantVendida);
        fgets(v.tipoVenta, sizeof(v.tipoVenta), ftxt);

        // Limpiar \n
        v.nombre[strcspn(v.nombre, "\n")] = 0;
        v.telefono[strcspn(v.telefono, "\n")] = 0;
        v.tipoVenta[strcspn(v.tipoVenta, "\n")] = 0;

        fwrite(&v, sizeof(VendedorSinEstado), 1, fbin);
        contador++;
    }

    fclose(ftxt);
    fclose(fbin);

    printf("Conversión completa: %d vendedores exportados a binario.\n", contador);
}

void mostrarVendedor(VendedorSinEstado v) {
    printf("  DNI: %d\n", v.dni);
    printf("  Nombre: %s\n", v.nombre);
    printf("  Telefono: %s\n", v.telefono);
    printf("  Valor: %.2f\n", v.valor);
    printf("  Cantidad Vendida: %d\n", v.cantVendida);
    printf("  Tipo Venta: %s\n", v.tipoVenta);
}

void mostrarEstructura() {
    FILE *f = fopen(ARCHIVO_BIN, "rb");
    rewind(f);
    if (!f) {
        printf("No se pudo abrir el archivo binario.\n");
        return;
    }

    VendedorSinEstado v;
    for (int i = 0; i < M; i++) {
        printf("\033[34mBalde %d\033[0m\n", i);
        for (int j = 0; j < R; j++) {
            int pos = i * R + j;
            fseek(f, pos * sizeof(VendedorSinEstado), SEEK_SET);
            if (fread(&v, sizeof(VendedorSinEstado), 1, f) == 1) {
                printf("\033[31mRanura %d:\033[0m\n", j);
                /*switch (v.estado) {
                    case -1: printf("VIRGEN\n"); break;
                    case 0: printf("LIBRE\n"); break;
                    case 1:
                        printf("OCUPADA\n");*/
                        mostrarVendedor(v);
                        /*break;
                    default: printf("ESTADO DESCONOCIDO\n");
                }*/
            } else {
                printf("Ranura %d - ERROR DE LECTURA\n", j);
            }
        }
        printf("--------------------------------------------------\n");
    }

    fclose(f);
}

// -------------------- LOCALIZAR ----------------------
// Retorna: 1 si existe, 0 si no existe, -1 si lleno. Pos contiene posición lógica (balde y ranura).
Posicion localizarEnDisco(int dni, int *exito) {
    FILE *f = fopen(ARCHIVO_BIN, "rb");
    rewind(f);
    Posicion pos = {-1, -1};

    printf("Entrando a localizar");

    if (!f) {
        *exito = -1;
        return pos;
    }

    printf("\nAntes de hhashing\n");

    int h = hashing(dni);
    int i = 0, MAux = -1;
    Vendedor v;

    while (i < M * R) {
        fseek(f, sizeof(Vendedor) * h, SEEK_SET);
        fread(&v, sizeof(Vendedor), 1, f);

        if (v.estado == -1) break;  // celda virgen
        if (v.estado == 0 && MAux == -1) MAux = h;
        if (v.estado == 1 && v.dni == dni) {
            pos.posicionBalde = h / R; 
            pos.posicionRanura = h % R;
            fclose(f);
            *exito = 1;
            return pos;  // Encontrado
        }

        h = (h + 1) % (M * R);
        i++;
    }

    if (i >= M * R && MAux == -1) {
        fclose(f);
        *exito = -1;  // lleno
        return pos;
    }

    int libre = (MAux != -1) ? MAux : h;
    pos.posicionBalde = libre / R;
    pos.posicionRanura = libre % R;
    *exito = 0;
    fclose(f);
    return pos;
}


// -------------------- ALTA ----------------------
int altaEnDisco(Vendedor nuevo) {
    int exito;
    Posicion pos = localizarEnDisco(nuevo.dni, &exito);
    if (exito != 0) return 0;  // ya existe o disco lleno

    FILE *f = fopen(ARCHIVO_BIN, "rb+");
    if (!f) f = fopen(ARCHIVO_BIN, "wb+");
    if (!f) return 0;

    nuevo.estado = 1;
    int posicionAbsoluta = pos.posicionBalde * R + pos.posicionRanura;

    fseek(f, sizeof(Vendedor) * posicionAbsoluta, SEEK_SET);
    fwrite(&nuevo, sizeof(Vendedor), 1, f);
    fclose(f);
    return 1;
}


// -------------------- BAJA ----------------------
int bajaEnDisco(Vendedor aBorrar) {
    int exito;
    Posicion pos = localizarEnDisco(aBorrar.dni, &exito);
    if (exito != 1) return 0;  // no encontrado

    FILE *f = fopen(ARCHIVO_BIN, "rb+");
    if (!f) return 0;

    int posicionAbsoluta = pos.posicionBalde * R + pos.posicionRanura;
    Vendedor v;
    fseek(f, sizeof(Vendedor) * posicionAbsoluta, SEEK_SET);
    fread(&v, sizeof(Vendedor), 1, f);

    // Comparación manual sin tener en cuenta campo 'estado'
    if (v.dni == aBorrar.dni &&
        strcmp(v.nombre, aBorrar.nombre) == 0 &&
        strcmp(v.telefono, aBorrar.telefono) == 0 &&
        v.valor == aBorrar.valor &&
        v.cantVendida == aBorrar.cantVendida &&
        strcmp(v.tipoVenta, aBorrar.tipoVenta) == 0)
    {
        v.estado = 0;
        fseek(f, sizeof(Vendedor) * posicionAbsoluta, SEEK_SET);
        fwrite(&v, sizeof(Vendedor), 1, f);
        fclose(f);
        return 1;
    }

    fclose(f);
    return 0;
}

// ---EVOCAR---
Vendedor evocarEnDisco(int dni) {
    Vendedor vacio = {0};
    int exito;
    Posicion pos = localizarEnDisco(dni, &exito);
    if (exito != 1) return vacio;

    FILE *f = fopen(ARCHIVO_BIN, "rb");
    if (!f) return vacio;

    int posicionAbsoluta = pos.posicionBalde * R + pos.posicionRanura;
    Vendedor v;
    fseek(f, sizeof(Vendedor) * posicionAbsoluta, SEEK_SET);
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