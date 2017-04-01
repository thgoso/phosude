/*
phoneshow_comp

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
#include "buffer_sizes.h"
#include "phoneshow_types.h"
#include "phonetics.h"
#include "phoneshow_str.h"


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Private Funktion
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Prüft ob übergebenes Wort phonetisch gleich mit einem Name aus Liste ist
// Rückgabe:  true bei Übereinstimmung
//            false wenn keine
static bool compare_word_name (const nameslist_t *list, const int idx, const phops_t *phops, const char *word)
{
  int   lvst_dist;
  char  code[BUFFER_SIZE_CODE];
  char  word_lower[BUFFER_SIZE_WORD];

  // Wenn keine phonetischen Optionen gewählt
  // Nur Lev Dist auf Name & Wort anwenden
  // Groß, Kleinschreibung ignorieren
  if (phops->k == false && phops->p == false && phops->s == false && phops->e == false) {
    strcpy(word_lower, word);
    to_lower(word_lower);
    lvst_dist = levenshtein(list->items[idx].name_lower, word_lower);
    if (lvst_dist <= phops->l) return true;
    else return false;
  }

  // Phon Opt gewählt, also Wort in phone Code wandeln
  // mit jeweiligen phon. Code des Namens vergleichen
  if (phops->k == true) {
    phoneconvert_cologne(word, code);
    if (phops->l != 0) {
      lvst_dist = levenshtein (code, list->items[idx].code_k);
      if (lvst_dist <= phops->l) return true;
    }
    else if (strcmp (code, list->items[idx].code_k) == 0) return true;
  }
  
  if (phops->p == true) {
    phoneconvert_phonem(word, code);
    if (phops->l != 0) {
      lvst_dist = levenshtein (code, list->items[idx].code_p);
      if (lvst_dist <= phops->l) return true;
    }
    else if (strcmp (code, list->items[idx].code_p) == 0) return true;
  }

  if (phops->s == true) {
    phoneconvert_soundex(word, code);
    if (phops->l != 0) {
      lvst_dist = levenshtein (code, list->items[idx].code_s);
      if (lvst_dist <= phops->l) return true;
    }
    else if (strcmp (code, list->items[idx].code_s) == 0) return true;
  }

  if (phops->e == true) {
    phoneconvert_exsoundex(word, code);
    if (phops->l != 0) {
      lvst_dist = levenshtein (code, list->items[idx].code_e);
      if (lvst_dist <= phops->l) return true;
    }
    else if (strcmp (code, list->items[idx].code_e) == 0) return true;
  }

  // Keine phon. Übereinstimmung gefunden
  return false;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Öffentliche Funktion
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Rückgabe:  false wenn keine phonetische Übereinstimmung gefunden wurde
//                in "idx" wird -1 zurückgegeben
//            true wenn Übereinstimmung
//                in "idx" wird der Index des zugehörigens Namens aus Namensliste zurückgegeben
bool compare_word_nameslist (const nameslist_t *list, const phops_t *phops, const char *word, int *idx)
{
  int cnt;
    
  // Wort gegen Minusnamen prüfen, Ende bei EXAKTER Übereinstimmung
  // Natürlich mit -1, denn genau DIESE Schreibweise soll ja NICHT ausgegeben werden
  for (cnt = 0; cnt < list->number_of_names; cnt++) {
    if (list->items[cnt].is_minusname == true) {
      if (strcmp(list->items[cnt].name_norm, word) == 0) {
        *idx= -1;
        return false;
      }
    }
  }
  
  // Wort gegen normale Namen prüfen, Rückgabe mit Index füllen
  for (cnt = 0; cnt < list->number_of_names; cnt++) {
    if (list->items[cnt].is_minusname == false) {
      if (compare_word_name(list, cnt, phops, word) == true) {
        *idx=cnt;
        return true;
      }
    }
  }
  
  *idx= -1;
  return false;
}
