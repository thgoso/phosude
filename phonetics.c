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
  if (strlen(dest) < 4) strcat(dest, "0000");
  dest[4] = '\0';
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Extended Soundex Verfahren
// Wenn Wort in dest Platz hat, paßt der Code auch da hinein
static void make_exsoundex (phcode_t dest)
{
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
  if (strlen(dest) < 5) strcat (dest, "00000");
  dest[5]='\0';
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
// Caverphone v2.0
static void make_caverphone (phcode_t dest)
{
  size_t  len;
  char    first;
  
  // REMOVE FINAL E
  len = strlen(dest);
  if (dest[len-1] == 'E') dest[len-1] = '\0';
  // IF THE NAME STARTS WITH COUGH MAKE IT COU2F
  // IF THE NAME STARTS WITH ROUGH MAKE IT ROU2F
  // IF THE NAME STARTS WITH TOUGH MAKE IT TOU2F
  if ((strncmp(dest, "COUGH", 5)== 0) || (strncmp(dest, "ROUGH", 5)== 0) || (strncmp(dest, "TOUGH", 5)== 0)) {
    dest[3] = '2';
  }
  // IF THE NAME STARTS WITH ENOUGH MAKE IT ENOU2F
  // IF THE NAME STARTS WITH TROUGH MAKE IT TROU2F
  if ((strncmp(dest, "ENOUGH", 6)== 0) || (strncmp(dest, "TROUGH", 6)== 0)) dest[4] = '2';
  // IF THE NAME STARTS WITH GN MAKE IT 2N
  if (strncmp(dest, "GN", 2)== 0) dest[0] = '2';
  // IF THE NAME ENDS WITH MB MAKE IT M2
  len = strlen(dest);
  if ((dest[len-2] == 'M') && (dest[len-1] == 'B')) dest[len-1] = '2';
  str_replace_same_size (dest, "CQ", "2Q");     // REPLACE CQ WITH 2Q
  str_replace_same_size (dest, "CI", "SI");     // REPLACE CI WITH SI
  str_replace_same_size (dest, "CE", "SE");     // REPLACE CE WITH SE
  str_replace_same_size (dest, "CY", "SY");     // REPLACE CY WITH SY
  str_replace_same_size (dest, "TCH", "2CH");   // REPLACE TCH WITH 2CH
  str_replace_same_size (dest, "C", "K");       // REPLACE C WITH K
  str_replace_same_size (dest, "Q", "K");       // REPLACE Q WITH K
  str_replace_same_size (dest, "X", "K");       // REPLACE X WITH K
  str_replace_same_size (dest, "V", "F");       // REPLACE V WITH F
  str_replace_same_size (dest, "DG", "2G");     // REPLACE DG WITH 2G
  str_replace_same_size (dest, "TIO", "SIO");   // REPLACE TIO WITH SIO
  str_replace_same_size (dest, "TIA", "SIA");   // REPLACE TIA WITH SIA
  str_replace_same_size (dest, "D", "T");       // REPLACE D WITH T
  str_replace_same_size (dest, "PH", "FH");     // REPLACE PH WITH FH
  str_replace_same_size (dest, "B", "P");       // REPLACE B WITH P
  str_replace_same_size (dest, "SH", "S2");     // REPLACE SH WITH S2
  str_replace_same_size (dest, "Z", "S");       // REPLACE Z WITH S
  // REPLACE AN INITIAL VOWEL WITH AN a 
  // REPLACE ALL OTHER VOWELS WITH A 3
  first = dest[0];
  if ((first == 'A') || (first == 'E') || (first == 'I') || (first == 'O') || (first == 'U')) first = 'a';
  str_replace_same_size (dest, "A", "3");
  str_replace_same_size (dest, "E", "3");
  str_replace_same_size (dest, "I", "3");
  str_replace_same_size (dest, "O", "3");
  str_replace_same_size (dest, "U", "3");
  dest[0] = first;
  str_replace_same_size (dest, "J", "Y");       // REPLACE J WITH Y
  if (strncmp(dest, "Y3", 2) ==0) dest[0]='y';  // REPLACE AN INITIAL Y3 WITH y3
  if (dest[0] == 'Y') dest[0] = 'a';            // REPLACE AN INITIAL Y WITH a
  str_replace_same_size (dest, "Y", "3");       // REPLACE Y WITH 3
  str_replace_same_size (dest, "3GH3", "3KH3"); // REPLACE 3GH3 WITH 3KH3
  str_replace_same_size (dest, "GH", "22");     // REPLACE GH WITH 22
  str_replace_same_size (dest, "G", "K");       // REPLACE G WITH K
  str_replace_group_with_one (dest, 'S', 's');  // REPLACE GROUPS OF THE LETTER S WITH A s
  str_replace_group_with_one (dest, 'T', 't');  // REPLACE GROUPS OF THE LETTER T WITH A t
  str_replace_group_with_one (dest, 'P', 'p');  // REPLACE GROUPS OF THE LETTER P WITH A p
  str_replace_group_with_one (dest, 'K', 'k');  // REPLACE GROUPS OF THE LETTER K WITH A k
  str_replace_group_with_one (dest, 'F', 'f');  // REPLACE GROUPS OF THE LETTER F WITH A f
  str_replace_group_with_one (dest, 'M', 'm');  // REPLACE GROUPS OF THE LETTER M WITH A m
  str_replace_group_with_one (dest, 'N', 'n');  // REPLACE GROUPS OF THE LETTER N WITH A n
  str_replace_same_size (dest, "W3", "w3");     // REPLACE W3 WITH w3
  str_replace_same_size (dest, "WH3", "wH3");   // REPLACE WH3 WITH wH3
  len = strlen(dest);                           // IF THE NAME ENDS IN W REPLACE THE FINAL W WITH 3
  if (dest[len-1] == 'W') dest[len-1] = '3';
  str_replace_same_size (dest, "W", "2");       // REPLACE W WITH 2
  if (dest[0] == 'H') dest[0] = 'a';            // REPLACE AN INITIAL H WITH AN a
  str_replace_same_size (dest, "H", "2");       // REPLACE ALL OTHER OCCURRENCES OF H WITH A 2
  str_replace_same_size (dest, "R3", "r3");     // REPLACE R3 WITH r3
  len = strlen(dest);                           // IF THE NAME ENDS IN R REPLACE THE REPLACE FINAL R WITH 3
  if (dest[len-1] == 'R') dest[len-1] = '3';
  str_replace_same_size (dest, "R", "2");       // REPLACE R WITH 2
  str_replace_same_size (dest, "L3", "l3");     // REPLACE L3 WITH l3
  len = strlen(dest);                           // IF THE NAME ENDS IN L REPLACE THE REPLACE FINAL L WITH 3
  if (dest[len-1] == 'L') dest[len-1] = '3';
  str_replace_same_size (dest, "L", "2");       // REPLACE L WITH 2
  str_del_chars (dest, '2');                    // REMOVE ALL 2S
  len = strlen(dest);                           // IF THE NAME END IN 3, REPLACE THE FINAL 3 WITH a
  if (dest[len-1] == '3') dest[len-1] = 'a';
  str_del_chars (dest, '3');                    // REMOVE ALL 3S
  // PUT TEN 1S ON THE END
  // TAKE THE FIRST TEN CHARACTERS AS THE CODE
  if (strlen(dest) < 10) strcat (dest, "1111111111");
  dest[10] = '\0';
  str_to_ascii_upper (dest);
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
  else if (code_no == PH_CAVERPHONE) make_caverphone (dest);
  
  // Fehlerhafte Codenummer
  else dest[0] = '\0';
}
