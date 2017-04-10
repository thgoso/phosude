/*
phosude phonetics.c
Copyright (C) 2015-2017, Thomas Gollmer, th_goso@freenet.de
Dieses Programm ist freie Software. Sie können es unter den Bedingungen der GNU General Public License,
wie von der Free Software Foundation veröffentlicht, weitergeben und/oder modifizieren,
entweder gemäß Version 3 der Lizenz oder (nach Ihrer Option) jeder späteren Version.

Die Veröffentlichung dieses Programms erfolgt in der Hoffnung, daß es Ihnen von Nutzen sein wird,
aber OHNE IRGENDEINE GARANTIE, sogar ohne die implizite Garantie der
MARKTREIFE oder der VERWENDBARKEIT FÜR EINEN BESTIMMTEN ZWECK.
Details finden Sie in der GNU General Public License. Sie sollten ein Exemplar der GNU General Public License
zusammen mit diesem Programm erhalten haben. Falls nicht, siehe <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include "phosude.h"
#include "string.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Private Funktionen phonetische Codierung
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Soundex Verfahren
static void make_soundex (phcode_t dest)
{
  char    first;
  size_t  size;
  size_t  pos=0;
  
  // 1. Zeichen aus dest merken und in dest mit '-' ausmaskieren
  first=dest[0];
  dest[0]='-';

  // Ersetzungsregeln auf dest anwenden
  while (dest[pos] != '\0') {
    switch (dest[pos]) {
      case 'A': case 'E': case 'H': case 'I': case 'O': case 'U': case 'W': case 'Y':
        dest[pos] = '0';
        break;
      case 'B': case 'F': case 'P': case 'V':
        dest[pos] = '1';
        break;
      case 'C': case 'G': case 'J': case 'K': case 'Q': case 'S': case 'X': case 'Z':
        dest[pos] = '2';
        break;
      case 'D': case 'T':
        dest[pos] = '3';
        break;
      case 'L':
        dest[pos] = '4';
        break;
      case 'M': case 'N':
        dest[pos] = '5';
        break;
      case 'R':
        dest[pos] = '6';
        break;
    }
    pos++;
  }

  // Doppelte entfernen, dann 0en entfernen
  str_del_multiple_chars(dest);
  str_del_chars(dest, '0');

  // gemerktes 1. Zeichen wieder nach dest schieben... Alles auf 4 Stellen kürzen/0en anhängen
  dest[0]=first;
  size=strlen(dest);
  if (size == 1) strcat (dest, "000");
  else if (size == 2) strcat (dest, "00");
  else if (size == 3) strcat (dest, "0");
  else if (size > 4) dest[4]='\0';
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Extended Soundex Verfahren
// Wenn Wort in dest Platz hat, paßt der Code auch da hinein
static void make_exsoundex (phcode_t dest)
{
  size_t  size;
  size_t  pos=0;

  // Ersetzungsregeln auf dest anwenden
  while (dest[pos] != '\0') {
    switch (dest[pos]) {
      case 'A': case 'E': case 'H': case 'I': case 'O': case 'U': case 'W': case 'Y':
        dest[pos] = '0';
        break;
      case 'B': case 'P':
        dest[pos] = '1';
        break;
      case 'V': case 'F':
        dest[pos] = '2';
        break;
      case 'C': case 'K': case 'S':
        dest[pos] = '3';
        break;
      case 'G': case 'J':
        dest[pos] = '4';
        break;
      case 'Q': case 'X': case 'Z':
        dest[pos] = '5';
        break;
      case 'D': case 'T':
        dest[pos] = '6';
        break;
      case 'L':
        dest[pos] = '7';
        break;
      case 'M': case 'N':
        dest[pos] = '8';
        break;
      case 'R':
        dest[pos] = '9';
        break;
    }
    pos++;
  }

  // Doppelte entfernen, dann 0en entfernen
  str_del_multiple_chars(dest);
  str_del_chars(dest, '0');

  // Auf 5 Stellen kürzen / mit "0" auffüllen
  size=strlen(dest);
  if (size == 0) strcat (dest, "00000");
  else if (size == 1) strcat (dest, "0000");
  else if (size == 2) strcat (dest, "000");
  else if (size == 3) strcat (dest, "00");
  else if (size == 4) strcat (dest, "0");
  else dest[5]='\0';
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Kölner Phonetik Verfahren
static void make_cologne (phcode_t dest)
{
  phcode_t  code;
  char      first;
  char      group[3];
  size_t    size;
  size_t    pos;

  // Wegen lesen in 3er Gruppen kopieren wir dest nach code
  // und erweitern den übergebenen String hinten und vorn um ein Leerzeichen
  code[0]=' ';
  code[1]='\0';
  strcat(code, dest);
  strcat(code, " ");

  // Wort wird in Gruppen von 3 Buchstaben gelesen, da es Regeln für "NACH" und "VOR" Buchstabe gibt
  // Der Referenzbuchstabe ist immer der in der Mitte der 3er Gruppe
  // Lesen von code, und nach Regeln konvertiert in dest
  size = strlen(code);
  dest[0]='\0';
  for (pos=1; pos<=size - 2; pos++) {
    // In 3er Gruppen lesen
    group[0]=code[pos-1];
    group[1]=code[pos];
    group[2]=code[pos+1];

    // Sonderfälle Wortanfang
    if (pos==1) {
      if (group[1] == 'C') {
        if (group[2] == 'A' || group[2] == 'H' || group[2] == 'K' ||
          group[2] == 'L' || group[2] == 'O' || group[2] == 'Q' ||
          group[2] == 'R' || group[2] == 'U' || group[2] == 'X') {
            strcat(dest, "4");
            continue;
        }
        else {
          strcat(dest, "8");
          continue;
        }
      }
    }

    // Normale codierung KEIN Wortanfang
    if (group[1] == 'P' && group[2] == 'H') {
      strcat (dest, "3");
      continue;
    }
    if ((group[1] == 'D' && group[2] == 'S') || (group[1] == 'D' && group[2] == 'C') ||
      (group[1] == 'D' && group[2] == 'Z') || (group[1] == 'T' && group[2] == 'S') ||
      (group[1] == 'T' && group[2] == 'C') || (group[1] == 'T' && group[2] == 'Z')) {
        strcat(dest, "8");
        continue;
    }
    if (group[1] == 'D' || group[1] == 'T') {
      strcat(dest, "2");
      continue;
    }
    if ((group[0] == 'C' && group[1] == 'X') || (group[0] == 'K' && group[1] == 'X') ||
      (group[0] == 'Q' && group[1] == 'X')) {
        strcat(dest, "8");
        continue;
    }
    if (group[1] == 'X') {
      strcat(dest, "48");
      continue;
    }
    if ((group[0] == 'S' && group[1] == 'C') || (group[0] == 'S' && group[1] == 'Z')) {
      strcat(dest, "8");
      continue;
    }
    if (group[1] == 'C') {
      if (group[2] == 'A' || group[2] == 'O' || group[2] == 'U' || group[2] == 'H' ||
        group[2] == 'K' || group[2] == 'X' || group[2] == 'Q') {
          strcat(dest, "4");
          continue;
      }
      else {
        strcat(dest, "8");
        continue;
      }
    }
    if (group[1] == 'H') {
      strcat(dest, "H");
      continue;
    }
    if (group[1] == 'A' || group[1] == 'E' || group[1] == 'I' || group[1] == 'J' ||
      group[1] == 'Y' || group[1] == 'O' || group[1] == 'U') {
        strcat(dest, "0");
        continue;
    }
    if (group[1] == 'B' || group[1] == 'P') {
      strcat(dest, "1");
      continue;
    }
    if (group[1] == 'F' || group[1] == 'V' || group[1] == 'W') {
      strcat(dest, "3");
      continue;
    }
    if (group[1] == 'G' || group[1] == 'K' || group[1] == 'Q') {
      strcat(dest, "3");
      continue;
    }
    if (group[1] == 'L') {
      strcat(dest, "5");
      continue;
    }
    if (group[1] == 'R') {
      strcat(dest, "7");
      continue;
    }
    if (group[1] == 'M' || group[1] == 'N') {
      strcat(dest, "6");
      continue;
    }
    if (group[1] == 'S' || group[1] == 'Z') {
      strcat(dest, "8");
      continue;
    }
  }

  // doppelte entfernen, H entfernen
  str_del_multiple_chars(dest);
  str_del_chars(dest, 'H');

  // 0 Außer am Anfang entfernen
  first=dest[0];
  dest[0]='-';
  str_del_chars(dest, '0');
  dest[0]=first;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Phonem Verfahren
static void make_phonem (phcode_t dest)
{
  phcode_t  code;
  char      tmp[5];
  size_t    pos=0;

  // 1. Durchgang = Buchstabenpaare kopieren/nach Regeln codieren, Quelle:dest, Ziel: code
  //                Da es mit dem ö zu Problemen kommt, wird es vorerst als 0 kodiert
  code[0]='\0';
  while (dest[pos] != '\0') {
    // AE
    if (dest[pos] == 'A' && dest[pos+1] == 'E') {
      strcat(code, "E");
      pos+=2;
    }
    // PF
    else if (dest[pos] == 'P' && dest[pos+1] == 'F') {
      strcat(code, "V");
      pos+=2;
    }
    // KS
    else if (dest[pos] == 'K' && dest[pos+1] == 'S') {
      strcat(code, "X");
      pos+=2;
    }
    // QU
    else if (dest[pos] == 'Q' && dest[pos+1] == 'U') {
      strcat(code, "KW");
      pos+=2;
    }
    // OE muß eigentlich zu Ö werden, was aber Doppelbytezeichen wäre
    // Das ließe sich aber mit Routine zum entfernen von Doppelten Zeichen nicht
    // Fehlerfrei abarbeiten, daher machen wir einfach erst mal eine 0 draus (Einybytezeichen)
    else if (dest[pos] == 'O' && dest[pos+1] == 'E') {
      strcat(code, "0");
      pos+=2;
    }
    // EU
    else if (dest[pos] == 'E' && dest[pos+1] == 'U') {
      strcat(code, "OY");
      pos+=2;
    }
    // OU
    else if (dest[pos] == 'O' && dest[pos+1] == 'U') {
      strcat(code, "U");
      pos+=2;
    }
    // EI,EY
    else if ((dest[pos] == 'E' && dest[pos+1] == 'I') || (dest[pos] == 'E' && dest[pos+1] == 'Y')) {
      strcat(code, "AY");
      pos+=2;
    }
    // SC,SZ,CZ,TZ,TS
    else if ((dest[pos] == 'S' && dest[pos+1] == 'C') ||
      (dest[pos] == 'S' && dest[pos+1] == 'Z') ||
      (dest[pos] == 'C' && dest[pos+1] == 'Z') ||
      (dest[pos] == 'T' && dest[pos+1] == 'Z') ||
      (dest[pos] == 'T' && dest[pos+1] == 'S')) {
        strcat(code, "C");
        pos+=2;
    }
    else {
      // Wenn Buchstabenpaar nicht nach Regeln kodiert werden konnte
      // nur einen Buchstaben kopieren und weiter ab dem nächsten mit Paarsuche
      tmp[0]=dest[pos];
      tmp[1]='\0';
      strcat (code, tmp);
      pos++;
    }
  }

  // 2. Durchgang einzelne Buchstaben ersetzten/kodieren in code
  pos=0;
  while (code[pos] != '\0') {
    switch (code[pos]) {
      case 'Z': case 'K': case 'G': case 'Q':
        code[pos] = 'C';
        break;
      case 'U': case 'I': case 'J':
        code[pos] = 'Y';
        break;
      case 'F': case 'W':
        code[pos] = 'V';
        break;
      case 'A':
        code[pos] = 'E';
        break;
      case 'P':
        code[pos] = 'B';
        break;
      case 'T':
        code[pos] = 'D';
        break;
    }
    pos++;
  }

  // 3. Durchgang alle doppelten Zeichen entfernen in code
  // Da das Ö als 0 codiert ist, bekommen wir doppelte davon einfach weg
  str_del_multiple_chars(code);

  // 4. Durchgang alles von code nach dest kopieren, unerlaubte Zeichen dabei außlassen
  // Jetzt erst aus 0 das endgültige Ö machen
  pos=0;
  dest[0]='\0';
  while (code[pos] != '\0') {
    if (code[pos] == 'A' || code[pos] == 'B' || code[pos] == 'C' || code[pos] == 'D' || code[pos] == 'L' ||
      code[pos] == 'M' || code[pos] == 'N' || code[pos] == 'O' || code[pos] == 'R' || code[pos] == 'S' ||
      code[pos] == 'U' || code[pos] == 'V' || code[pos] == 'W' || code[pos] == 'X' || code[pos] == 'Y') {
        tmp[0] = code[pos];
        tmp[1] = '\0';
        strcat (dest, tmp);
    }
    else if (code[pos] == '0') strcat (dest, "Ö");
    pos++;
  }

  // Falls Wort keinen gültigen Code erzeugt... "---" zurückgeben, sonst Code
  if (dest[0] == '\0') strcpy(dest, "---");
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Öffentliche Funktion
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// phonetschen Code erstellen
// führt Vorkonvertierung für phonetische Routinen durch
void phonetics_get_code (const word_t src, phcode_t dest, const int code_no)
{
  // src nach dest kopieren in ASCII Großbuchstaben wandeln
  strcpy (dest, src);
  str_to_ascii_upper(dest);
    
  // Eigentliche Codierroutine anspringen
  // Diese kann jetzt auf/mit Vorkonvertiertem Wort in dest arbeiten
  if (code_no == PH_COLOGNE) make_cologne (dest);
  else if (code_no == PH_PHONEM) make_phonem (dest);
  else if (code_no == PH_SOUNDEX) make_soundex (dest);
  else if (code_no == PH_EXSOUNDEX) make_exsoundex (dest);
  
  // Fehlerhafte Codenummer
  else dest[0] = '\0';
}
