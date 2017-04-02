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


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Private Funktion
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Prüft ob übergebenes Wort nach übergebenen phonetischen Optionen phonetisch gleich mit einem Name ist
// Rückgabe:  true bei Übereinstimmung
//            false wenn keine
static bool compare_word_name (const name_t *name, const phops_t *phops, const char *word)
{
  int   lvst_dist;
  char  code[BUFFER_SIZE_WORD];
  char  word_lower[BUFFER_SIZE_WORD];

  // Wenn keine phonetischen Optionen gewählt
  // Nur Lev Dist auf Name & Wort anwenden
  // Groß, Kleinschreibung ignorieren
  if (phops->k == false && phops->p == false && phops->s == false && phops->e == false) {
    strcpy(word_lower, word);
    to_lower(word_lower);
    lvst_dist = levenshtein(name->name_lower, word_lower);
    if (lvst_dist <= phops->l) return true;
    else return false;
  }

  // Phon Opt gewählt, also Wort in phone Code wandeln
  // mit jeweiligen phon. Code des Namens vergleichen
  if (phops->k == true) {
    phoneconvert_cologne(word, code);
    if (phops->l != 0) {
      lvst_dist = levenshtein (code, name->code_k);
      if (lvst_dist <= phops->l) return true;
    }
    else {
      if (strcmp (code, name->code_k) == 0) return true;
    }
  }

  if (phops->p == true) {
    phoneconvert_phonem(word, code);
    if (phops->l != 0) {
      lvst_dist = levenshtein (code, name->code_p);
      if (lvst_dist <= phops->l) return true;
    }
    else {
      if (strcmp (code, name->code_p) == 0) return true;
    }
  }

  if (phops->s == true) {
    phoneconvert_soundex(word, code);
    if (phops->l != 0) {
      lvst_dist = levenshtein (code, name->code_s);
      if (lvst_dist <= phops->l) return true;
    }
    else {
      if (strcmp (code, name->code_s) == 0) return true;
    }
  }

  if (phops->e == true) {
    phoneconvert_exsoundex(word, code);
    if (phops->l != 0) {
      lvst_dist = levenshtein (code, name->code_e);
      if (lvst_dist <= phops->l) return true;
    }
    else {
      if (strcmp (code, name->code_e) == 0) return true;
    }
  }

  // Keine phon. Übereinstimmung gefunden
  return false;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Öffentliche Funktion
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Prüft die ganze Namensliste auf phonetische Übereinstimmung mit übergebenem Wort
// Rückgabe: Pointer auf zugehörigen Namen in list wenn eine phonetische Übereinstimmung gefunden wurde
//           Pointer auf NULL wenn keine phonetische Übereinstimmung gefunden wurde
name_t *compare_word_nameslist (const nameslist_t *list, const phops_t *phops, const char *word)
{
  int    cnt;
  name_t *name;

  // Wort gegen Minusnamen prüfen, Ende bei EXAKTER Übereinstimmung
  // Natürlich mit NULL (Kein Fund), denn genau DIESE Schreibweise soll ja NICHT ausgegeben werden
  for (cnt = 0; cnt < list->number_of_names; cnt++) {
    if (list->items[cnt].is_minusname == true) {
      if (strcmp(list->items[cnt].name_norm, word) == 0) return NULL;
    }
  }

  // name zeigt auf Anfang der Liste
  name = list->items;
  // Wort gegen normale Namen prüfen, Rückgabe mit Index füllen
  for (cnt = 0; cnt < list->number_of_names; cnt++) {
    // Ab 1 Auf nächsten Namen
    if (cnt > 0) name++;
    if (list->items[cnt].is_minusname == false) {
      if (compare_word_name (name, phops, word) == true) return name;
    }
  }

  // Liste ohne Funde durch
  return NULL;
}
