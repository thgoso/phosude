/*
phosude soundex.c
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
#include "soundex.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void make_soundex (const word_t src, soundex_t dest)
{
  word_t  tmp;
  char    first;
  size_t  pos=0;
  
  // src kopieren, in ASCII-Groß wandeln
  strcpy (tmp, src);
  str_to_ascii_upper(tmp);
  
  // 1. Zeichen merken und in tmp mit '-' ausmaskieren
  first=tmp[0];
  tmp[0]='-';

  // Alle W/H entfernen
  str_del_chars (tmp, 'W');
  str_del_chars (tmp, 'H');

  // Ersetzungsregeln auf tmp anwenden
  while (tmp[pos] != '\0') {
    switch (tmp[pos]) {
      case 'A': case 'I': case 'E': case 'O': case 'U': case 'Y':
        tmp[pos] = '0';
        break;
      case 'B': case 'F': case 'P': case 'V':
        tmp[pos] = '1';
        break;
      case 'C': case 'G': case 'J': case 'K': case 'Q': case 'S': case 'X': case 'Z':
        tmp[pos] = '2';
        break;
      case 'D': case 'T':
        tmp[pos] = '3';
        break;
      case 'L':
        tmp[pos] = '4';
        break;
      case 'M': case 'N':
        tmp[pos] = '5';
        break;
      case 'R':
        tmp[pos] = '6';
        break;
    }
    pos++;
  }

  // Erst doppelte entfernen, dann 0en entfernen
  str_del_multiple_chars(tmp);
  str_del_chars(tmp, '0');

  // gemerktes 1. Zeichen wieder nach dest schieben... Alles auf 4 Stellen kürzen/0en anhängen
  tmp[0]=first;
  if (strlen(tmp) < 4) strcat(tmp, "0000");
  tmp[4] = '\0';
  
  // Rückgabe füllen
  strcpy(dest, tmp);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool comp_word_soundex (const word_t word, const soundex_t code, int *lev)
{
  soundex_t   tmp;
  int         dist;
  
  // Vergleichscode erstellen
  make_soundex (word, tmp);
  
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
