/*
Erzeugen von phonetischen Codes

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
#include "typedefs.h"
#include "string.h"


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Soundex Verfahren
void phoneconvert_soundex (const word_t *src, code_t *dest)
{
  char    first;
  size_t  size;
  size_t  pos=0;
  
  // Wir arbeiten gleich auf den Ausgabestring
  strcpy(dest->s, src->s);
  if (str_to_ascii_upper_word(dest->s) == false) return;
    
  // 1. Zeichen aus dest merken und in dest mit '-' ausmaskieren
  first=dest->s[0];
  dest->s[0]='-';

  // Ersetzungsregeln auf dest anwenden
  while (dest->s[pos] != '\0') {
    switch (dest->s[pos]) {
      case 'A': case 'E': case 'H': case 'I': case 'O': case 'U': case 'W': case 'Y':
        dest->s[pos] = '0';
        break;
      case 'B': case 'F': case 'P': case 'V':
        dest->s[pos] = '1';
        break;
      case 'C': case 'G': case 'J': case 'K': case 'Q': case 'S': case 'X': case 'Z':
        dest->s[pos] = '2';
        break;
      case 'D': case 'T':
        dest->s[pos] = '3';
        break;
      case 'L':
        dest->s[pos] = '4';
        break;
      case 'M': case 'N':
        dest->s[pos] = '5';
        break;
      case 'R':
        dest->s[pos] = '6';
        break;
    }
    pos++;
  }

  // Doppelte entfernen, dann 0en entfernen
  str_del_multiple_ascii_chars(dest->s);
  str_del_ascii_chars(dest->s, '0');

  // gemerktes 1. Zeichen wieder nach dest schieben... Alles auf 4 Stellen kürzen/0en anhängen
  dest->s[0]=first;
  size=strlen(dest->s);
  if (size == 1) strcat (dest->s, "000");
  else if (size == 2) strcat (dest->s, "00");
  else if (size == 3) strcat (dest->s, "0");
  else if (size > 4) dest->s[4]='\0';
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Extended Soundex Verfahren
void phoneconvert_exsoundex (const word_t *src, code_t *dest)
{
  size_t  size;
  size_t  pos=0;

  // Wir arbeiten gleich auf den Ausgabestring
  strcpy(dest->s, src->s);
  if (str_to_ascii_upper_word(dest->s) == false) return;
  
  // Ersetzungsregeln auf dest anwenden
  while (dest->s[pos] != '\0') {
    switch (dest->s[pos]) {
      case 'A': case 'E': case 'H': case 'I': case 'O': case 'U': case 'W': case 'Y':
        dest->s[pos] = '0';
        break;
      case 'B': case 'P':
        dest->s[pos] = '1';
        break;
      case 'V': case 'F':
        dest->s[pos] = '2';
        break;
      case 'C': case 'K': case 'S':
        dest->s[pos] = '3';
        break;
      case 'G': case 'J':
        dest->s[pos] = '4';
        break;
      case 'Q': case 'X': case 'Z':
        dest->s[pos] = '5';
        break;
      case 'D': case 'T':
        dest->s[pos] = '6';
        break;
      case 'L':
        dest->s[pos] = '7';
        break;
      case 'M': case 'N':
        dest->s[pos] = '8';
        break;
      case 'R':
        dest->s[pos] = '9';
        break;
    }
    pos++;
  }

  // Doppelte entfernen, dann 0en entfernen
  str_del_multiple_ascii_chars(dest->s);
  str_del_ascii_chars(dest->s, '0');

  // Auf 5 Stellen kürzen / mit "0" auffüllen
  size=strlen(dest->s);
  if (size == 0) strcat (dest->s, "00000");
  else if (size == 1) strcat (dest->s, "0000");
  else if (size == 2) strcat (dest->s, "000");
  else if (size == 3) strcat (dest->s, "00");
  else if (size == 4) strcat (dest->s, "0");
  else dest->s[5]='\0';
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Kölner Phonetik Verfahren
void phoneconvert_cologne (const word_t *src, code_t *dest)
{
  char    scode1[(BUFFSIZE_CODE + 4)];    // Zwischenbuffer
  char    scode2[(BUFFSIZE_CODE + 4)];    // Etwas Größer wegen Erweiterung
  char    first;
  char    group[3];
  size_t  size;
  size_t  pos;

  // Wegen lesen in 3er Gruppen kopieren wir scode1 nach scode2
  // und erweitern den übergebenen String hinten und vorn um ein Leerzeichen
  strcpy(scode1, src->s);
  if (str_to_ascii_upper_word(scode1) == false) {
    dest->s[0] = '\0';
    return;
  }
  scode2[0]=' ';
  scode2[1]='\0';
  strcat(scode2, scode1);
  strcat(scode2, " ");

  // Wort wird in Gruppen von 3 Buchstaben gelesen, da es Regeln für "NACH" und "VOR" Buchstabe gibt
  // Der Referenzbuchstabe ist immer der in der Mitte der 3er Gruppe
  // Lesen von scode2, und nach Regeln konvertiert in scode1
  size = strlen(scode2);
  scode1[0]='\0';
  for (pos=1; pos<=size - 2; pos++) {
    // In 3er Gruppen lesen
    group[0]=scode2[pos-1];
    group[1]=scode2[pos];
    group[2]=scode2[pos+1];

    // Sonderfälle Wortanfang
    if (pos==1) {
      if (group[1] == 'C') {
        if (group[2] == 'A' || group[2] == 'H' || group[2] == 'K' ||
          group[2] == 'L' || group[2] == 'O' || group[2] == 'Q' ||
          group[2] == 'R' || group[2] == 'U' || group[2] == 'X') {
            strcat(scode1, "4");
            continue;
        }
        else {
          strcat(scode1, "8");
          continue;
        }
      }
    }

    // Normale codierung KEIN Wortanfang
    if (group[1] == 'P' && group[2] == 'H') {
      strcat (scode1, "3");
      continue;
    }
    if ((group[1] == 'D' && group[2] == 'S') || (group[1] == 'D' && group[2] == 'C') ||
      (group[1] == 'D' && group[2] == 'Z') || (group[1] == 'T' && group[2] == 'S') ||
      (group[1] == 'T' && group[2] == 'C') || (group[1] == 'T' && group[2] == 'Z')) {
        strcat(scode1, "8");
        continue;
    }
    if (group[1] == 'D' || group[1] == 'T') {
      strcat(scode1, "2");
      continue;
    }
    if ((group[0] == 'C' && group[1] == 'X') || (group[0] == 'K' && group[1] == 'X') ||
      (group[0] == 'Q' && group[1] == 'X')) {
        strcat(scode1, "8");
        continue;
    }
    if (group[1] == 'X') {
      strcat(scode1, "48");
      continue;
    }
    if ((group[0] == 'S' && group[1] == 'C') || (group[0] == 'S' && group[1] == 'Z')) {
      strcat(scode1, "8");
      continue;
    }
    if (group[1] == 'C') {
      if (group[2] == 'A' || group[2] == 'O' || group[2] == 'U' || group[2] == 'H' ||
        group[2] == 'K' || group[2] == 'X' || group[2] == 'Q') {
          strcat(scode1, "4");
          continue;
      }
      else {
        strcat(scode1, "8");
        continue;
      }
    }
    if (group[1] == 'H') {
      strcat(scode1, "H");
      continue;
    }
    if (group[1] == 'A' || group[1] == 'E' || group[1] == 'I' || group[1] == 'J' ||
      group[1] == 'Y' || group[1] == 'O' || group[1] == 'U') {
        strcat(scode1, "0");
        continue;
    }
    if (group[1] == 'B' || group[1] == 'P') {
      strcat(scode1, "1");
      continue;
    }
    if (group[1] == 'F' || group[1] == 'V' || group[1] == 'W') {
      strcat(scode1, "3");
      continue;
    }
    if (group[1] == 'G' || group[1] == 'K' || group[1] == 'Q') {
      strcat(scode1, "3");
      continue;
    }
    if (group[1] == 'L') {
      strcat(scode1, "5");
      continue;
    }
    if (group[1] == 'R') {
      strcat(scode1, "7");
      continue;
    }
    if (group[1] == 'M' || group[1] == 'N') {
      strcat(scode1, "6");
      continue;
    }
    if (group[1] == 'S' || group[1] == 'Z') {
      strcat(scode1, "8");
      continue;
    }
  }

  // doppelte entfernen, H entfernen
  str_del_multiple_ascii_chars(scode1);
  str_del_ascii_chars(scode1, 'H');

  // 0 Außer am Anfang entfernen
  first=scode1[0];
  scode1[0]='-';
  str_del_ascii_chars(scode1, '0');
  scode1[0]=first;

  strcpy(dest->s, scode1);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Phonem Verfahren
// Phonem liefert nicht für jedes Wort einen gültigen Code
// z.B. das Wort Ahe
// In solchen fällen wird im Code "---" zurückgegeben
void phoneconvert_phonem (const word_t *src, code_t *dest)
{
  char    scode1[BUFFSIZE_CODE];
  char    scode2[BUFFSIZE_CODE];
  char    tmp[5];
  size_t  pos=0;

  // wir abrbeiten mit Buffer
  strcpy(scode1, src->s);
  if (str_to_ascii_upper_word(scode1) == false) {
    dest->s[0] = '\0';
    return;
  }
    
  // 1. Durchgang = Buchstabenpaare kopieren/nach Regeln codieren, Quelle:scode1, Ziel: scode2
  //                Da es mit dem ö zu Problemen kommt, wird es vorerst als 0 kodiert
  scode2[0]='\0';
  while (scode1[pos] != '\0') {
    // AE
    if (scode1[pos] == 'A' && scode1[pos+1] == 'E') {
      strcat(scode2, "E");
      pos+=2;
    }
    // PF
    else if (scode1[pos] == 'P' && scode1[pos+1] == 'F') {
      strcat(scode2, "V");
      pos+=2;
    }
    // KS
    else if (scode1[pos] == 'K' && scode1[pos+1] == 'S') {
      strcat(scode2, "X");
      pos+=2;
    }
    // QU
    else if (scode1[pos] == 'Q' && scode1[pos+1] == 'U') {
      strcat(scode2, "KW");
      pos+=2;
    }
    // OE muß eigentlich zu Ö werden, was aber Doppelbytezeichen wäre
    // Das ließe sich aber mit Routine zum entfernen von Doppelten Zeichen nicht
    // Fehlerfrei abarbeiten, daher machen wir einfach erst mal eine 0 draus (Einybytezeichen)
    else if (scode1[pos] == 'O' && scode1[pos+1] == 'E') {
      strcat(scode2, "0");
      pos+=2;
    }
    // EU
    else if (scode1[pos] == 'E' && scode1[pos+1] == 'U') {
      strcat(scode2, "OY");
      pos+=2;
    }
    // OU
    else if (scode1[pos] == 'O' && scode1[pos+1] == 'U') {
      strcat(scode2, "U");
      pos+=2;
    }
    // EI,EY
    else if ((scode1[pos] == 'E' && scode1[pos+1] == 'I') || (scode1[pos] == 'E' && scode1[pos+1] == 'Y')) {
      strcat(scode2, "AY");
      pos+=2;
    }
    // SC,SZ,CZ,TZ,TS
    else if ((scode1[pos] == 'S' && scode1[pos+1] == 'C') ||
      (scode1[pos] == 'S' && scode1[pos+1] == 'Z') ||
      (scode1[pos] == 'C' && scode1[pos+1] == 'Z') ||
      (scode1[pos] == 'T' && scode1[pos+1] == 'Z') ||
      (scode1[pos] == 'T' && scode1[pos+1] == 'S')) {
        strcat(scode2, "C");
        pos+=2;
    }
    else {
      // Wenn Buchstabenpaar nicht nach Regeln kodiert werden konnte
      // nur einen Buchstaben kopieren und weiter ab dem nächsten mit Paarsuche
      tmp[0]=scode1[pos];
      tmp[1]='\0';
      strcat (scode2, tmp);
      pos++;
    }
  }

  // 2. Durchgang einzelne Buchstaben ersetzten/kodieren in scode2
  pos=0;
  while (scode2[pos] != '\0') {
    switch (scode2[pos]) {
      case 'Z': case 'K': case 'G': case 'Q':
        scode2[pos] = 'C';
        break;
      case 'U': case 'I': case 'J':
        scode2[pos] = 'Y';
        break;
      case 'F': case 'W':
        scode2[pos] = 'V';
        break;
      case 'A':
        scode2[pos] = 'E';
        break;
      case 'P':
        scode2[pos] = 'B';
        break;
      case 'T':
        scode2[pos] = 'D';
        break;
    }
    pos++;
  }

  // 3. Durchgang alle doppelten Zeichen entfernen in scode2
  // Da das Ö als 0 codiert ist, bekommen wir doppelte davon einfach weg
  str_del_multiple_ascii_chars(scode2);

  // 4. Durchgang alles von scode2 nach scode1 kopieren, unerlaubte Zeichen dabei außlassen
  // Jetzt erst aus 0 das endgültige Ö machen
  pos=0;
  scode1[0]='\0';
  while (scode2[pos] != '\0') {
    if (scode2[pos] == 'A' || scode2[pos] == 'B' || scode2[pos] == 'C' || scode2[pos] == 'D' || scode2[pos] == 'L' ||
      scode2[pos] == 'M' || scode2[pos] == 'N' || scode2[pos] == 'O' || scode2[pos] == 'R' || scode2[pos] == 'S' ||
      scode2[pos] == 'U' || scode2[pos] == 'V' || scode2[pos] == 'W' || scode2[pos] == 'X' || scode2[pos] == 'Y') {
        tmp[0] = scode2[pos];
        tmp[1] = '\0';
        strcat (scode1, tmp);
    }
    else if (scode2[pos] == '0') strcat (scode1, "Ö");
    pos++;
  }

  // Falls Wort keinen gültigen Code erzeugt... "---" zurückgeben, sonst Code
  if (scode1[0] == '\0') strcpy(dest->s, "---");
  else strcpy(dest->s, scode1);
}
