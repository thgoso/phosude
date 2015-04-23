# phonepack-de

Programmpack für Linux Nutzer die auch mit einem Terminal klar kommen zur phonetischen Namenssuche in Textdateien.
Getestet mit PC Ubuntu Studio 13.10 64 Bit, Lappi Lubuntu 14.10 32 Bit, RaspberryPi Debian Wheezy 32 Bit... alles jeweils mit UTF-8 deutsches System.

Enthalten sind 3 Programme:
phonecode-de zum Erzeugen von phonetischen Codes zu Namen/Worten
phonecomp-de zum Vergleichen von zwei Namen/Worten auf phonetische Gleichheit
phoneshow-de zum Durchsuchen von Text auf phonetisch ähnliche Namen/Worte

Implementiert sind die phonetischen Verfahren: Kölner Phonetik, Phonem, Soundex, Extended Soundex.
Mittelpunkt ist das Programm phoneshow-de mit dem man eine phonetische Wort/Namenssuche in einem Text durchführen kann.
Zusätzlich zu den phonetischen Suchverfahren ist noch ein Levenshtein Filter integriert, sodaß man egal was man sucht, so ziemlich alles findet.
Das Programm funktioniert ähnlich "grep" nur daß eben eine phonetische Suche stattfindet.

cat Liesmich.txt | phoneshow-de -k -p Müller Meier
Zeigt z.B. alle Textzeilen in denen sich phonetisch ähnlichen Worte zu Müller und Meier befinden.
Müller, Mueller, Myllar, Mühlherr, Möller, Mölar, Meier, Mayer, Maier, Maiherr, Neyer usw.

Die Programme sind auf Grund der phonetischen Verfahren ausschließlich für den deutschen Sprachraum angedacht.

