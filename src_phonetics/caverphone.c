/*
phosude caverphone.c
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
#include "caverphone.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void make_caverphone (const word_t src, caverphone_t dest)
{
  word_t  tmp;
  size_t  len;
  char    first;
  
  // src nach tmp kopieren, in ASCII Groß wandeln
  strcpy(tmp, src);
  str_to_ascii_upper(tmp);
    
  // REMOVE FINAL E
  len = strlen(tmp);
  if (tmp[len-1] == 'E') tmp[len-1] = '\0';
  // IF THE NAME STARTS WITH COUGH MAKE IT COU2F
  // IF THE NAME STARTS WITH ROUGH MAKE IT ROU2F
  // IF THE NAME STARTS WITH TOUGH MAKE IT TOU2F
  if ((strncmp(tmp, "COUGH", 5)== 0) || (strncmp(tmp, "ROUGH", 5)== 0) || (strncmp(tmp, "TOUGH", 5)== 0)) {
    tmp[3] = '2';
  }
  // IF THE NAME STARTS WITH ENOUGH MAKE IT ENOU2F
  // IF THE NAME STARTS WITH TROUGH MAKE IT TROU2F
  if ((strncmp(tmp, "ENOUGH", 6)== 0) || (strncmp(tmp, "TROUGH", 6)== 0)) tmp[4] = '2';
  // IF THE NAME STARTS WITH GN MAKE IT 2N
  if (strncmp(tmp, "GN", 2)== 0) tmp[0] = '2';
  // IF THE NAME ENDS WITH MB MAKE IT M2
  len = strlen(tmp);
  if (len >= 2) {
    if ((tmp[len-2] == 'M') && (tmp[len-1] == 'B')) tmp[len-1] = '2';
  }
  str_replace_same_size (tmp, "CQ", "2Q");      // REPLACE CQ WITH 2Q
  str_replace_same_size (tmp, "CI", "SI");      // REPLACE CI WITH SI
  str_replace_same_size (tmp, "CE", "SE");      // REPLACE CE WITH SE
  str_replace_same_size (tmp, "CY", "SY");      // REPLACE CY WITH SY
  str_replace_same_size (tmp, "TCH", "2CH");    // REPLACE TCH WITH 2CH
  str_replace_same_size (tmp, "C", "K");        // REPLACE C WITH K
  str_replace_same_size (tmp, "Q", "K");        // REPLACE Q WITH K
  str_replace_same_size (tmp, "X", "K");        // REPLACE X WITH K
  str_replace_same_size (tmp, "V", "F");        // REPLACE V WITH F
  str_replace_same_size (tmp, "DG", "2G");      // REPLACE DG WITH 2G
  str_replace_same_size (tmp, "TIO", "SIO");    // REPLACE TIO WITH SIO
  str_replace_same_size (tmp, "TIA", "SIA");    // REPLACE TIA WITH SIA
  str_replace_same_size (tmp, "D", "T");        // REPLACE D WITH T
  str_replace_same_size (tmp, "PH", "FH");      // REPLACE PH WITH FH
  str_replace_same_size (tmp, "B", "P");        // REPLACE B WITH P
  str_replace_same_size (tmp, "SH", "S2");      // REPLACE SH WITH S2
  str_replace_same_size (tmp, "Z", "S");        // REPLACE Z WITH S
  // REPLACE AN INITIAL VOWEL WITH AN a 
  // REPLACE ALL OTHER VOWELS WITH A 3
  first = tmp[0];
  if ((first == 'A') || (first == 'E') || (first == 'I') || (first == 'O') || (first == 'U')) first = 'a';
  str_replace_same_size (tmp, "A", "3");
  str_replace_same_size (tmp, "E", "3");
  str_replace_same_size (tmp, "I", "3");
  str_replace_same_size (tmp, "O", "3");
  str_replace_same_size (tmp, "U", "3");
  tmp[0] = first;
  str_replace_same_size (tmp, "J", "Y");        // REPLACE J WITH Y
  if (strncmp(tmp, "Y3", 2) ==0) tmp[0]='y';    // REPLACE AN INITIAL Y3 WITH y3
  if (tmp[0] == 'Y') tmp[0] = 'a';              // REPLACE AN INITIAL Y WITH a
  str_replace_same_size (tmp, "Y", "3");        // REPLACE Y WITH 3
  str_replace_same_size (tmp, "3GH3", "3KH3");  // REPLACE 3GH3 WITH 3KH3
  str_replace_same_size (tmp, "GH", "22");      // REPLACE GH WITH 22
  str_replace_same_size (tmp, "G", "K");        // REPLACE G WITH K
  str_replace_group_with_one (tmp, 'S', 's');   // REPLACE GROUPS OF THE LETTER S WITH A s
  str_replace_group_with_one (tmp, 'T', 't');   // REPLACE GROUPS OF THE LETTER T WITH A t
  str_replace_group_with_one (tmp, 'P', 'p');   // REPLACE GROUPS OF THE LETTER P WITH A p
  str_replace_group_with_one (tmp, 'K', 'k');   // REPLACE GROUPS OF THE LETTER K WITH A k
  str_replace_group_with_one (tmp, 'F', 'f');   // REPLACE GROUPS OF THE LETTER F WITH A f
  str_replace_group_with_one (tmp, 'M', 'm');   // REPLACE GROUPS OF THE LETTER M WITH A m
  str_replace_group_with_one (tmp, 'N', 'n');   // REPLACE GROUPS OF THE LETTER N WITH A n
  str_replace_same_size (tmp, "W3", "w3");      // REPLACE W3 WITH w3
  str_replace_same_size (tmp, "WH3", "wH3");    // REPLACE WH3 WITH wH3
  len = strlen(tmp);                            // IF THE NAME ENDS IN W REPLACE THE FINAL W WITH 3
  if (tmp[len-1] == 'W') tmp[len-1] = '3';
  str_replace_same_size (tmp, "W", "2");        // REPLACE W WITH 2
  if (tmp[0] == 'H') tmp[0] = 'a';              // REPLACE AN INITIAL H WITH AN a
  str_replace_same_size (tmp, "H", "2");        // REPLACE ALL OTHER OCCURRENCES OF H WITH A 2
  str_replace_same_size (tmp, "R3", "r3");      // REPLACE R3 WITH r3
  len = strlen(tmp);                            // IF THE NAME ENDS IN R REPLACE THE REPLACE FINAL R WITH 3
  if (tmp[len-1] == 'R') tmp[len-1] = '3';
  str_replace_same_size (tmp, "R", "2");        // REPLACE R WITH 2
  str_replace_same_size (tmp, "L3", "l3");      // REPLACE L3 WITH l3
  len = strlen(tmp);                            // IF THE NAME ENDS IN L REPLACE THE REPLACE FINAL L WITH 3
  if (tmp[len-1] == 'L') tmp[len-1] = '3';
  str_replace_same_size (tmp, "L", "2");        // REPLACE L WITH 2
  str_del_chars (tmp, '2');                     // REMOVE ALL 2S
  len = strlen(tmp);                            // IF THE NAME END IN 3, REPLACE THE FINAL 3 WITH a
  if (tmp[len-1] == '3') tmp[len-1] = 'a';
  str_del_chars (tmp, '3');                     // REMOVE ALL 3S
  // PUT TEN 1S ON THE END
  // TAKE THE FIRST TEN CHARACTERS AS THE CODE
  if (strlen(tmp) < 10) strcat (tmp, "1111111111");
  tmp[10] = '\0';
  str_to_ascii_upper (tmp);
  
  // Rückgabe füllen
  strcpy(dest, tmp);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool comp_word_caverphone (const word_t word, const caverphone_t code, int *lev)
{
  caverphone_t  tmp;
  int           dist;
  
  // Vergleichscode erstellen
  make_caverphone (word, tmp);
  
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
