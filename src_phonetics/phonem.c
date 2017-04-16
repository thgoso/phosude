/*
phosude phonem.c
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
#include <stdbool.h>
#include "string.h"
#include "phonem.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void make_phonem (const word_t src, phonem_t dest)
{
  phonem_t  code;
  char      tmp[5];
  size_t    pos=0;

  // scr nach dest kopieren, in ASCII Groß wandeln
  strcpy(dest, src);
  str_to_ascii_upper(dest);
  
  // 1. Durchgang = Buchstabenpaare kopieren/nach Regeln codieren, Quelle:dest, Ziel: code
  //                Da es mit dem Ö zu Problemen kommt, wird es vorerst als 0 kodiert
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
bool comp_word_phonem (const word_t word, const phonem_t code, int *lev)
{
  phonem_t  tmp;
  int       dist;
  
  // Vergleichscode erstellen
  make_phonem (word, tmp);
  
  // Falls lev=0 reicht normaler Stringvergleich
  if (*lev == 0) {
    if (strcmp(tmp, code) == 0) return true;
    else return false;
  }
  // sonst lev mit einbeziehen
  else {
    dist = str_lev(tmp, code);
    if (dist <= *lev) {
      *lev = dist;
      return true;
    }
    else {
      *lev = dist;
      return false;
    }
  }
}
