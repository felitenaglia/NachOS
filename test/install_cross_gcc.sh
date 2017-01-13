#!/bin/bash
tar xvzf gcc-mips.tar.gz
mkdir "$PWD/lib"
ln -s "$PWD/mips-dec-ultrix42" "$PWD/lib/gcc-lib"
echo -e "\n\nAhora debe reemplazar en test/Makefile la variable GCCDIR para que apunte a $PWD/mips-dec-ultrix42/mips- \n\n"
