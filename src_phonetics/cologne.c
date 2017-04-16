/*
phosude cologne.c
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
#include "cologne.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void make_cologne (const word_t src, cologne_t dest)
{
  cologne_t tmp;
  char      first;
  char      group[3];
  size_t    size;
  size_t    pos;

  // Wegen lesen in 3er Gruppen kopieren wir src nach tmp
  // und erweitern den übergebenen String hinten und vorn um ein Leerzeichen
  // Danach wandeln in ASCII Großbuchstaben
  tmp[0]=' ';
  tmp[1]='\0';
  strcat(tmp, src);
  strcat(tmp, " ");
  str_to_ascii_upper(tmp);
  
  // Wort wird in Gruppen von 3 Buchstaben gelesen, da es Regeln für "NACH" und "VOR" Buchstabe gibt
  // Der Referenzbuchstabe ist immer der in der Mitte der 3er Gruppe
  // Lesen von tmp, und nach Regeln konvertiert in dest
  size = strlen(tmp);
  dest[0]='\0';
  for (pos=1; pos<=size - 2; pos++) {
    // In 3er Gruppen lesen
    group[0]=tmp[pos-1];
    group[1]=tmp[pos];
    group[2]=tmp[pos+1];

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
bool comp_word_cologne (const word_t word, const cologne_t code, int *lev)
{
  cologne_t   tmp;
  int         dist;
    
  // Vergleichscode erstellen
  make_cologne (word, tmp);
  
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
