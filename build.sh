#!/bin/bash

x=$(locale | grep LANG=de_DE.UTF-8)
if [ -z $x ]; then
  echo -e "\033[1;37m\033[41mphosude ist auf deutschen UTF-8 Sytemen getestet.\033[m"
  echo -e "\033[1;37m\033[41mDeine Spracheinstellung ist jedoch anders !\033[m"
  echo -e "\033[1;37m\033[41mTeste bitte ob es bei dir funktioniert.\033[m"
fi

echo
echo "Kompiliere..."
echo "cmdparse.c"
gcc -Wall -Wextra -O3 -c cmdparse.c -o cmdparse.o
echo "phonetics.c"
gcc -Wall -Wextra -O3 -c phonetics.c -o phonetics.o
echo "string.c"
gcc -Wall -Wextra -O3 -c string.c -o string.o
echo "stdin.c"
gcc -Wall -Wextra -O3 -c stdin.c -o stdin.o
echo "textprint.c"
gcc -Wall -Wextra -O3 -c textprint.c -o textprint.o
echo "comp.c"
gcc -Wall -Wextra -O3 -c comp.c -o comp.o
echo "main.c"
gcc -Wall -Wextra -O3 -c main.c -o main.o
echo "Linke..."
gcc -Wall -Wextra cmdparse.o phonetics.o string.o stdin.o textprint.o comp.o main.o -o phosude
echo "Fertig"

rm -f cmdparse.o
rm -f phonetics.o
rm -f string.o
rm -f stdin.o
rm -f textprint.o
rm -f comp.o
rm -f main.o

exit 0

