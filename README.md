[Español](#Español)
[English](#English)

# Español

## cwfuzz

Hice por que queria aprender a usar ```libcurl```, para instalarla:

**NetBSD**

```bash
pkgin install libcurl-gnutls
```

**Debian y Ubuntu based**

```bash
apt install libcurl4-gnutls-dev
apt install libcurl4-openssl-dev (opcional)
```

**Arch based**

```bash
pacman -S libcurl-gnutls
```

#### Instalar la herramienta

```bash
sudo make install
```

## Uso

### Opciones:

- u: url

- w: wordlist 

- x: codigos de estados los cuales hara match, puede recibir varios separados por comas

- t: cantidad de hilos

- o: archivo a guardar los resultados (opcional)

- a: user-agent (opcional, se usa uno por defecto)

Tambien el programa incluye la palabra reservada "CWFUZZ" que tiene el mismo funcionamiento como en las herramientas wfuzz o ffuf.

La enumeracion de subdominios se hace en modo VHOST, el dominio debe ir sin `http` o `https` y `www`

### Ejemplo de uso:

> [!IMPORTANT]  
> Recomiendo siempre poner en la opcion -x, tambien el codigo de estado 301, ya que por defecto no sigue las redirecciones


```bash
cwfuzz -u https://ic4rta.github.io/ -w wordlist.txt -x 200 -t 20
```

Multiples codigos de estado

```bash
cwfuzz -u https://ic4rta.github.io/ -w wordlist.txt -x 200,301 -t 20
```

Guardar la salida en el archivo /tmp/fuzz

```bash
cwfuzz -u https://ic4rta.github.io/CWFUZZ -w wordlist.txt -x 200,301 -t 20 -o /tmp/fuzz
```

Poner un User-Agent personalizado

```bash
cwfuzz -u https://ic4rta.github.io/CWFUZZ -w wordlist.txt -x 200,301 -t 20 -o /tmp/fuzz -a "Cwfuzz"
```

Enumerar subdominios (VHOST)

```bash
cwfuzz -u CWFUZZ.jupiter.htb -w wordlist.txt -x 200,301 -t 20
```

# English

## cwfuzz

I did because I wanted to learn how to use `libcurl`, to install it:

**NetBSD**

```bash
pkgin install libcurl-gnutls
```

**Debian y Ubuntu based**

```bash
apt install libcurl4-gnutls-dev
apt install libcurl4-openssl-dev (opcional)
```

**Arch based**

```bash
pacman -S libcurl-gnutls
```

## Use

### Options


- u: url

- w: wordlist 

- x: state codes which will be matched, you can enter several separated by commas

- t: number of threads

- o: file to save the results (optional)

- a: user-agent (optional, one is used by default)


Also the program includes the reserved word “CWFUZZ” which has the same functionality as in the wfuzz or ffuf tools

The enumeration of subdomains is done in VHOST mode, the domain must go without `http` or `https` and `www`

### Examples:

> [!IMPORTANT]  
> I recommend to always put in the -x option, also the status code 301, because by default it does not follow the redirects

```bash
cwfuzz -u https://ic4rta.github.io/ -w wordlist.txt -x 200 -t 20
```

Multiple status codes

```bash
cwfuzz -u https://ic4rta.github.io/ -w wordlist.txt -x 200,301 -t 20
```

Save output to file /tmp/fuzz

```bash
cwfuzz -u https://ic4rta.github.io/CWFUZZ -w listapalabras.txt -x 200,301 -t 20 -o /tmp/fuzz
```

Set a custom User-Agent

```bash
cwfuzz -u https://ic4rta.github.io/CWFUZZ -w wordlist.txt -x 200,301 -t 20 -o /tmp/fuzz -a “Cwfuzz”
```

Enumerate subdomains (VHOST)

```bash
cwfuzz -u CWFUZZ.jupiter.htb -w wordlist.txt -x 200,301 -t 20
```
