/*
phosude textprint.c
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

#include <stdio.h>
#include <string.h>
#include "phosude.h"
#include "phonetics.h"
#include "stdin.h"
#include "string.h"
#include "comp.h"

// Funktionsprototypen private Unterfunktionen
static void write_out_verbose (const nameslist_t *list, const phops_t *phops, const word_t word, const size_t line_no);
static void verbose_word (const nameslist_t *list, const phops_t *phops, const word_t word);
static void show_footer (const nameslist_t *list, const phops_t *phops, const size_t matchcount);

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Öffentliche Ausgaberoutinen
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus a ... gesammten Text anzeigen
int printout_a (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops)
{
  int     word_stat;
  int     line_stat;
  size_t  line_cnt=0;
  size_t  match_cnt=0;
  name_t  *match;
  word_t  word;
  
  while (1) {
    line_stat = stdin_read_line();
    line_cnt++;
    if (line_stat == STAT_ERR_REC_OVERLEN_LINE) return STAT_ERR_REC_OVERLEN_LINE;
    if (line_stat == STAT_END_REACHED) break;
    stdin_carrige_return();
    if (frmops->n == true) printf("%zd ", line_cnt);
    while (1) {
      word_stat = stdin_get_word(word);
      if (word_stat == STAT_ERR_REC_OVERLEN_WORD) return STAT_ERR_REC_OVERLEN_WORD;
      if (word_stat == STAT_END_REACHED) break;
      if (word_stat == STAT_IS_SPECIAL_CHAR) printf("%s", word);
      else if (word_stat == STAT_IS_WORD) {
        match = comp_nameslist_word (list, word, phops);
        if (match != NULL) {
          match_cnt++;
          printf("%s%s\033[m", match->color, word);
        }
        else printf("%s", word);
      }
    }
    printf("\n");
    // ggf. mehr Anzeigen (Alle Zeilen)
    if (frmops->v == true) write_out_verbose (list, phops, NULL, line_cnt);
  }

  if (frmops->x == true) show_footer (list, phops, match_cnt);
  return STAT_SUCCESS;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus z ... nur Fundzeilen anzeigen
int printout_z (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops)
{
  bool    show_line;
  int     word_stat;
  int     line_stat;
  size_t  line_cnt=0;
  size_t  match_cnt=0;
  name_t  *match;
  word_t  word;
  
  while (1) {
    // Zeile lesen
    line_stat = stdin_read_line();
    line_cnt++;
    if (line_stat == STAT_ERR_REC_OVERLEN_LINE) return STAT_ERR_REC_OVERLEN_LINE;
    if (line_stat == STAT_END_REACHED) break;
    stdin_carrige_return();
    show_line = false;
    // Alle Worte in Zeile prüfen ob passendes Wort in Zeile, noch keine Ausgabe
    while (1) {
      word_stat = stdin_get_word(word);
      if (word_stat == STAT_ERR_REC_OVERLEN_WORD) return STAT_ERR_REC_OVERLEN_WORD;
      if (word_stat == STAT_END_REACHED) break;
      if (word_stat == STAT_IS_WORD) {
        match = comp_nameslist_word (list, word, phops); 
        if (match != NULL) {
          show_line = true;
          break;
        }
      }
    }
    // Wenn Fund in Zeile... das ganze Spiel von vorn, aber alles brav ausgeben
    // Wortlängenüberschreitungen hätten oben schon Fehler ausgelöst
    if (show_line == true) {
      stdin_carrige_return();
      if (frmops->n == true) printf("%zd ", line_cnt);
      while (1) {
        word_stat = stdin_get_word(word);
        if (word_stat == STAT_END_REACHED) break;
        if (word_stat == STAT_IS_SPECIAL_CHAR) printf("%s", word);
        else if (word_stat == STAT_IS_WORD) {
          match = comp_nameslist_word (list, word, phops);
          if (match != NULL) {
            match_cnt++;
            printf("%s%s\033[m", match->color, word);
          }
          else printf("%s", word);
        }
      }
      printf("\n");
      // Normale Ausgabe abgeschlossen ggf. mehr Anzeigen (Fundzeilen)
      if (frmops->v == true) write_out_verbose (list, phops, NULL, line_cnt);
    }
  }

  if (frmops->x == true) show_footer (list, phops, match_cnt);
  return STAT_SUCCESS;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus w ... Fundworte anzeigen
int printout_w (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops)
{
  int     word_stat;
  int     line_stat;
  size_t  line_cnt=0;
  size_t  match_cnt=0;
  name_t  *match;
  word_t  word;
  
  while (1) {
    line_stat = stdin_read_line();
    line_cnt++;
    if (line_stat == STAT_ERR_REC_OVERLEN_LINE) return STAT_ERR_REC_OVERLEN_LINE;
    if (line_stat == STAT_END_REACHED) break;
    stdin_carrige_return();
    while (1) {
      word_stat = stdin_get_word(word);
      if (word_stat == STAT_ERR_REC_OVERLEN_WORD) return STAT_ERR_REC_OVERLEN_WORD;
      if (word_stat == STAT_END_REACHED) break;
      if (word_stat == STAT_IS_WORD) {
        match = comp_nameslist_word (list, word, phops);
        if (match != NULL) {
          match_cnt++;
          if (frmops->n == true) printf("%zd ",line_cnt);
          printf("%s%s\033[m\n", match->color, word);
          // ggf. mehr Anzeigen für einzelnes wort
          if (frmops->v == true) write_out_verbose (list, phops, word, line_cnt);
        }
      }
    }
  }

  if (frmops->x == true) show_footer (list, phops, match_cnt);
  return STAT_SUCCESS;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus c
// Positionen der Funde anzeigen
// Läuft ab wie mode_w nur daß statt der Wörter Positionen angezeigt werden
int printout_c (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops)
{
  size_t  pos, len;
  int     word_stat;
  int     line_stat;
  size_t  line_cnt=0;
  size_t  match_cnt=0;
  name_t  *match;
  word_t  word;
  
  while (1) {
    line_stat = stdin_read_line();
    line_cnt++;
    if (line_stat == STAT_ERR_REC_OVERLEN_LINE) return STAT_ERR_REC_OVERLEN_LINE;
    if (line_stat == STAT_END_REACHED) break;
    stdin_carrige_return();
    while (1) {
      pos = stdin_get_carrige_pos();
      word_stat = stdin_get_word(word);
      if (word_stat == STAT_ERR_REC_OVERLEN_WORD) return STAT_ERR_REC_OVERLEN_WORD;
      if (word_stat == STAT_END_REACHED) break;
      if (word_stat == STAT_IS_WORD) {
        match = comp_nameslist_word (list, word, phops);
        if (match != NULL) {
          match_cnt++;
          len = strlen(word);
          printf("%s%zd %zd %zd\033[m\n", match->color, line_cnt, (pos+1), len);
          // ggf. mehr Anzeigen für einzelnes wort
          if (frmops->v == true) write_out_verbose (list, phops, word, line_cnt);
        }
      }
    }
  }

  if (frmops->x == true) show_footer (list, phops, match_cnt);
  return STAT_SUCCESS;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus convert
// Textzeilen Original, in Wörter zerlegt und allen Codevarianten ausgeben
int printout_convert (void)
{
  int       line_stat;
  int       word_stat;
  int       code_cnt;
  phcode_t  code;
  word_t    word;
      
  while (1) {
    line_stat = stdin_read_line();
    if (line_stat == STAT_ERR_REC_OVERLEN_LINE) return STAT_ERR_REC_OVERLEN_LINE;
    if (line_stat == STAT_END_REACHED) break;
    // 1. Durchgang Zeile Original ausgeben
    // Einmalig WORD_OVERLEN prüfen, ab 2. Durchgang nicht mehr nötig
    printf("Zeile: ");
    stdin_carrige_return();
    while (1) {
      word_stat = stdin_get_word(word);
      if (word_stat ==STAT_ERR_REC_OVERLEN_WORD) return STAT_ERR_REC_OVERLEN_WORD;
      if (word_stat == STAT_END_REACHED) break;
      else printf("%s", word);
    }
    printf("\n");
    // 2. Durchgang Wörter der Zeile ausgeben
    printf("Worte: ");
    stdin_carrige_return();
    while (1) {
      word_stat = stdin_get_word(word);
      if (word_stat == STAT_END_REACHED) break;
      if (word_stat == STAT_IS_WORD) printf("%s ", word);
    }
    printf("\n");
    // 3. Durchgang alle Codevarianten ausgeben
    for (code_cnt = 0; code_cnt < PH_NUMBER_OF_CODES; code_cnt++) {
      printf("Code%s: ", PhCodeSign[code_cnt]);
      stdin_carrige_return();
      while (1) {
        word_stat = stdin_get_word(word);
        if (word_stat == STAT_END_REACHED) break;
        if (word_stat == STAT_IS_WORD) {
          phonetics_get_code (word, code, code_cnt);
          printf("%s ", code);
        }
      }
      printf("\n");
    }
  }  
  
  return STAT_SUCCESS;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Private Unterfunktionen
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Zeigt die Legende zur Suche an
// Wird von Ausgaberoutinen aufgerufen
// Übergeben werden muß Anzahl der Funde
static void show_footer (const nameslist_t *list, const phops_t *phops, const size_t matchcount)
{
  int   cnt;
  bool  show_excludes=false;
  bool  textmode=true; 

  printf("---------------LEGENDE---------------\n");

  //Suchnamen gibt's immer, gleich prüfen ob's auch "Minusnamen" in Liste gibt
  printf("Suche: ");
  for (cnt = 0; cnt < list->number_of_names; cnt++) {
    if (list->items[cnt].exclude == false) {
      printf("%s%s\033[m ", list->items[cnt].color, list->items[cnt].name_norm);
    }
    else show_excludes=true;
  }
  //Verbotene Namen, falls vorhanden
  if (show_excludes == true) {
    printf("\nnicht: ");
    for (cnt = 0; cnt < list->number_of_names; cnt++) {
      if (list->items[cnt].exclude == true) printf("%s ", list->items[cnt].name_norm);
    }
  }
  // Phonetische Suche oder nur Textsuche ?
  for (cnt = 0; cnt < PH_NUMBER_OF_CODES; cnt++) {
    if (phops->show[cnt] == true) {
      textmode = false;
      break;
    }
  }
  // Optionen Phonetik/Levenshtein
  if (textmode == true) {
    if (phops->lev == 0) printf ("\nTyp:   Einfache Textsuche");
    else printf ("\nTyp:   Textsuche mit Levenshtein-Distanz von nicht mehr als %i", phops->lev);
  }
  else {
    printf("\nTyp:   Phonetische Suche ");
    for (cnt = 0; cnt < PH_NUMBER_OF_CODES; cnt++) {
      if (phops->show[cnt] == true) printf("(%s) ", PhCodeName[cnt]);
    }
    if (phops->lev != 0) {
      printf("\n       mit Levenshtein-Distanz von nicht mehr als %i im phonetischen Code",phops->lev);
    }
  }
  // Optionen Längen
  if ((phops->min == 0) && (phops->max != 0)) printf("\nLänge: %i Buchstaben Maximum", phops->max);
  else if ((phops->min != 0) && (phops->max == 0)) printf("\nLänge: %i Buchstaben Minimum", phops->min);
  else if ((phops->min != 0) && (phops->max != 0)) {
    if (phops->min == phops->max) printf("\nLänge: %i Buchstaben", phops->min);
    else printf("\nLänge: %i bis %i Buchstaben", phops->min, phops->max);
  }
  //Funde
  printf("\nFunde: %zd\n", matchcount);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Zeigt genaue Analysedaten an
// Übergabe:  Namensliste, phonetische Optionen, Zeilennummer zur Anzeige
//            Übergabe ein Wort in "word" für Verbose Anzeige für dieses Wort
//            Übergabe NULL Pointer in "word" ganze Zeile wird Verbos ausgegeben
static void write_out_verbose (const nameslist_t *list, const phops_t *phops, const word_t word, const size_t line_no)
{
  int     stat;
  word_t  tmp;
  
  printf("  Zeile=%zd\n", line_no);
  if (word != NULL) verbose_word (list, phops, word);
  else {
    stdin_carrige_return();
    while (1) {
      stat = stdin_get_word(tmp);
      // WORD_OVERLEN könnte geliefert werden...
      // Zeile wurde aber schon mal in write_out_ gelesen, hätte da schon Fehler gegeben
      if (stat == STAT_END_REACHED) break;
      if (stat == STAT_IS_WORD) verbose_word (list, phops, tmp);
    }
  }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Unterfunkton für write_out_verbose
// Verbose Ausgabe für ein Wort
static void verbose_word (const nameslist_t *list, const phops_t *phops, const word_t word)
{
  int       last_idx;
  int       code_cnt, name_cnt;
  bool      textmode=true;
  word_t    word_upper;
  phcode_t  code[PH_NUMBER_OF_CODES];
  int       lev[PH_NUMBER_OF_CODES];
  
  // Phonetische Suche oder nur Textsuche ?
  for (code_cnt = 0; code_cnt < PH_NUMBER_OF_CODES; code_cnt++) {
    if (phops->show[code_cnt] == true) {
      textmode = false;
      break;
    }
  }
  // Einfache Textsuche
  if ((textmode == true) && (phops->lev == 0)) {
    printf("    Wort=%s\n", word);
    for (name_cnt = 0; name_cnt < list->number_of_names; name_cnt++) {
      if (list->items[name_cnt].exclude == true) continue;
      strcpy(word_upper, word);
      str_to_ascii_upper(word_upper);
      if (strcmp(word_upper, list->items[name_cnt].name_upper) == 0) {
        printf("      %sSuche=%s\033[m\n", list->items[name_cnt].color, list->items[name_cnt].name_norm);
      }
      else printf("      Suche=%s\n", list->items[name_cnt].name_norm);
    }
    return;
  }
  // Einfache Textsuche incl. Lev
  if ((textmode == true) && (phops->lev != 0)) {
    printf("    Wort=%s\n", word);
    for (name_cnt = 0; name_cnt < list->number_of_names; name_cnt++) {
      if (list->items[name_cnt].exclude == true) continue;
      printf("      Suche=%s\n", list->items[name_cnt].name_norm);
      strcpy(word_upper, word);
      str_to_ascii_upper(word_upper);
      lev[0] = str_lev(word_upper, list->items[name_cnt].name_upper);
      if (lev[0] <= phops->lev) printf("        %sLev=%i\033[m\n", list->items[name_cnt].color, lev[0]);
      else printf("        Lev=%i\n", lev[0]);
    }
    return;
  }

  // Phonetische Suche
  // letzten "Plusnamen" aus Liste bestimmen, sonst gibt's keine schöne Grafik
  // Falls der letzte Name ein "Minusname" ist
  last_idx=(list->number_of_names)-1;
  while (1) {
    if (list->items[last_idx].exclude == false) break;
    last_idx--;
  }
  // Wort und dessen Codes in einer Zeile ausgeben
  printf("    Wort=%s\n", word);
  printf("    │ ");
  for (code_cnt = 0; code_cnt < PH_NUMBER_OF_CODES; code_cnt++) {
    if (phops->show[code_cnt] == true) {
      phonetics_get_code (word, code[code_cnt], code_cnt);
      printf("Code%s=%s ", PhCodeSign[code_cnt], code[code_cnt]);
    }
  }
  printf("\n");
  // Suchnamen ausgeben und Codes in einer Zeile
  for (name_cnt = 0; name_cnt <= last_idx; name_cnt++) {
    if (list->items[name_cnt].exclude == false) {
      // einen Suchnamen ausgeben, Verbindungslinien zu Wort anständig zeichnen
      if (name_cnt < last_idx) printf("    ├─Suche=%s\n", list->items[name_cnt].name_norm);
      else printf("    └─Suche=%s\n", list->items[name_cnt].name_norm);
      if (name_cnt < last_idx) printf("    │ ");
      else printf("      ");
      // Codes zu Suchnamen in eine Zeile
      for (code_cnt = 0; code_cnt < PH_NUMBER_OF_CODES; code_cnt++) {
        if (phops->show[code_cnt] == true) {
          if (strcmp(list->items[name_cnt].code[code_cnt], code[code_cnt]) == 0) {
            printf("%sCode%s=%s\033[m ", 
              list->items[name_cnt].color, PhCodeSign[code_cnt], list->items[name_cnt].code[code_cnt]);
          }
          else printf("Code%s=%s ", PhCodeSign[code_cnt], list->items[name_cnt].code[code_cnt]);
        }
      }
      printf("\n");
      // ggf. jetzt noch alle Lev-Distanzen in eine Zeile
      if (phops->lev != 0) {
        if (name_cnt < last_idx) printf("    │   ");
        else printf("        ");
        for (code_cnt = 0; code_cnt < PH_NUMBER_OF_CODES; code_cnt++) {
          lev[code_cnt] = str_lev (list->items[name_cnt].code[code_cnt], code[code_cnt]);
          if (phops->show[code_cnt] == true) {
            if (lev[code_cnt] <= phops->lev) {
              printf("%sLev%s=%i\033[m ", list->items[name_cnt].color, PhCodeSign[code_cnt], lev[code_cnt]);
            }
            else printf("Lev%s=%i ", PhCodeSign[code_cnt], lev[code_cnt]);
          }
        }
        printf("\n");
      }
    }
  }
}
