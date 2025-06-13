#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

// 210 / (6*39)  =  0,897...
// Desperdicio: 40 bytes (4096−39×104 = 4096−4056 = 40 bytes)

#define M 6  // Cantidad de baldes
#define R 39   // Ranuras por balde
#define ARCHIVO "RAL.txt"   //nombre del archivo contenedor del RAL
#define A_VENDEDORES "Vendedores.txt"  //archivo con vendedores
#define A_OPERACIONES "Operaciones.txt" //archivo con operaciones

// Estructura para costos
typedef struct 
{
    int cantidad;
    int maximo;
    int temp;
    int sumatoria;
}Costos;

// Estructura para un vendedor
typedef struct {
    int dni;
    char nombre[51];
    char telefono[16];
    float valor;
    int cantVendida;
    char tipoVenta[21];
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
int bajaEnDiscoManual();
Vendedor evocarEnDisco();
void convertirTxtABinario();
void consultaBaldeContenedor();
void mostrarEstructura();
void memorizarDesdeArchivo();
void mostrarVendedor();
void analisisCostos();
void crearArchivoInicial();
int Memorizacion_Previa();
void cuadroComp();
int compararVendedor();

Costos baldeAlta = {0,0,0,0};
Costos baldeBaja = {0,0,0,0};
Costos baldeEvoc = {0,0,0,0};
Costos baldeEvocNE = {0,0,0,0};
Costos ranuraAlta = {0,0,0,0};
Costos ranuraBaja = {0,0,0,0};
Costos ranuraEvoc = {0,0,0,0};
Costos ranuraEvocNE = {0,0,0,0};
int costoBaldeAux = 0;
int costoRanuraAux = 0;

void menu() {
    int opcion, dni, pos;
    Vendedor vendedorAux = {0,"","",0,0,""};
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
                vendedorAux.dni = dni;
                bajaEnDisco(vendedorAux,1);
                break;
            case 3:
                printf("\n-------CONSULTAR VENDEDOR-------");
                printf("\nIngrese DNI de vendedor: ");
                scanf("%d", &dni);
                vendedorAux = evocarEnDisco(dni);
                if (vendedorAux.dni > 0) {
                    mostrarVendedor(vendedorAux);
                } else {
                    printf("No se encontro un vendedor activo con DNI %d.\n", dni);
                }
                break;
            case 4:
                consultaBaldeContenedor();
                break;
            case 5:
                mostrarEstructura();
                break;
            case 6:
                Memorizacion_Previa();
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

int main(){   
    crearArchivoInicial(0);
    menu();
    return 0;
}

// forzar sera 1 al llamar operaciones (para limpiar todo el RAL) y 0 al iniciar el main
void crearArchivoInicial(int forzar) {
    FILE *f = fopen(ARCHIVO, "rb");
    if (f && forzar == 0) {
        fclose(f);
        return; // Ya existe, y no se debe sobreescribir
    }
    if (f) fclose(f);  // Si existe y se fuerza, cerrarlo

    f = fopen(ARCHIVO, "wb");
    if (!f) {
        printf("Error al crear el archivo.\n");
        return;
    }

    Balde baldeVacio;
    Vendedor vacio = {-1, "", "", 0.0, 0, ""};
    for (int i = 0; i < R; i++) {
        baldeVacio.ranuras[i] = vacio;
    }

    // escribir los M baldes
    for (int i = 0; i < M; i++) {
        fwrite(&baldeVacio, sizeof(Balde), 1, f);

        // 40 bytes como relleno (DESPERDICIO)
        char relleno[40] = {0};
        fwrite(relleno, sizeof(char), 40, f);
    }

    fclose(f);
    printf("\n\033[34mRAL en disco creado correctamente.\033[0m\n");
}

void mostrarVendedor(Vendedor v) {
    printf("  DNI: %d\n", v.dni);
    printf("  Nombre: %s\n", v.nombre);
    printf("  Telefono: %s\n", v.telefono);
    printf("  Valor: %.2f\n", v.valor);
    printf("  Cantidad Vendida: %d\n", v.cantVendida);
    printf("  Tipo Venta: %s\n", v.tipoVenta);
}

void mostrarEstructura() {
    FILE *f = fopen(ARCHIVO, "rb");
    if (!f) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }

    Balde balde;

    printf("\n========== ESTRUCTURA ==========\n");

    for (int b = 0; b < M; b++) {
        fseek(f, b * (sizeof(Balde) + 40), SEEK_SET);  // saltar al balde actual

        if (fread(&balde, sizeof(Balde), 1, f) != 1) break;

        printf("\033[33mBalde %d:\033[0m\n", b);
        for (int r = 0; r < R; r++) {
            Vendedor v = balde.ranuras[r];

            printf("\033[32mRanura %d:\033[0m", r);
            if (v.dni == -1 && strlen(v.nombre) == 0 && strlen(v.telefono) == 0 &&
                strlen(v.tipoVenta) == 0 && v.valor == 0 && v.cantVendida == 0) {
                printf("  Estado: VIRGEN\n");
            } else if (v.dni == 0) {
                printf("  Estado: LIBRE\n");
            } else {
                printf("  Estado: OCUPADA\n");
                mostrarVendedor(v);
            }
            printf("\n");
        }
        printf("--------------------------------------------------\n");
    }

    fclose(f);
}

// --LOCALIZAR--
// Retorna: 1 si existe, 0 si no existe, -1 si está lleno. Devuelve también el balde leído.
Posicion localizarEnDisco(int dni, int* exito, Balde* baldeOut) {
    FILE* f = fopen(ARCHIVO, "rb");
    if (!f) {
        *exito = -1;
        return (Posicion){-1, -1};
    }

    baldeAlta.temp = baldeBaja.temp = baldeEvoc.temp = baldeEvocNE.temp = 0;
    ranuraAlta.temp = ranuraBaja.temp = ranuraEvoc.temp = ranuraEvocNE.temp = 0;
    costoBaldeAux = costoRanuraAux = 0;

    int h = hashing(dni);
    int intentos = 0;
    int MAuxBalde = -1, MAuxRanura = -1;
    int hayBaldeLibre = 0;
    Balde baldeLibre;
    Posicion ret = {-1, -1};

    while (intentos < M) {
        int baldeActual = (h + intentos) % M;
        costoBaldeAux++;

        fseek(f, baldeActual * (sizeof(Balde) + 40), SEEK_SET);
        Balde balde;
        fread(&balde, sizeof(Balde), 1, f);

        for (int r = 0; r < R; r++) {
            Vendedor aux = balde.ranuras[r];
            costoRanuraAux++;

            if (aux.dni == dni) {
                *exito = 1;
                *baldeOut = balde;
                ret.posicionBalde = baldeActual;
                ret.posicionRanura = r;
                fclose(f);
                return ret;
            } else if (aux.dni == -1) {
                fclose(f);
                if (hayBaldeLibre) {
                    *exito = 0;
                    *baldeOut = baldeLibre;
                    ret.posicionBalde = MAuxBalde;
                    ret.posicionRanura = MAuxRanura;
                    return ret;
                } else {
                    *exito = 0;
                    *baldeOut = balde;
                    ret.posicionBalde = baldeActual;
                    ret.posicionRanura = r;
                    return ret;
                }
            } else if (aux.dni == 0 && !hayBaldeLibre) {
                MAuxBalde = baldeActual;
                MAuxRanura = r;
                baldeLibre = balde;
                hayBaldeLibre = 1;
            }
        }

        intentos++;
    }

    fclose(f);

    if (hayBaldeLibre) {
        *exito = 0;
        *baldeOut = baldeLibre;
        ret.posicionBalde = MAuxBalde;
        ret.posicionRanura = MAuxRanura;
        return ret;
    }

    *exito = -1;
    return ret;
}


// -- ALTA --
int altaEnDisco(Vendedor nuevo) {
    Balde balde;
    int resultado;
    Posicion pos = localizarEnDisco(nuevo.dni, &resultado, &balde);

    baldeAlta.temp = costoBaldeAux;
    if (baldeAlta.temp < baldeAlta.maximo){
        baldeAlta.maximo = baldeAlta.temp;
    }
    baldeAlta.sumatoria += baldeAlta.temp;
    baldeAlta.cantidad++;

    ranuraAlta.temp = costoRanuraAux;
    if (ranuraAlta.temp < ranuraAlta.maximo){
        ranuraAlta.maximo = ranuraAlta.temp;
    }
    ranuraAlta.sumatoria += ranuraAlta.temp;
    ranuraAlta.cantidad++;

    if (resultado != 0) return 0;  // ya existe o estructura llena

    balde.ranuras[pos.posicionRanura] = nuevo;

    FILE* f = fopen(ARCHIVO, "rb+");
    if (!f) return 0;

    fseek(f, (sizeof(Balde) + 40) * pos.posicionBalde, SEEK_SET);
    fwrite(&balde, sizeof(Balde), 1, f);

    fclose(f);
    return 1;
}

int compararVendedor(Vendedor v, Vendedor v2){
    if ( (strcasecmp(v.nombre, v2.nombre) == 0) && v.dni == v2.dni && (strcasecmp(v.tipoVenta, v2.tipoVenta) == 0) && (strcasecmp(v.telefono, v2.telefono) == 0)
&& v.cantVendida == v2.cantVendida && v.valor == v2.valor)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int bajaEnDisco(Vendedor v2, int bajaManual) {
    Balde balde;
    int resultado;
    Posicion pos = localizarEnDisco(v2.dni, &resultado, &balde);

    // Costos (si aplica)
    baldeBaja.temp = costoBaldeAux;
    if (baldeBaja.temp > baldeBaja.maximo) baldeBaja.maximo = baldeBaja.temp;
    baldeBaja.sumatoria += baldeBaja.temp;
    baldeBaja.cantidad++;

    ranuraBaja.temp = costoRanuraAux;
    if (ranuraBaja.temp > ranuraBaja.maximo) ranuraBaja.maximo = ranuraBaja.temp;
    ranuraBaja.sumatoria += ranuraBaja.temp;
    ranuraBaja.cantidad++;

    if (resultado != 1) {
        if (bajaManual){ printf("No se encontro el vendedor.\n");}
        return 0;
    }

    Vendedor v = balde.ranuras[pos.posicionRanura];
    int confirmacion = 1;

    if (bajaManual) {
        printf("\nVENDEDOR ENCONTRADO CON EXITO\n");
        mostrarVendedor(v);
        printf("Si desea eliminarlo presione 1: ");
        scanf("%d", &confirmacion);
    } else {
        confirmacion = compararVendedor(v, v2);
    }

    if (confirmacion != 1) {return 0;}

    Vendedor vacio = {0, "", "", 0.0, 0, ""};
    balde.ranuras[pos.posicionRanura] = vacio;

    FILE *f = fopen(ARCHIVO, "rb+");
    if (!f) return 0;

    fseek(f, (sizeof(Balde) + 40) * pos.posicionBalde, SEEK_SET);
    fwrite(&balde, sizeof(Balde), 1, f);
    fclose(f);

    if (bajaManual) { printf("\nVendedor eliminado correctamente.\n");}
    return 1;
}


void consultaBaldeContenedor() {
    int dni;
    printf("Ingrese el DNI del vendedor a consultar: ");
    scanf("%d", &dni);

    Balde balde;
    int exito;
    Posicion pos = localizarEnDisco(dni, &exito, &balde);

    if (exito != 1) {
        printf("\n\033[31mEl vendedor no fue encontrado en la estructura.\033[0m\n");
        return;
    }

    printf("\n\033[34mEl vendedor se encuentra en el balde #%d\033[0m\n", pos.posicionBalde);
    printf("\033[33mContenido del balde:\033[0m\n");

    for (int r = 0; r < R; r++) {
        Vendedor v = balde.ranuras[r];
        printf("\033[32mRanura %d:\033[0m", r);

        if (v.dni == -1 &&
            strlen(v.nombre) == 0 && strlen(v.telefono) == 0 && 
            v.valor == 0.0 && v.cantVendida == 0 && strlen(v.tipoVenta) == 0) {
            printf("  Estado: VIRGEN\n");
        }
        else if (v.dni == 0) {
            printf("  Estado: LIBRE\n");
        }
        else {
            printf("  Estado: OCUPADA\n");
            printf("  DNI: %d\n  Nombre: %s\n  Telefono: %s\n", v.dni, v.nombre, v.telefono);
            printf("  Valor: %.2f\n  Cantidad Vendida: %d\n  Tipo Venta: %s\n", 
                    v.valor, v.cantVendida, v.tipoVenta);
        }
        printf("\n");
    }
}

// ---EVOCAR---
Vendedor evocarEnDisco(int dniBuscado) {
    Vendedor vacio = {-1, "", "", 0.0, 0, ""};
    Balde balde;
    int exito;
    Posicion pos = localizarEnDisco(dniBuscado, &exito, &balde);    

    if (exito == 1) {

        baldeEvoc.temp = costoBaldeAux;
        if (baldeEvoc.temp < baldeEvoc.maximo){
            baldeEvoc.maximo = baldeEvoc.temp;
        }
        baldeEvoc.sumatoria += baldeEvoc.temp;
        baldeEvoc.cantidad++;

        ranuraEvoc.temp = costoRanuraAux;
        if (ranuraEvoc.temp < ranuraEvoc.maximo){
            ranuraEvoc.maximo = ranuraEvoc.temp;
        }
        ranuraEvoc.sumatoria += ranuraEvoc.temp;
        ranuraEvoc.cantidad++;

        return balde.ranuras[pos.posicionRanura];
    } else {

        baldeEvocNE.temp = costoBaldeAux;
        if (baldeEvocNE.temp < baldeEvocNE.maximo){
            baldeEvocNE.maximo = baldeEvocNE.temp;
        }
        baldeEvocNE.sumatoria += baldeEvocNE.temp;
        baldeEvocNE.cantidad++;

        ranuraEvocNE.temp = costoRanuraAux;
        if (ranuraEvocNE.temp < ranuraEvocNE.maximo){
            ranuraEvocNE.maximo = ranuraEvocNE.temp;
        }
        ranuraEvocNE.sumatoria += ranuraEvocNE.temp;
        ranuraEvocNE.cantidad++;

        return vacio; // no encontrado
    }
}

int Memorizacion_Previa() {
    FILE *fp = fopen("Vendedores.txt", "r");
    if (fp == NULL) {
        printf("No se pudo abrir Vendedores.txt\n");
        return 0;
    }

    Vendedor aux;
    while (!feof(fp)) {
        if (fscanf(fp, "%d\n", &aux.dni) != 1) break;
        fgets(aux.nombre, sizeof(aux.nombre), fp); aux.nombre[strcspn(aux.nombre, "\n")] = 0;
        fgets(aux.telefono, sizeof(aux.telefono), fp); aux.telefono[strcspn(aux.telefono, "\n")] = 0;
        fscanf(fp, "%f\n", &aux.valor);
        fscanf(fp, "%d\n", &aux.cantVendida);
        fgets(aux.tipoVenta, sizeof(aux.tipoVenta), fp); aux.tipoVenta[strcspn(aux.tipoVenta, "\n")] = 0;

        // Insertar en el archivo binario usando altaEnDisco
        if (!altaEnDisco(aux)) {
            printf("No se pudo insertar al vendedor con DNI %d (ya existente o estructura llena).\n", aux.dni);
        }
    }

    fclose(fp);
    return 1;
}

void cuadroComp()
{

    system("cls");
    system("color 03");
    printf("##============================================================ ##\n");
    printf("##    OPERACIONES      |BALDES CONSULTADOS|RANURAS CONSULTADAS ##\n");
    printf("##============================================================ ##\n");
    printf("|| MAX. ALTA           |     %.3f      |     %.3f     ##\n", (float)baldeAlta.maximo, (float)ranuraAlta.maximo);
    printf("|| MED. ALTA           |     %.3f      |     %.3f     ##\n",
           (baldeAlta.cantidad != 0) ? baldeAlta.sumatoria/baldeAlta.cantidad : 0.0,
           (ranuraAlta.cantidad != 0) ? ranuraAlta.sumatoria/ranuraAlta.cantidad : 0.0);
    printf("|| MAX. BAJA           |     %.3f      |     %.3f     ##\n", baldeBaja.maximo,ranuraBaja.maximo);
    printf("|| MED. BAJA           |     %.3f      |     %.3f     ##\n",
           (baldeBaja.cantidad != 0) ? baldeBaja.sumatoria/baldeBaja.cantidad : 0.0,
           (ranuraBaja.cantidad != 0) ? ranuraBaja.sumatoria / ranuraBaja.cantidad : 0.0);
    printf("|| MAX. EVOC EXITOSA   |     %.3f      |     %.3f     ##\n", baldeEvoc.maximo,ranuraEvoc.maximo);
    printf("|| MED. EVOC EXITOSA   |     %.3f      |     %.3f     ##\n",
           (baldeEvoc.cantidad != 0) ? baldeEvoc.sumatoria / baldeEvoc.cantidad : 0.0,
           (ranuraEvoc.cantidad != 0) ? ranuraEvoc.sumatoria / ranuraEvoc.cantidad : 0.0);
    printf("|| MAX. EVOC NO EXITOSA|     %.3f      |     %.3f     ##\n",baldeEvocNE.maximo, ranuraEvocNE.maximo);
    printf("|| MED. EVOC NO EXITOSA|     %.3f      |     %.3f     ##\n",
           (baldeEvocNE.cantidad != 0) ? baldeEvocNE.sumatoria / baldeEvocNE.cantidad : 0.0,
           (ranuraEvocNE.cantidad != 0) ? ranuraEvocNE.sumatoria / ranuraEvocNE.cantidad : 0.0);
    printf("##============================================================##\n");
    system("pause");
}

void analisisCostos() {
    crearArchivoInicial(1);
    
    Costos baldeAlta = {0,0,0,0};
    Costos baldeBaja = {0,0,0,0};
    Costos baldeEvoc = {0,0,0,0};
    Costos baldeEvocNE = {0,0,0,0};
    Costos ranuraAlta = {0,0,0,0};
    Costos ranuraBaja = {0,0,0,0};
    Costos ranuraEvoc = {0,0,0,0};
    Costos ranuraEvocNE = {0,0,0,0};

    FILE* fp = fopen(A_OPERACIONES, "r");
    if (!fp) {
        printf("No se pudo abrir el archivo operaciones.txt.\n");
        return;
    }

    int cod, dni, cant;
    float valor;
    char nombre[51], tel[16], tipoVenta[21];
    Vendedor vAux;

    while (!feof(fp)) {
        fscanf(fp, "%d", &cod);
        if (cod == 1 || cod == 2) {
            fscanf(fp, "%d\n", &vAux.dni);
            fgets(nombre, sizeof(nombre), fp); nombre[strcspn(nombre, "\n")] = 0;
            fgets(tel, sizeof(tel), fp);       tel[strcspn(tel, "\n")] = 0;
            fscanf(fp, "%f\n", &valor);
            fscanf(fp, "%d\n", &cant);
            fgets(tipoVenta, sizeof(tipoVenta), fp); tipoVenta[strcspn(tipoVenta, "\n")] = 0;

            strcpy(vAux.nombre, nombre);
            strcpy(vAux.telefono, tel);
            strcpy(vAux.tipoVenta, tipoVenta);
            vAux.valor = valor;
            vAux.cantVendida = cant;

            if (cod == 1){
                altaEnDisco(vAux);
            } else {
                bajaEnDisco(vAux,0);
            }
        } else if (cod == 3) {
            fscanf(fp, "%d\n", dni);
            evocarEnDisco(dni);
        } else {
            printf("Codigo de operacion no valido: %d\n", cod);
            break;
        }
    }
    fclose(fp);
    printf("Operaciones.txt leido con exito.\n");
    system("pause");
    cuadroComp();
}


// Hashing provisto
int hashing(int dni){
    char x[8];
    int longitud, i;
    int contador = 0;
    sprintf(x, "%d", dni);
    longitud = strlen(x);
    for (i = 0; i < longitud; i++){
        contador += ((int)x[i]) * (i + 1);
    }
    return (contador % M);
}