/*
phosude comp.c
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
#include "phonetics.h"
#include "string.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Private Funktionen
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Phonetische Übereinstimmung für einen Namen aus Liste mit Wort finden
// Gibt true bei phon Übereinstimmung zurück, sonst false
static bool comp_name_word (const name_t *name, const word_t word, const phops_t *phops)
{
  int       lev, cnt, len;
  phcode_t  code;
  word_t    word_upper;
  bool      textmode=true;

  // Falls Optionen zur Mindest- Höchstlänge gesezt
  // Wird als erstes die Wortlänge geprüft
  if ((phops->min != 0) || (phops->max != 0)) {
    len = (int)str_count_german_letters (word);
    if (phops->min != 0) {
      if (len < phops->min) return false;
    }
    if (phops->max != 0) {
      if (len > phops->max) return false;
    }
  }

  // Phonetische Suche oder nur Textsuche ?
  for (cnt = 0; cnt < PH_NUMBER_OF_CODES; cnt++) {
    if (phops->show[cnt] == true) {
      textmode = false;
      break;
    }
  }
  
  // Wenn keine phonetischen Optionen gewählt nur Textvergleich
  // Groß- Kleinschreibung ignorieren ggf. mit Lev-Dist
  if (textmode == true) {
    strcpy(word_upper, word);
    str_to_ascii_upper(word_upper);
    if (phops->lev == 0) {
      if (strcmp(word_upper, name->name_upper) == 0) return true;
      else return false;
    }
    else {
      lev = str_lev(name->name_upper, word_upper);
      if (lev <= phops->lev) return true;
      else return false;
    }
  }

  // Phon Opt gewählt, also Wort in phone Code wandeln
  // mit jeweiligen phon. Code des Namens vergleichen
  // ggf. Lev-Dist auf erzeugten Code anwenden
  for (cnt = 0; cnt < PH_NUMBER_OF_CODES; cnt++) {
    if (phops->show[cnt] == true) {
      phonetics_get_code (word, code, cnt);
      if (phops->lev != 0) {
        lev = str_lev(code, name->code[cnt]);
        if (lev <= phops->lev) return true;
      }
      else {
        if (strcmp (code, name->code[cnt]) == 0) return true;
      }
    }
  }
  
  // Keine phon. Übereinstimmung gefunden
  return false;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Vergleicht komplette Namensliste nach phonetischen Optionen mit einem Wort
// Rückgabe Pointer auf passenden Namen in Liste oder Pointer auf NULL
name_t *comp_nameslist_word (const nameslist_t *list, const word_t word, const phops_t *phops)
{
  int    cnt;
  name_t *name = list->items;

  // Wort gegen Minusnamen prüfen, Ende bei EXAKTER Übereinstimmung
  // Natürlich mit NULL (Kein Fund), denn genau DIESE Schreibweise soll ja NICHT ausgegeben werden
  for (cnt = 0; cnt < list->number_of_names; cnt++) {
    if (list->items[cnt].exclude == true) {
      if (strcmp(list->items[cnt].name_norm, word) == 0) return NULL;
    }
  }

  // Wort gegen normale Namen prüfen
  for (cnt = 0; cnt < list->number_of_names; cnt++) {
    if (list->items[cnt].exclude == false) {
      if (comp_name_word (name, word, phops) == true) return name;
    }
    name++;
  }

  // Liste ohne Funde durch
  return NULL;
}
