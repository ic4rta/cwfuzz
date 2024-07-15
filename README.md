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

#### Opciones:

-u: url

-w: wordlist 

-x: codigos de estados los cuales hara match, puede recibir varios separados por comas

-t: cantidad de hilos

-o: archivo a guardar los resultados (opcional)

-a: user-agent (opcional, se usa uno por defecto)

Tambien el programa incluye la palabra reservada "CWFUZZ" que tiene el mismo funcionamiento como en las herramientas wfuzz o ffuf

#### Ejemplo de uso:

Recomiendo siempre poner en la opcion -x, tambien el codigo de estado 301, ya que por defecto no sigue las redirecciones

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
