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

#### Ejemplo de uso:

```bash
cwfuzz -u https://ic4rta.github.io/ -w wordlist.txt -x 200 -t 20
```

```bash
cwfuzz -u https://ic4rta.github.io/ -w wordlist.txt -x 200,300 -t 20
```
