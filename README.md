# Stun Client Library

This is a very simple STUN client library - `libstuncli` - compatible **only** with the `XOR-MAPPED-ADDRESS` attribute described in [RFC5389](https://datatracker.ietf.org/doc/html/rfc5389).

It was design to be used in a [mptcpd plugin](https://gitlab.com/mptcp_lab/net_check_plugin), therefore it allows to specify the network interface from where the Bind Request is supposed to be sent, allowing it to work in Multihoming scenarios. 

## Building

To build `libstuncli` the following dependencies are required:

* `libstuncli` Build Dependencies
  * C compiler (C99 compliant)
  * [GNU Autoconf](https://www.gnu.org/software/autoconf/)
  * [GNU Automake](https://www.gnu.org/software/automake/)
  * [GNU Libtool](https://www.gnu.org/software/libtool/)
  * [GNU Autoconf Archive](https://www.gnu.org/software/autoconf-archive/)
  * [Pandoc](https://pandoc.org/) >= 2.2.1 (needed to convert `README.md`
    contents from the GitHub markdown format content to plain text)
  * [Doxygen](http://www.doxygen.nl/) (only needed to build
    documentation)

### Bootstrapping

Assuming all build dependencies listed above are installed, Bootstrapping
`libstuncli` simply requires to run the [`bootstrap`](bootstrap) script 
in the top-level source directory, _e.g._:

```sh
$ ./bootstrap

```

### Build Steps

These build steps are the same as the ones found in all Autotool enabled 
software packages, _i.e._ running the `configure` followed by the command 
`make`.

```sh
./configure
make
```

### Instalation

__NOTE__: Installing `libstuncli` requires to be run with `sudo` if 
installing in a directory owned by `root`.

Installing `libstuncli` on any linux system just requires to run:

```sh
make install
```

By default `libstuncli` will be installed in `/usr/local/include` and `
/usr/local/lib`, however this can be changed if in the 
[__Build Steps__](#build-steps) section the option `--prefix` or 
the options `--includedir` and `--libdir` are specified.

 For example, to install `libstuncli` in `/usr/include` and `/usr/lib` 
 one could do:

 ```sh
 ./configure --prefix=/usr/
 make
 sudo make install
 ```
or
 ```sh
 ./configure --includedir=/usr/include --libdir=/usr/lib
 make
 sudo make install
 ```

### Examples

For an example on how to use `libstuncli`, see [example.c](examples/example.c).
It can be compile with:
```sh
make check
```
