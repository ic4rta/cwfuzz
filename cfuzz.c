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
} Hilo;

int hacer_peticion(const char* url) {
    CURL *curl;
    CURLcode respuesta;
    long codigo_estado = 0;

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
    return (int)codigo_estado;
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
            snprintf(url_completa, sizeof(url_completa), "%s/%s", hilo_struct->base_url, linea);
            int codigo_estado = hacer_peticion(url_completa);
            for (int i = 0; i < hilo_struct->cantidad_codigos; ++i) {
                if (codigo_estado == hilo_struct->codigos_estado[i]) {
                    printf("/%s\t%d\n", linea, codigo_estado);
                    break;
                }
            }
        }
        contador++;
    }

    fclose(ARCHIVO);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    int opt;
    const char* url = NULL;
    const char* archivo = NULL;
    int* codigos_estado = NULL;
    int cantidad_codigos = 0;
    int cantidad_hilos = 0;

    while ((opt = getopt(argc, argv, "u:w:x:t:")) != -1) {
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
            default:
                fprintf(stderr, "Uso: %s -u <url> -w <archivo> -x <codigo-estado> -t <hilos>\n", argv[0]);
                return 1;
        }
    }

    if (url == NULL || archivo == NULL || codigos_estado == NULL || cantidad_codigos == 0 || cantidad_hilos == 0) {
        fprintf(stderr, "Uso: %s -u <url> -w <archivo> -x <codigo-estado...> -t <hilos>\n", argv[0]);
        return 1;
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
        pthread_create(&hilos[i], NULL, fuzz, (void*) &thread_hilo_struct[i]);
    }

    for (int i = 0; i < cantidad_hilos; ++i) {
        pthread_join(hilos[i], NULL);
    }

    free(codigos_estado);
    return 0;
}