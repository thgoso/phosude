/*
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
#include "string.h"


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// String in GROSSBUCHSTABEN wandeln, deutsche Umlaute inclusive
void str_to_upper (char *s)
{
  const char  uml[4][3] = {{"ä"},{"ö"},{"ü"},{"ß"}};
  const char  rep[4][3] = {{"Ä"},{"Ö"},{"Ü"},{"SS"}};
  int         cnt;
  size_t      pos=0;
  
  while (s[pos] != '\0') {
    // a-z
    if ((s[pos] >= 'a') && (s[pos] <= 'z')) {
      s[pos] -= 32;
      pos++;
      continue;
    }
    // Umlaute 0-3
    for (cnt=0; cnt<=3; cnt++) {
      if ((s[pos] == uml[cnt][0]) && (s[pos+1] == uml[cnt][1])) {
        s[pos] = rep[cnt][0];
        s[pos+1] = rep[cnt][1];
        pos++;
        break;
      }
    }
    pos++;
  }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Levenshtein-Distanz aus http://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance
// Berechnet die Levenshtein-Distanz zweier übergebener Strings
// Rückgabe:  Distanz als int (immer positiv)
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int str_lev (const char *s1, const char *s2)
{
  unsigned int s1len, s2len, x, y, lastdiag, olddiag;
  s1len = strlen(s1);
  s2len = strlen(s2);
  unsigned int column[s1len+1];

  for (y = 1; y <= s1len; y++)
    column[y] = y;
  for (x = 1; x <= s2len; x++) {
    column[0] = x;
    for (y = 1, lastdiag = x-1; y <= s1len; y++) {
      olddiag = column[y];
      column[y] = MIN3(column[y] + 1, column[y-1] + 1, lastdiag + (s1[y-1] == s2[x-1] ? 0 : 1));
      lastdiag = olddiag;
    }
  }
  return (column[s1len]);
}
