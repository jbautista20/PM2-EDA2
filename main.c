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
Vendedor evocarEnDisco();
void convertirTxtABinario();
void consultaBaldeContenedor();
void mostrarEstructura();
void memorizarDesdeArchivo();
void mostrarVendedor();
void analisisCostos();
void crearArchivoInicial();

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

                vendedorAux = evocarEnDisco(dni);
                if (vendedorAux.dni < 1) {
                    printf("No se encontro el vendedor.\n");
                    break;
                }

                int confirmacion;
                printf("\nVENDEDOR ENCONTRADO CON EXITO\n");
                mostrarVendedor(vendedorAux);
                printf("Si desea eliminarlo presione 1: ");
                scanf("%d", &confirmacion);
                if(confirmacion==1){
                    bajaEnDisco(vendedorAux.dni);
                }
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
    crearArchivoInicial(0);
    menu();
    return 0;
}

// forzar sera 1 al llamar operaciones (para limpiar todo el RAL) y 0 al iniciar el main
void crearArchivoInicial(int forzar) {
    FILE *f = fopen(ARCHIVO, "r");
    if (f && forzar == 0) {
        fclose(f);
        return; // Ya existe, y no se sobreescribe
    }

    f = fopen(ARCHIVO, "w");
    if (!f) {
        printf("Error al crear el archivo.\n");
        return;
    }

    Vendedor vacio = {-1, "", "", 0.0, 0, ""};

    for (int i = 0; i < M * R; i++) {
        fprintf(f, "%d\n\n\n%.2f\n%d\n\n", 
                vacio.dni, vacio.valor, vacio.cantVendida);
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

void limpiarSaltoDeLinea(char *str) {
    str[strcspn(str, "\n")] = '\0';  // Reemplaza '\n' por '\0'
}

void mostrarEstructura() {
    FILE *f = fopen(ARCHIVO, "r");
    if (!f) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
    rewind(f);

    char buffer[100];
    Vendedor vendedor;
    int ranura = 0;

    printf("\n========== ESTRUCTURA ==========\n");

    for (int b = 0; b < M; b++) {
        printf("\033[33mBalde %d:\033[0m\n", b);
        for (int r = 0; r < R; r++) {
            if (fgets(buffer, sizeof(buffer), f) == NULL) break;
            limpiarSaltoDeLinea(buffer);
            vendedor.dni = atoi(buffer);

            fgets(buffer, sizeof(buffer), f); limpiarSaltoDeLinea(buffer); strcpy(vendedor.nombre, buffer);
            fgets(buffer, sizeof(buffer), f); limpiarSaltoDeLinea(buffer); strcpy(vendedor.telefono, buffer);
            fgets(buffer, sizeof(buffer), f); limpiarSaltoDeLinea(buffer); vendedor.valor = atof(buffer);
            fgets(buffer, sizeof(buffer), f); limpiarSaltoDeLinea(buffer); vendedor.cantVendida = atoi(buffer);
            fgets(buffer, sizeof(buffer), f); limpiarSaltoDeLinea(buffer); strcpy(vendedor.tipoVenta, buffer);

            printf("\033[32mRanura %d:\033[0m", r);

            if (vendedor.dni == -1 && strlen(vendedor.nombre) == 0 && strlen(vendedor.telefono) == 0 && 
                strlen(vendedor.tipoVenta) == 0 && vendedor.valor == 0 && vendedor.cantVendida == 0) {
                printf("  Estado: VIRGEN\n");
            } else if (vendedor.dni == 0) {
                printf("  Estado: LIBRE\n");
            } else {
                printf("  Estado: OCUPADA\n");
                mostrarVendedor(vendedor);
            }
            printf("\n");
        }
        printf("--------------------------------------------------\n");
    }

    fclose(f);
}

// -------------------- LOCALIZAR ----------------------
// Retorna: 1 si existe, 0 si no existe, -1 si lleno. Pos contiene posición (balde y ranura).
Posicion localizarEnDisco(int dni, int* exito, Balde* baldeOut) {
    FILE* f = fopen(ARCHIVO, "r");
    Balde balde;
    Posicion ret = {-1,-1};
    if (!f) {
        *exito = -1;
        return ret;
    }

    int h = hashing(dni);  // balde inicial
    int intentos = 0;
    int MAuxBalde = -1, MAuxRanura = -1;

    while (intentos < M) {
        int baldeActual = (h + intentos) % M;        
        int lineaInicio = baldeActual * R * 6;
        int lineaActual = 0;
        char buffer[200];
        Vendedor aux;

        rewind(f);
        // Saltar hasta inicio del balde
        while (lineaActual < lineaInicio && fgets(buffer, sizeof(buffer), f)) {
            lineaActual++;
        }

        // Leer balde completo
        for (int r = 0; r < R; r++) {
            fgets(buffer, sizeof(buffer), f); sscanf(buffer, "%d", &aux.dni);
            fgets(aux.nombre, sizeof(aux.nombre), f);
            fgets(aux.telefono, sizeof(aux.telefono), f);
            fgets(buffer, sizeof(buffer), f); sscanf(buffer, "%f", &aux.valor);
            fgets(buffer, sizeof(buffer), f); sscanf(buffer, "%d", &aux.cantVendida);
            fgets(aux.tipoVenta, sizeof(aux.tipoVenta), f);

            aux.nombre[strcspn(aux.nombre, "\n")] = 0;
            aux.telefono[strcspn(aux.telefono, "\n")] = 0;
            aux.tipoVenta[strcspn(aux.tipoVenta, "\n")] = 0;

            balde.ranuras[r] = aux;

            printf("dni aux: %d\n",aux.dni);

            if (aux.dni == dni) {
                *exito = 1;
                *baldeOut = balde;
                ret.posicionBalde = baldeActual;
                ret.posicionRanura = r;
                fclose(f);
                return ret;
            }
            else if (aux.dni == -1) {   // celda virgen, detener localizar
            // completar el resto del balde con datos vacíos
            for (int k = r; k < R; k++) {
                Vendedor vacio = {-1, "", "", 0.0, 0, ""};
                balde.ranuras[k] = vacio;
            }
            *exito = 0;
            *baldeOut = balde;
            ret.posicionBalde = baldeActual;
            ret.posicionRanura = r;
            fclose(f);
            return ret;
            }
            else if(aux.dni == 0 && MAuxBalde == -1) {    // ranura LIBRE                
                    MAuxBalde = baldeActual;
                    MAuxRanura = r;
                    *baldeOut = balde;
                    // Nota: no retornamos de inmediato, seguimos viendo si ya existe
                }
            }

        // Si no se encontró en este balde, continuar al siguiente
        intentos++;
    }

    fclose(f);

    if (MAuxBalde != -1) {  // se busco por toda la estructura y no esta, se retorna la primer LIBRE (si existe)
        *exito = 0;
        ret.posicionBalde = MAuxBalde;
        ret.posicionRanura = MAuxRanura;
        return ret;
    }

    *exito = -1;    //no esta y no se puede insertar porque ya esta lleno el RAL.
    return ret;
}


/* alta utiliza archivo temporal (CONSULTAR!!!)
sin eso no logramos hacerlo andar debido a que:
No se puede hacer un fseek() hasta la línea 234 fácilmente, porque las líneas tienen longitudes variables (telefono, nombre).
Si escribimos encima y lo nuevo ocupa más o menos espacio que lo anterior, se corrompe todo el contenido posterior.

-Se abre el archivo original RAL en modo lectura.
-Se crea un archivo temporal (temp.txt) en modo escritura.
Se copia línea por línea del original al temporal:
-Cuando se llega a la posición que debe modificarse (por ejemplo, un alta o baja), se escribe la nueva versión del dato.
-En las demás posiciones, se copia tal cual lo leído del archivo original.
Una vez copiado todo el archivo:
-Se cierra ambos archivos.
-Se borra el archivo original.
-Se renombra el archivo temporal como archivo principal (rename("temp.txt", RAL)).

*/
int altaEnDisco(Vendedor nuevo) {
    Balde balde;
    int resultado;
    Posicion pos = localizarEnDisco(nuevo.dni, &resultado, &balde);

    if (resultado != 0) return 0;  // ya existe o estructura llena

    balde.ranuras[pos.posicionRanura] = nuevo;

    FILE* fOriginal = fopen(ARCHIVO, "r");
    FILE* fTemp = fopen("temp.txt", "w");
    if (!fOriginal || !fTemp) return 0;

    rewind(fOriginal);
    char buffer[200];

    for (int b = 0; b < M; b++) {
        for (int r = 0; r < R; r++) {
            Vendedor v;

            if (b == pos.posicionBalde && r == pos.posicionRanura) {
                // Modifico la ranura
                v = nuevo;
            } else {
                // Copio el archivo original
                fgets(buffer, sizeof(buffer), fOriginal); sscanf(buffer, "%d", &v.dni);
                fgets(v.nombre, sizeof(v.nombre), fOriginal);
                fgets(v.telefono, sizeof(v.telefono), fOriginal);
                fgets(buffer, sizeof(buffer), fOriginal); sscanf(buffer, "%f", &v.valor);
                fgets(buffer, sizeof(buffer), fOriginal); sscanf(buffer, "%d", &v.cantVendida);
                fgets(v.tipoVenta, sizeof(v.tipoVenta), fOriginal);

                // limpieza de \n
                v.nombre[strcspn(v.nombre, "\n")] = 0;
                v.telefono[strcspn(v.telefono, "\n")] = 0;
                v.tipoVenta[strcspn(v.tipoVenta, "\n")] = 0;
            }

            // Escribo al archivo temporal
            fprintf(fTemp, "%d\n%s\n%s\n%.2f\n%d\n%s\n",
                v.dni,
                v.nombre,
                v.telefono,
                v.valor,
                v.cantVendida,
                v.tipoVenta);
        }
    }

    fclose(fOriginal);
    fclose(fTemp);
    remove(ARCHIVO);
    rename("temp.txt", ARCHIVO);

    return 1;
}

// -------------------- BAJA ----------------------
int bajaEnDisco(int dni) {
    Balde balde;
    int resultado;
    Posicion pos = localizarEnDisco(dni, &resultado, &balde);

    if (resultado != 1) return 0; // No encontrado

    // Reemplazar con ranura vacía
    Vendedor vacio = {0, "", "", 0.0, 0, ""};
    balde.ranuras[pos.posicionRanura] = vacio;

    FILE* fOriginal = fopen(ARCHIVO, "r");
    FILE* fTemp = fopen("temp.txt", "w");
    if (!fOriginal || !fTemp) return 0;

    char buffer[200];

    for (int b = 0; b < M; b++) {
        for (int r = 0; r < R; r++) {
            Vendedor v;
            if (b == pos.posicionBalde) {
                v = balde.ranuras[r];  // del balde ya modificado
            } else {
                // Leer desde archivo original
                fgets(buffer, sizeof(buffer), fOriginal); sscanf(buffer, "%d", &v.dni);
                fgets(v.nombre, sizeof(v.nombre), fOriginal);
                fgets(v.telefono, sizeof(v.telefono), fOriginal);
                fgets(buffer, sizeof(buffer), fOriginal); sscanf(buffer, "%f", &v.valor);
                fgets(buffer, sizeof(buffer), fOriginal); sscanf(buffer, "%d", &v.cantVendida);
                fgets(v.tipoVenta, sizeof(v.tipoVenta), fOriginal);
                v.nombre[strcspn(v.nombre, "\n")] = 0;
                v.telefono[strcspn(v.telefono, "\n")] = 0;
                v.tipoVenta[strcspn(v.tipoVenta, "\n")] = 0;
            }

            // Escribir al archivo temporal
            fprintf(fTemp, "%d\n%s\n%s\n%.2f\n%d\n%s\n",
                v.dni,
                v.nombre,
                v.telefono,
                v.valor,
                v.cantVendida,
                v.tipoVenta);
        }
    }

    fclose(fOriginal);
    fclose(fTemp);
    remove(ARCHIVO);
    rename("temp.txt", ARCHIVO);
    return 1;
}

// ---EVOCAR---
Vendedor evocarEnDisco(int dniBuscado) {
    Vendedor v = {-1, "", "", 0.0, 0, ""};  // inicializado para retornar si no se encuentra
    FILE *f = fopen(ARCHIVO, "r");
    if (!f) {
        printf("No se pudo abrir el archivo.\n");
        return v;
    }
    rewind(f);

    int pos = hashing(dniBuscado);
    int intentos = 0;
    int dni;
    char nombre[51], telefono[16], tipoVenta[21];
    float valor;
    int cantVendida;

    // Empieza desde la posición hasheada y recorre en rebalse abierto lineal
    while (intentos < M * R) {
        int balde = pos / R;
        int ranura = pos % R;

        // saltar a la posición correspondiente
        rewind(f);
        for (int i = 0; i < pos; i++) {
            // descartar las 6 líneas del vendedor anterior
            fscanf(f, "%*[^\n]\n%*[^\n]\n%*[^\n]\n%*[^\n]\n%*[^\n]\n%*[^\n]\n");
        }

        // leer datos del vendedor actual
        if (fscanf(f, "%d\n", &dni) != 1) break;
        fgets(nombre, sizeof(nombre), f); nombre[strcspn(nombre, "\n")] = '\0';
        fgets(telefono, sizeof(telefono), f); telefono[strcspn(telefono, "\n")] = '\0';
        fscanf(f, "%f\n", &valor);
        fscanf(f, "%d\n", &cantVendida);
        fgets(tipoVenta, sizeof(tipoVenta), f); tipoVenta[strcspn(tipoVenta, "\n")] = '\0';

        if (dni == dniBuscado) {
            // encontrado
            strcpy(v.nombre, nombre);
            strcpy(v.telefono, telefono);
            strcpy(v.tipoVenta, tipoVenta);
            v.dni = dni;
            v.valor = valor;
            v.cantVendida = cantVendida;
            fclose(f);
            return v;
        }

        pos = (pos + 1) % (M * R);
        intentos++;
    }

    fclose(f);
    return v;  // no encontrado
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
    printf("Hashing: %d\n",contador % M);
    return (contador % M);
}