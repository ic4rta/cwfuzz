#ifndef CWFUZZ_H
#define CWFUZZ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <curl/curl.h>
#include <unistd.h>

#define USER_AGENT "Mozilla/5.0 (X11; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0"
#define MAX_HILOS 1024

typedef struct {
    const char* url;
    const char* archivo;
    int* codigos_estado;
    int cantidad_codigos;
    int cantidad_hilos;
    const char* archivo_resultados;
    FILE* archivo_res;
    pthread_mutex_t mutex;
    CURL* curl_handles[MAX_HILOS];
} Cwfuzz;

typedef struct {
    Cwfuzz* cwfuzz_struct;
    int hilo_id;
} Hilo;

void banner(const Cwfuzz *cwfuzz_struct) {
    printf("Fuzzing: %s | Status code: ", cwfuzz_struct->url);
    for (int i = 0; i < cwfuzz_struct->cantidad_codigos; ++i) {
        printf("%d ", cwfuzz_struct->codigos_estado[i]);
    }
    printf("| Threads: %d\n\n", cwfuzz_struct->cantidad_hilos);
}

void mostrar_resultado(const char* archivo, int codigo_estado) {
    const char* color = "\033[0m";
    if (codigo_estado == 200) {
        color = "\033[32m";
    } else if (codigo_estado >= 300 && codigo_estado < 400) {
        color = "\033[33m";
    } else if (codigo_estado >= 400 && codigo_estado < 500) {
        color = "\033[31m";
    } else if (codigo_estado >= 500) {
        color = "\033[38;5;214m";
    }

    printf("%s%-*s%d\033[0m\n", color, 20, archivo, codigo_estado);
}

void guardar_resultados(Cwfuzz *cwfuzz_struct) {
    if (cwfuzz_struct->archivo_resultados) {
        cwfuzz_struct->archivo_res = fopen(cwfuzz_struct->archivo_resultados, "w");
        if (!cwfuzz_struct->archivo_res) {
            perror("No se pudo crear el archivo para guardar los resultados");
            exit(1);
        }
        pthread_mutex_init(&cwfuzz_struct->mutex, NULL);
        fprintf(cwfuzz_struct->archivo_res, "%s\n\n", cwfuzz_struct->url);
    }
}

void init_hilo_curl(Cwfuzz *cwfuzz_struct) {
    for (int i = 0; i < cwfuzz_struct->cantidad_hilos; ++i) {
        cwfuzz_struct->curl_handles[i] = curl_easy_init();
        if (!cwfuzz_struct->curl_handles[i]) {
            fprintf(stderr, "Error al inicializar el CURL para el hilo %d\n", i);
            exit(1);
        }
    }
}

void liberar_recursos(Cwfuzz *cwfuzz_struct, pthread_t *hilos) {
    for (int i = 0; i < cwfuzz_struct->cantidad_hilos; ++i) {
        pthread_join(hilos[i], NULL);
        curl_easy_cleanup(cwfuzz_struct->curl_handles[i]);
    }

    if (cwfuzz_struct->archivo_res) {
        fclose(cwfuzz_struct->archivo_res);
        pthread_mutex_destroy(&cwfuzz_struct->mutex);
    }

    free(cwfuzz_struct->codigos_estado);
    curl_global_cleanup();
}

int hacer_peticion(const char* url, CURL* curl) {
    CURLcode respuesta;
    int codigo_estado = 0;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
    respuesta = curl_easy_perform(curl);
    if (respuesta == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &codigo_estado);
    }
    return codigo_estado;
}

void construir_url_completa(char* url_completa, size_t url_completa_size, const Cwfuzz* cwfuzz_struct, const char* linea) {
    char* cwfuzz = strstr(cwfuzz_struct->url, "CWFUZZ");

    if (cwfuzz) {
        snprintf(url_completa, url_completa_size, "%.*s%s%s", (int)(cwfuzz - cwfuzz_struct->url), cwfuzz_struct->url, linea, cwfuzz + strlen("CWFUZZ"));
    } else {
        snprintf(url_completa, url_completa_size, "%s/%s", cwfuzz_struct->url, linea);
    }
}

void* fuzz(void* arg) {
    Hilo* datos = (Hilo*) arg;
    Cwfuzz* cwfuzz_struct = datos->cwfuzz_struct;
    int hilo_id = datos->hilo_id;
    char linea[128];
    char url_completa[1024];

    FILE* archivo = fopen(cwfuzz_struct->archivo, "r");
    if (!archivo) {
        perror("No se pudo abrir el archivo");
        pthread_exit(NULL);
    }

    int contador = 0;
    while (fgets(linea, sizeof(linea), archivo)) {
        if (contador % cwfuzz_struct->cantidad_hilos == hilo_id) {
            linea[strcspn(linea, "\n")] = 0;

            construir_url_completa(url_completa, sizeof(url_completa), cwfuzz_struct, linea);

            int codigo_estado = hacer_peticion(url_completa, cwfuzz_struct->curl_handles[hilo_id]);
            for (int i = 0; i < cwfuzz_struct->cantidad_codigos; ++i) {
                if (codigo_estado == cwfuzz_struct->codigos_estado[i]) {
                    mostrar_resultado(linea, codigo_estado);
                    if (cwfuzz_struct->archivo_res) {
                        pthread_mutex_lock(&cwfuzz_struct->mutex);
                        fprintf(cwfuzz_struct->archivo_res, "/%s\t%d\n", linea, codigo_estado);
                        pthread_mutex_unlock(&cwfuzz_struct->mutex);
                    }
                    break;
                }
            }
        }
        contador++;
    }

    fclose(archivo);
    pthread_exit(NULL);
}

void crear_hilos(Cwfuzz *cwfuzz_struct, pthread_t *hilos, Hilo *datos_hilos) {
    for (int i = 0; i < cwfuzz_struct->cantidad_hilos; ++i) {
        datos_hilos[i].cwfuzz_struct = cwfuzz_struct;
        datos_hilos[i].hilo_id = i;
        pthread_create(&hilos[i], NULL, fuzz, (void*) &datos_hilos[i]);
    }
}

#endif /* CWFUZZ_H */