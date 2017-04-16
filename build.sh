#!/bin/bash

x=$(locale | grep LANG=de_DE.UTF-8)
if [ -z $x ]; then
  echo -e "\033[1;37m\033[41mphosude ist auf deutschen UTF-8 Sytemen getestet.\033[m"
  echo -e "\033[1;37m\033[41mDeine Spracheinstellung ist jedoch anders !\033[m"
  echo -e "\033[1;37m\033[41mTeste bitte ob es bei dir funktioniert.\033[m"
fi

echo
echo "Kompiliere..."
echo "string.c"
gcc -Wall -Wextra -O3 -c src_phonetics/string.c -o string.o
echo "soundex.c"
gcc -Wall -Wextra -O3 -c src_phonetics/soundex.c -o soundex.o
echo "exsoundex.c"
gcc -Wall -Wextra -O3 -c src_phonetics/exsoundex.c -o exsoundex.o
echo "cologne.c"
gcc -Wall -Wextra -O3 -c src_phonetics/cologne.c -o cologne.o
echo "phonem.c"
gcc -Wall -Wextra -O3 -c src_phonetics/phonem.c -o phonem.o
echo "caverphone.c"
gcc -Wall -Wextra -O3 -c src_phonetics/caverphone.c -o caverphone.o
echo "daitch-mokotoff.c"
gcc -Wall -Wextra -O3 -c src_phonetics/daitch-mokotoff.c -o daitch-mokotoff.o
echo "cmdparse.c"
gcc -Wall -Wextra -O3 -c src_io/cmdparse.c -o cmdparse.o
echo "stdin.c"
gcc -Wall -Wextra -O3 -c src_io/stdin.c -o stdin.o
echo "textprint.c"
gcc -Wall -Wextra -O3 -c src_io/textprint.c -o textprint.o
echo "comp.c"
gcc -Wall -Wextra -O3 -c src_io/comp.c -o comp.o
echo "phosude_main.c"
gcc -Wall -Wextra -O3 -c phosude_main.c -o phosude_main.o
echo "Erstelle phosude"
gcc -Wall -Wextra string.o soundex.o exsoundex.o cologne.o phonem.o caverphone.o daitch-mokotoff.o cmdparse.o stdin.o textprint.o comp.o phosude_main.o -o phosude
echo "Fertig"

rm -f string.o
rm -f soundex.o
rm -f exsoundex.o
rm -f cologne.o
rm -f phonem.o
rm -f caverphone.o
rm -f daitch-mokotoff.o
rm -f cmdparse.o
rm -f stdin.o
rm -f textprint.o
rm -f comp.o
rm -f phosude_main.o

exit 0

