/*
phosude exsoundex.c
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
#include "exsoundex.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void make_exsoundex (const word_t src, exsoundex_t dest)
{
  word_t  tmp;
  size_t  pos=0;
  char    first;

  // src kopieren, nach ASCII Groß wandeln
  strcpy(tmp, src);
  str_to_ascii_upper(tmp);

  // H/W außer am Anfang entfernen
  first = tmp[0];
  tmp[0] = '-';
  str_del_chars(tmp, 'H');
  str_del_chars(tmp, 'W');
  tmp[0] = first;

  // Ersetzungsregeln auf tmp anwenden
  while (tmp[pos] != '\0') {
    switch (tmp[pos]) {
      case 'A': case 'E': case 'H': case 'I': case 'O': case 'U': case 'W': case 'Y':
        tmp[pos] = '0';
        break;
      case 'B': case 'P':
        tmp[pos] = '1';
        break;
      case 'V': case 'F':
        tmp[pos] = '2';
        break;
      case 'C': case 'K': case 'S':
        tmp[pos] = '3';
        break;
      case 'G': case 'J':
        tmp[pos] = '4';
        break;
      case 'Q': case 'X': case 'Z':
        tmp[pos] = '5';
        break;
      case 'D': case 'T':
        tmp[pos] = '6';
        break;
      case 'L':
        tmp[pos] = '7';
        break;
      case 'M': case 'N':
        tmp[pos] = '8';
        break;
      case 'R':
        tmp[pos] = '9';
        break;
    }
    pos++;
  }

  // Doppelte entfernen, dann 0en entfernen, außer 0 am Anfang
  str_del_multiple_chars(tmp);
  first = tmp[0];
  tmp[0] = '-';
  str_del_chars(tmp, '0');
  tmp[0] = first;

  // Auf 5 Stellen kürzen / mit "0" auffüllen
  if (strlen(tmp) < 5) strcat (tmp, "00000");
  tmp[5]='\0';
  
  // Rückgabe füllen
  strcpy(dest, tmp);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool comp_word_exsoundex (const word_t word, const exsoundex_t code, int *lev)
{
  exsoundex_t tmp;
  int         dist;
   
  // Vergleichscode erstellen
  make_exsoundex (word, tmp);
  
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
