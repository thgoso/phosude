#!/bin/bash

if ! [ $(id -u) = 0 ]; then
   echo "Script muß als root ausgeführt werden !"
   exit 1
fi

echo "Lösche:"
echo "/usr/local/bin/phonecode-de"
echo "/usr/local/bin/phonecomp-de"
echo "/usr/local/bin/phoneshow-de"

rm -f phonecode-de /usr/local/bin/phonecode-de
rm -f phonecomp-de /usr/local/bin/phonecomp-de
rm -f phoneshow-de /usr/local/bin/phoneshow-de


exit 0

