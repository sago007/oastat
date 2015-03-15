# Introduction #

This guide shown how to build the program. It should be noted that the program is rather `*`nix-specific in its way to read from stdin and the provided make file also only includes compiling for a Linux-system or the like.


# Details #

## Requirements ##

  * libgcrypt11-dev
  * libdbi-dev
  * [libdbixx-dev](http://sourceforge.net/projects/cppcms/files/dbixx/0.0.4/)
  * libboost-program-options-dev (testet with 1.54)

## Download ##

From git:
```
git clone https://code.google.com/p/oastat/ 
```

## Building ##
```
cd oastat/src
make
```
The result is located in the build