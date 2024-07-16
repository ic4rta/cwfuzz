#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <curl/curl.h>
#include <unistd.h>
#include "include/cwfuzz.h"

int main(int argc, char* argv[]) {
    Cwfuzz cwfuzz_struct = {0};
    pthread_t hilos[MAX_HILOS];
    Hilo datos_hilos[MAX_HILOS];
    int opcion;

    while ((opcion = getopt(argc, argv, "u:w:x:t:o:a:e:")) != -1) {
        switch (opcion) {
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
                if (cwfuzz_struct.cantidad_hilos > MAX_HILOS) {
                    fprintf(stderr, "Error: The number of threads cannot exceed %d\n", MAX_HILOS);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'o':
                cwfuzz_struct.archivo_output = optarg;
                break;
            case 'a':
                cwfuzz_struct.user_agent = optarg;
                break;
            case 'e':
                {
                    char* token = strtok(optarg, ",");
                    while (token != NULL) {
                        cwfuzz_struct.extensiones = realloc(cwfuzz_struct.extensiones, (cwfuzz_struct.cantidad_extensiones + 1) * sizeof(char*));
                        cwfuzz_struct.extensiones[cwfuzz_struct.cantidad_extensiones] = strdup(token);
                        cwfuzz_struct.cantidad_extensiones++;
                        token = strtok(NULL, ",");
                    }
                }
                break;
            default:
                fprintf(stderr, "Use: %s -u <URL> -w <wordlist> -x <status code...> -t <threads> [-o output file] [-a user agent] [-e extensions]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (!cwfuzz_struct.url || !cwfuzz_struct.worlist || !cwfuzz_struct.codigos_estado || cwfuzz_struct.cantidad_hilos == 0) {
        fprintf(stderr, "Use: %s -u <URL> -w <wordlist> -x <status code...> -t <threads> [-o output file] [-a user agent] [-e extensions]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

	banner(&cwfuzz_struct);
    guardar_resultados(&cwfuzz_struct);
    init_hilo_curl(&cwfuzz_struct);
    crear_hilos(&cwfuzz_struct, hilos, datos_hilos);
    liberar_recursos(&cwfuzz_struct, hilos);
    return 0;
}
