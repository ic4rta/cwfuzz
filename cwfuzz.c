#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <curl/curl.h>
#include <unistd.h>

#define USER_AGENT "Mozilla/5.0 (X11; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0"

typedef struct {
    const char* base_url;
    const char* archivo;
    int* codigos_estado;
    int cantidad_codigos;
    int cantidad_hilos;
    int hilo_id;
    FILE* archivo_resultados;
    pthread_mutex_t* mutex;
} Hilo;

int hacer_peticion(const char* url) {
    CURL *curl;
    CURLcode respuesta;
    int codigo_estado = 0;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
        respuesta = curl_easy_perform(curl);
        if(respuesta == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &codigo_estado);
        }
        curl_easy_cleanup(curl);
    }
    return codigo_estado;
}

void detectar_palabra_reservada(char* url_completa, size_t url_completa_size, const Hilo* hilo_struct, const char* linea) {
    char* cwfuzz = strstr(hilo_struct->base_url, "CWFUZZ");

    if (cwfuzz) {
        snprintf(url_completa, url_completa_size, "%.*s%s%s", (int)(cwfuzz - hilo_struct->base_url), hilo_struct->base_url, linea, cwfuzz + strlen("CWFUZZ"));
    } else {
        snprintf(url_completa, url_completa_size, "%s/%s", hilo_struct->base_url, linea);
    }
}

void* fuzz(void* arg) {
    Hilo* hilo_struct = (Hilo*) arg;
    char linea[128];
    int contador = 0;
    char url_completa[1024];

    FILE *ARCHIVO = fopen(hilo_struct->archivo, "r");
    if (!ARCHIVO) {
        perror("No se pudo abrir el archivo");
        pthread_exit(NULL);
    }

    while (fgets(linea, sizeof(linea), ARCHIVO)) {
        if (contador % hilo_struct->cantidad_hilos == hilo_struct->hilo_id) {
            linea[strcspn(linea, "\n")] = 0;

            detectar_palabra_reservada(url_completa, sizeof(url_completa), hilo_struct, linea);

            int codigo_estado = hacer_peticion(url_completa);
            for (int i = 0; i < hilo_struct->cantidad_codigos; ++i) {
                if (codigo_estado == hilo_struct->codigos_estado[i]) {
                    printf("/%s\t%d\n", linea, codigo_estado);
                    if (hilo_struct->archivo_resultados) {
                        pthread_mutex_lock(hilo_struct->mutex);
                        fprintf(hilo_struct->archivo_resultados, "/%s\t%d\n", linea, codigo_estado);
                        pthread_mutex_unlock(hilo_struct->mutex);
                    }
                    break;
                }
            }
        }
        contador++;
    }

    fclose(ARCHIVO);
    pthread_exit(NULL);
}

void guardar_resultados(const char* filename, const char* content) {
    FILE* ARCHIVO = fopen(filename, "a");
    if (!ARCHIVO) {
        perror("No se pudo crear el archivo");
        return;
    }
    fprintf(ARCHIVO, "%s", content);
    fclose(ARCHIVO);
}

int main(int argc, char* argv[]) {
    int opt;
    const char* url = NULL;
    const char* archivo = NULL;
    int* codigos_estado = NULL;
    int cantidad_codigos = 0;
    int cantidad_hilos = 0;
    const char* archivo_resultados = NULL;
    FILE* archivo_res = NULL;
    pthread_mutex_t mutex;

    while ((opt = getopt(argc, argv, "u:w:x:t:o:")) != -1) {
        switch (opt) {
            case 'u':
                url = optarg;
                break;
            case 'w':
                archivo = optarg;
                break;
            case 'x':
                {
                    char* token = strtok(optarg, ",");
                    while (token != NULL) {
                        codigos_estado = realloc(codigos_estado, (cantidad_codigos + 1) * sizeof(int));
                        codigos_estado[cantidad_codigos++] = atoi(token);
                        token = strtok(NULL, ",");
                    }
                }
                break;
            case 't':
                cantidad_hilos = atoi(optarg);
                break;
            case 'o':
                archivo_resultados = optarg;
                break;
            default:
                fprintf(stderr, "Uso: %s -u <url> -w <archivo> -x <codigo-estado> -t <hilos> [-o <archivo_resultados>]\n", argv[0]);
                return 1;
        }
    }

    if (url == NULL || archivo == NULL || codigos_estado == NULL || cantidad_codigos == 0 || cantidad_hilos == 0) {
        fprintf(stderr, "Uso: %s -u <url> -w <archivo> -x <codigo-estado...> -t <hilos> [-o <archivo_resultados>]\n", argv[0]);
        return 1;
    }

    if (archivo_resultados) {
        archivo_res = fopen(archivo_resultados, "w");
        if (!archivo_res) {
            perror("No se pudo abrir el archivo de resultados");
            return 1;
        }
        pthread_mutex_init(&mutex, NULL);
        fprintf(archivo_res, "%s\n\n", url);
    }

    pthread_t hilos[cantidad_hilos];
    Hilo thread_hilo_struct[cantidad_hilos];

    printf("Fuzzing: %s | Status code: ", url);
    for (int i = 0; i < cantidad_codigos; ++i) {
        printf("%d ", codigos_estado[i]);
    }
    printf("| Threads: %d\n\n", cantidad_hilos);

    for (int i = 0; i < cantidad_hilos; ++i) {
        thread_hilo_struct[i].base_url = url;
        thread_hilo_struct[i].archivo = archivo;
        thread_hilo_struct[i].codigos_estado = codigos_estado;
        thread_hilo_struct[i].cantidad_codigos = cantidad_codigos;
        thread_hilo_struct[i].cantidad_hilos = cantidad_hilos;
        thread_hilo_struct[i].hilo_id = i;
        thread_hilo_struct[i].archivo_resultados = archivo_res;
        thread_hilo_struct[i].mutex = &mutex;
        pthread_create(&hilos[i], NULL, fuzz, (void*) &thread_hilo_struct[i]);
    }

    for (int i = 0; i < cantidad_hilos; ++i) {
        pthread_join(hilos[i], NULL);
    }

    if (archivo_res) {
        fclose(archivo_res);
        pthread_mutex_destroy(&mutex);
    }

    free(codigos_estado);
    return 0;
}
