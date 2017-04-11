/*
phosude string.c
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

#define NUMBER_OF_UML   7
static const char Uml[NUMBER_OF_UML][3] = {{"Ä"}, {"Ö"}, {"Ü"}, {"ä"}, {"ö"}, {"ü"}, {"ß"}};
static const char Rep[NUMBER_OF_UML][3] = {{"AE"},{"OE"},{"UE"},{"AE"},{"OE"},{"UE"},{"SS"}};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Private Funktionen
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Findet das erste Auftreten einer Buchstabengruppe gleicher Buchstaben im String
// Übergabe:  String in "s"
//            Buchstabe dessen Gruppe gefunden werden soll "letter"
// Rückgabe:  true bei Fund   Startposition der Gruppe in "start"
//                            Länge der Gruppe in "len"
//            false wenn keine Gruppe von Buchstaben bestehend aus "letter" gefunden wurde             
static bool find_group (const char *s, const char letter, size_t *start, size_t *len)
{
  size_t  pos=0;
  
  while (s[pos] != '\0') {
    if (s[pos] != letter) {
      pos++;
      continue;
    }
    else {
      *start = pos;
      *len=0;
      while (s[pos] == letter) {
        *len+=1;
        pos++;
      }
      if (*len > 1) return true;
    }
  }    
  return false;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Öffentliche Funktionen
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// String dynamischer Länge in Fixstring wandeln, sofern er nur aus deutschen Buchstaben besteht
int str_to_word (const char *src, word_t dest)
{
  int     cnt;
  size_t  pos=0;

  // Paßt in Buffer, dann kopieren
  if (strlen(src) >= BUFFSIZE_WORD) {
    dest[0] = '\0';
    return STAT_ERR_OVERLEN_WORD;
  }
  else strcpy(dest, src);
  
  // Leerstrings sind keine Worte
  if (dest[0] == '\0') return STAT_ERR_NO_WORD;

NextChar:
  if (dest[pos] == '\0') return STAT_SUCCESS;
  if ((dest[pos] >= 'A') && (dest[pos] <= 'Z')) {
    pos++;
    goto NextChar;
  }
  if ((dest[pos] >= 'a') && (dest[pos] <= 'z')) {
    pos++;
    goto NextChar;
  }
  // Umlaute
  for (cnt=0; cnt<NUMBER_OF_UML; cnt++) {
    if ((dest[pos] == Uml[cnt][0]) && (dest[pos+1] == Uml[cnt][1])) {
      pos+=2;
      goto NextChar;
    }
  }
  // Da war wohl ein verbotenes Zeichen
  dest[0]='\0';
  return STAT_ERR_NO_WORD;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// String nach ASCII Großbuchstaben wandeln, Nur ASCII Buchstaben und deutsche Umlaute werden ersetzt
void str_to_ascii_upper (char *s)
{
  int     cnt;
  size_t  pos=0;
  
NextChar:
  if (s[pos] == '\0') return;
  if ((s[pos] >= 'a') && (s[pos] <= 'z')) {
    s[pos] -= 32;
    pos++;
    goto NextChar;
  }
  // Umlaute
  for (cnt=0; cnt<NUMBER_OF_UML; cnt++) {
    if ((s[pos] == Uml[cnt][0]) && (s[pos+1] == Uml[cnt][1])) {
      s[pos] = Rep[cnt][0];
      s[pos+1] = Rep[cnt][1];
      pos+=2;
      goto NextChar;
    }
  }
  pos++;
  goto NextChar;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Anzahl deutscher Buchstaben im String bestimmen... A-Z, a-z, ÄÖÜäöüß
size_t str_count_german_letters (const char *s)
{
  size_t  pos=0;
  size_t  retval=0;
  size_t  cnt;
  
NextChar:
  if (s[pos] == '\0') return retval;
  // normale Buchstaben
  if ((s[pos] >= 'A') && (s[pos] <= 'Z')) {
     retval++;
     pos++;
     goto NextChar;
  }
  if ((s[pos] >= 'a') && (s[pos] <= 'z')) {
    retval++;
    pos++;
    goto NextChar;
  }
  // Umlaute
  for (cnt=0; cnt<=6; cnt++) {
    if ((s[pos] == Uml[cnt][0]) && (s[pos+1] == Uml[cnt][1])) {
      retval++;
      pos+=2;
      goto NextChar;
    }
  }
  // Weder noch
  pos++;
  goto NextChar;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Zeichen die übergebenem "c" entsprechen aus String entfernen
void str_del_chars (char *s, const char c)
{
  size_t  pos_read=0;
  size_t  pos_write=0;

  // Sicherheitshalber (wäre blöd das zu löschen)
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
// Alle mehrfach hintereinander auftretenden Zeichen (Typ char) im String reduzieren auf eins
void str_del_multiple_chars (char *s)
{
  size_t  pos_read=0;
  size_t  pos_write=0;
  char    last_char='\0';

  while (1) {
    if (s[pos_read] == '\0') {
      s[pos_write] = '\0';
      break;
    }
    else if (s[pos_read] == last_char) pos_read++;
    else {
      last_char = s[pos_read];
      s[pos_write] = last_char;
      pos_read++;
      pos_write++;
    }
  }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Stringreplace Funktion
// Ersetzt in "s" alle "from" durch "to"... Suche und Ersatz müssen gleich groß sein
void str_replace_same_size (char *s, const char *from, const char *to)
{
  char        *pos;
  const char  *cnt;
      
  // Sicherheitshalber
  if (strlen(from) != strlen(to)) return;
    
  while (1) {
    cnt = to;
    pos = strstr(s, from);
    if (pos == NULL) break;
    while (*cnt != '\0') {
      *pos = *cnt;
      pos++;
      cnt++;
    }
  }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ersetzt in "s" alle Nacheinander auftretenden "from" durch ein einziges "to"
void str_replace_group_with_one (char *s, const char from, const char to)
{
  size_t  matchpos, matchlen;
  size_t  readpos, writepos;
  
  while (find_group(s, from, &matchpos, &matchlen) != false) {
    s[matchpos] = to;
    readpos = matchpos + matchlen;
    writepos = matchpos + 1;
    while (s[readpos] != '\0') {
      s[writepos] = s[readpos];
      writepos++;
      readpos++;
    }
    s[writepos] = '\0';
  }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
