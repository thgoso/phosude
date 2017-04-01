#!/bin/bash

if ! [ $(id -u) = 0 ]; then
   echo "Script muß als root ausgeführt werden !"
   exit 1
fi

x=$(locale | grep LANG=de_DE.UTF-8)
if [ -z $x ]; then
  echo -e "\033[1;37m\033[41mphonepack-de ist auf deutschen UTF-8 Sytemen getestet.\033[m"
  echo -e "\033[1;37m\033[41mDeine Spracheinstellung ist jedoch anders !\033[m"
  echo -e "\033[1;37m\033[41mTeste bitte ob es bei dir funktioniert.\033[m"
fi

echo
echo "Kompiliere..."
gcc -Wall -Wextra -O3 -c phonetics.c -o phonetics.o
gcc -Wall -Wextra -O3 -c phonecode.c -o phonecode.o
gcc -Wall -Wextra -O3 -c phonecomp.c -o phonecomp.o
gcc -Wall -Wextra -O3 -c phoneshow.c -o phoneshow.o
gcc -Wall -Wextra -O3 -c phoneshow_read.c -o phoneshow_read.o
gcc -Wall -Wextra -O3 -c phoneshow_write.c -o phoneshow_write.o
gcc -Wall -Wextra -O3 -c phoneshow_str.c -o phoneshow_str.o
gcc -Wall -Wextra -O3 -c phoneshow_comp.c -o phoneshow_comp.o
echo "Linke..."
gcc -Wall -Wextra phonetics.o phonecode.o -o phonecode-de
gcc -Wall -Wextra phonetics.o phonecomp.o -o phonecomp-de
gcc -Wall -Wextra phonetics.o phoneshow.o phoneshow_read.o phoneshow_write.o phoneshow_str.o phoneshow_comp.o -o phoneshow-de
echo "...Fertig"

echo "Installiere nach:"
echo "/usr/local/bin/phonecode_de"
echo "/usr/local/bin/phonecomp_de"
echo "/usr/local/bin/phoneshow_de"
echo "zum deinstallieren sudo bash uninstall.sh aufrufen !"
cp phonecode-de /usr/local/bin/phonecode-de
cp phonecomp-de /usr/local/bin/phonecomp-de
cp phoneshow-de /usr/local/bin/phoneshow-de

rm -f phonetics.o
rm -f phonecode.o
rm -f phonecomp.o
rm -f phoneshow.o
rm -f phoneshow_read.o
rm -f phoneshow_write.o
rm -f phoneshow_str.o
rm -f phoneshow_comp.o

exit 0

