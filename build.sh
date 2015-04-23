#!/bin/bash

x=$(locale | grep LANG=de_DE.UTF-8)
if [ -z $x ]; then
  echo -e "\033[1;37m\033[41mphonepack-de ist auf deutschen UTF-8 Sytemen getestet.\033[m"
  echo -e "\033[1;37m\033[41mDeine Spracheinstellung ist jedoch anders !\033[m"
  echo -e "\033[1;37m\033[41mTeste bitte ob es bei dir funktioniert.\033[m"
fi

echo
echo "Kompiliere..."
gcc -Wall -Wextra -O3 -c basis_func.c -o basis_func.o
gcc -Wall -Wextra -O3 -c phoneshow_func.c -o phoneshow_func.o
gcc -Wall -Wextra -O3 -c main_phonecode-de.c -o main_phonecode-de.o
gcc -Wall -Wextra -O3 -c main_phonecomp-de.c -o main_phonecomp-de.o
gcc -Wall -Wextra -O3 -c main_phoneshow-de.c -o main_phoneshow-de.o
gcc -Wall -Wextra main_phonecode-de.o basis_func.o -o phonecode-de
gcc -Wall -Wextra main_phonecomp-de.o basis_func.o -o phonecomp-de
gcc -Wall -Wextra main_phoneshow-de.o basis_func.o phoneshow_func.o -o phoneshow-de
echo "...Fertig"

rm -f basis_func.o
rm -f phoneshow_func.o
rm -f main_phonecode-de.o
rm -f main_phonecomp-de.o
rm -f main_phoneshow-de.o

exit 0

