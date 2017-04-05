/*
phoneshow_str

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
#include "bool.h"
#include "string.h"
#include "phonetics.h"


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Kopiert String von *src nach *dest sofern String nicht zu lang ist
// Falls zu lang wird false zurückgegeben und in *dest '\0'
// Bei Erfolg wird true zurückgegeben
bool str_to_word_type (const char *src, word_t *dest)
{
  if (strlen(src) >= BUFFSIZE_WORD) {
    dest->s[0] = '\0';
    return false;
  }
  
  strcpy(dest->s, src);
  return true;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// String in ASCII GROSSBUCHSTABEN wandeln, deutsche Umlaute ersetzten
// Prüfen ob danach ein Wort raus kommt
bool str_to_ascii_upper_word (char *s)
{
  const char  uml[7][3] = {{"Ä"}, {"Ö"}, {"Ü"}, {"ä"}, {"ö"}, {"ü"}, {"ß"}};
  const char  rep[7][3] = {{"AE"},{"OE"},{"UE"},{"AE"},{"OE"},{"UE"},{"SS"}};
  int         cnt;
  int         replaced;
  size_t      pos=0;
  
  if (s[0] == '\0') return false;
  
  while (s[pos] != '\0') {
    // A-Z
    if ((s[pos] >= 'A') && (s[pos] <= 'Z')) {
      pos++;
      continue;
    }
    // a-z
    if ((s[pos] >= 'a') && (s[pos] <= 'z')) {
      s[pos] -= 32;
      pos++;
      continue;
    }
    // Umlaute 0-6
    replaced=0;
    for (cnt=0; cnt<=6; cnt++) {
      if ((s[pos] == uml[cnt][0]) && (s[pos+1] == uml[cnt][1])) {
        s[pos] = rep[cnt][0];
        s[pos+1] = rep[cnt][1];
        pos+=2;
        replaced=1;
        break;
      }
    }
    if (replaced == 1) continue;
    // Verbotenes Zeichen angetroffen... String verwerfen
    s[0]='\0';
    return false;
  }
  // Ohne Fehler durch
  return true;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// entfernt einzelne ASCII Zeichen im String die dem übergebenen Zeichen (c) entsprechen,
// sodaß diese Zeichen gelöscht werden
void str_del_ascii_chars (char *s, const char c)
{
  size_t  pos_read=0;
  size_t  pos_write=0;

  // Sicherheitshalber
  if (c == '\0') return;

  while (1) {
    if (s[pos_read] == '\0') {
      s[pos_write] = '\0';
      break;
    }
    else if (s[pos_read] == c) pos_read++;
    else {
      s[pos_write] = s[pos_read];
      pos_read++;
      pos_write++;
    }
  }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// entfernt alle mehrfach hintereinander auftretenden ASCII Zeichen im String,
// sodaß nur noch je eines vorhanden ist
void str_del_multiple_ascii_chars (char *s)
{
  size_t  pos_read=0;
  size_t  pos_write=0;
  char    last='\0';

  while (1) {
    if (s[pos_read] == '\0') {
      s[pos_write] = '\0';
      break;
    }
    else if (s[pos_read] == last) pos_read++;
    else {
      last = s[pos_read];
      s[pos_write] = last;
      pos_read++;
      pos_write++;
    }
  }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Levenshtein-Distanz aus http://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance
// Berechnet die Levenshtein-Distanz zweier übergebener Strings
// Was in dem Fall auf das Wort direkt oder den erzeugten phonetischen Code angewandt werden kann
// Übergabe:  Pointer auf Wort1 und Wort2
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
