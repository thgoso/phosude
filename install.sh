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
gcc -Wall -Wextra -O3 -c phonecode-de.c -o phonecode-de.o
gcc -Wall -Wextra -O3 -c phonecomp-de.c -o phonecomp-de.o
gcc -Wall -Wextra -O3 -c phoneshow-de.c -o phoneshow-de.o
gcc -Wall -Wextra phonecode-de.o phonetics.o -o phonecode-de
gcc -Wall -Wextra phonecomp-de.o phonetics.o -o phonecomp-de
gcc -Wall -Wextra phoneshow-de.o phonetics.o -o phoneshow-de
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
rm -f phonecode-de.o
rm -f phonecomp-de.o
rm -f phoneshow-de.o
rm -f phonecode-de
rm -f phonecomp-de
rm -f phoneshow-de

exit 0

