#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <curl/curl.h>
#include <unistd.h>
#include "include/cwfuzz.h"

int main(int argc, char* argv[]) {
    Cwfuzz cwfuzz_struct = {0};
    cwfuzz_struct.user_agent = NULL;
    int opt;
    pthread_t hilos[MAX_HILOS];
    Hilo datos_hilos[MAX_HILOS];
    curl_global_init(CURL_GLOBAL_ALL);

    while ((opt = getopt(argc, argv, "u:w:x:t:o:a:")) != -1) {
        switch (opt) {
            case 'u':
                cwfuzz_struct.url = optarg;
                break;
            case 'w':
                cwfuzz_struct.worlist = optarg;
                break;
            case 'x':
                {
                    char* token = strtok(optarg, ",");
                    while (token != NULL) {
                        cwfuzz_struct.codigos_estado = realloc(cwfuzz_struct.codigos_estado, (cwfuzz_struct.cantidad_codigos + 1) * sizeof(int));
                        cwfuzz_struct.codigos_estado[cwfuzz_struct.cantidad_codigos++] = atoi(token);
                        token = strtok(NULL, ",");
                    }
                }
                break;
            case 't':
                cwfuzz_struct.cantidad_hilos = atoi(optarg);
                break;
            case 'o':
                cwfuzz_struct.archivo_output = optarg;
                break;
	        case 'a':
	            cwfuzz_struct.user_agent = optarg;
	            break;
	        default:
	            fprintf(stderr, "Uso: %s -u <URL> -w <archivo> -x <códigos de estado> -t <hilos> [-o <archivo de resultados>] [-a <User-Agent personalizado>]\n", argv[0]);
	            exit(EXIT_FAILURE);
        }
    }

    if (!cwfuzz_struct.url || !cwfuzz_struct.worlist || !cwfuzz_struct.codigos_estado || cwfuzz_struct.cantidad_hilos <= 0) {
        fprintf(stderr, "Faltan argumentos obligatorios\n");
        fprintf(stderr, "Uso: %s -u <URL> -w <archivo> -x <códigos de estado> -t <hilos> [-o <archivo de resultados>] [-a <User-Agent personalizado>]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    banner(&cwfuzz_struct);
    curl_global_init(CURL_GLOBAL_ALL);
    guardar_resultados(&cwfuzz_struct);
    init_hilo_curl(&cwfuzz_struct);
    crear_hilos(&cwfuzz_struct, hilos, datos_hilos);
    liberar_recursos(&cwfuzz_struct, hilos);

    return EXIT_SUCCESS;
}
