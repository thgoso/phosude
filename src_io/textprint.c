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
#include "../phosude.h"
#include "stdin.h"
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
  int           line_stat;
  int           word_stat;
  int           code_cnt;
  word_t        word;
  cologne_t     code_k;
  phonem_t      code_p;
  soundex_t     code_s;
  exsoundex_t   code_e;
  caverphone_t  code_c;
  daitchmok_t   code_d;
  static const char sign[6] = {'K', 'P', 'S', 'E', 'C', 'D'};
        
  while (1) {
    line_stat = stdin_read_line();
    if (line_stat == STAT_ERR_REC_OVERLEN_LINE) return STAT_ERR_REC_OVERLEN_LINE;
    if (line_stat == STAT_END_REACHED) break;
    // Zeile Original ausgeben
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
    // Wörter der Zeile ausgeben
    printf("Worte: ");
    stdin_carrige_return();
    while (1) {
      word_stat = stdin_get_word(word);
      if (word_stat == STAT_END_REACHED) break;
      if (word_stat == STAT_IS_WORD) printf("%s ", word);
    }
    printf("\n");
    // Alle 6 Codevarianten ausgeben
    for (code_cnt = 0; code_cnt < 6; code_cnt++) {
      printf("Code%c: ", sign[code_cnt]);
      stdin_carrige_return();
      while (1) {
        word_stat = stdin_get_word(word);
        if (word_stat == STAT_END_REACHED) break;
        if (word_stat == STAT_IS_WORD) {
          if (sign[code_cnt] == 'K') {
            make_cologne (word, code_k);
            printf("%s ", code_k);
          }
          else if (sign[code_cnt] == 'P') {
            make_phonem (word, code_p);
            printf("%s ", code_p);
          }
          else if (sign[code_cnt] == 'S') {
            make_soundex (word, code_s);
            printf("%s ", code_s);
          }
          else if (sign[code_cnt] == 'E') {
            make_exsoundex (word, code_e);
            printf("%s ", code_e);
          }
          else if (sign[code_cnt] == 'C') {
            make_caverphone (word, code_c);
            printf("%s ", code_c);
          }
          else if (sign[code_cnt] == 'D') {
            make_daitchmok (word, code_d);
            printf("%s ", code_d);
          }
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
  // Nur Textsuche
  if (phops->textmode == true) {
    if (phops->lev == 0) printf ("\nTyp:   Einfache Textsuche");
    else printf ("\nTyp:   Textsuche mit Levenshtein-Distanz von nicht mehr als %i", phops->lev);
  }
  // Phonetische Suche
  else {
    printf("\nTyp:   Phonetische Suche ");
    if (phops->show_k == true) printf("-K ");
    if (phops->show_p == true) printf("-P ");
    if (phops->show_s == true) printf("-S ");
    if (phops->show_e == true) printf("-E ");
    if (phops->show_c == true) printf("-C ");
    if (phops->show_d == true) printf("-D ");    
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
  int           last_idx;
  int           name_cnt;
  int           lev;
  word_t        word_upper;
  cologne_t     code_k;
  phonem_t      code_p;
  soundex_t     code_s;
  exsoundex_t   code_e;
  caverphone_t  code_c;
  daitchmok_t   code_d;
  
  // Einfache Textsuche
  if ((phops->textmode == true) && (phops->lev == 0)) {
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
  if ((phops->textmode == true) && (phops->lev != 0)) {
    printf("    Wort=%s\n", word);
    for (name_cnt = 0; name_cnt < list->number_of_names; name_cnt++) {
      if (list->items[name_cnt].exclude == true) continue;
      printf("      Suche=%s\n", list->items[name_cnt].name_norm);
      strcpy(word_upper, word);
      str_to_ascii_upper(word_upper);
      lev = str_lev(word_upper, list->items[name_cnt].name_upper);
      if (lev <= phops->lev) printf("        %sLev=%i\033[m\n", list->items[name_cnt].color, lev);
      else printf("        Lev=%i\n", lev);
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
  if (phops->show_k == true) {
    make_cologne(word, code_k);
    printf("CodeK=%s ", code_k);
  }
  if (phops->show_p == true) {
    make_phonem(word, code_p);
    printf("CodeP=%s ", code_p);
  }
  if (phops->show_s == true) {
    make_soundex(word, code_s);
    printf("CodeS=%s ", code_s);
  }
  if (phops->show_e == true) {
    make_exsoundex(word, code_e);
    printf("CodeE=%s ", code_e);
  }
  if (phops->show_c == true) {
    make_caverphone(word, code_c);
    printf("CodeC=%s ", code_c);
  }
   if (phops->show_d == true) {
    make_daitchmok(word, code_d);
    printf("CodeD=%s ", code_d);
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
      // Codes zu Suchnamen in eine Zeile, Lev erst mal nicht berücksichtigen
      if (phops->show_k == true) {
        lev=0;
        if (comp_word_cologne (word, list->items[name_cnt].code_k, &lev) == true) {
          printf("%sCodeK=%s\033[m ", list->items[name_cnt].color, list->items[name_cnt].code_k);
        }
        else printf("CodeK=%s ", list->items[name_cnt].code_k);
      }
      if (phops->show_p == true) {
        lev=0;
        if (comp_word_phonem (word, list->items[name_cnt].code_p, &lev) == true) {
          printf("%sCodeP=%s\033[m ", list->items[name_cnt].color, list->items[name_cnt].code_p);
        }
        else printf("CodeP=%s ", list->items[name_cnt].code_p);
      }
      if (phops->show_s == true) {
        lev=0;
        if (comp_word_soundex (word, list->items[name_cnt].code_s, &lev) == true) {
          printf("%sCodeS=%s\033[m ", list->items[name_cnt].color, list->items[name_cnt].code_s);
        }
        else printf("CodeS=%s ", list->items[name_cnt].code_s);
      }
      if (phops->show_e == true) {
        lev=0;
        if (comp_word_exsoundex (word, list->items[name_cnt].code_e, &lev) == true) {
          printf("%sCodeE=%s\033[m ", list->items[name_cnt].color, list->items[name_cnt].code_e);
        }
        else printf("CodeE=%s ", list->items[name_cnt].code_e);
      }
      if (phops->show_c == true) {
        lev=0;
        if (comp_word_caverphone (word, list->items[name_cnt].code_c, &lev) == true) {
          printf("%sCodeC=%s\033[m ", list->items[name_cnt].color, list->items[name_cnt].code_c);
        }
        else printf("CodeC=%s ", list->items[name_cnt].code_c);
      }
      if (phops->show_d == true) {
        lev=0;
        if (comp_word_daitchmok (word, list->items[name_cnt].code_d, &lev) == true) {
          printf("%sCodeD=%s\033[m ", list->items[name_cnt].color, list->items[name_cnt].code_d);
        }
        else printf("CodeD=%s ", list->items[name_cnt].code_d);
      }
      printf("\n");
      // ggf. jetzt noch alle Lev-Distanzen in eine Zeile
      if (phops->lev != 0) {
        if (name_cnt < last_idx) printf("    │   ");
        else printf("        ");
        if (phops->show_k == true) {
          lev = phops->lev;
          if (comp_word_cologne (word, list->items[name_cnt].code_k, &lev) == true) {
            printf("%sLevK=%i\033[m ", list->items[name_cnt].color, lev);
          }
          else printf("LevK=%i ", lev);
        }
        if (phops->show_p == true) {
          lev = phops->lev;
          if (comp_word_phonem (word, list->items[name_cnt].code_p, &lev) == true) {
            printf("%sLevP=%i\033[m ", list->items[name_cnt].color, lev);
          }
          else printf("LevP=%i ", lev);
        }
        if (phops->show_s == true) {
          lev = phops->lev;
          if (comp_word_soundex (word, list->items[name_cnt].code_s, &lev) == true) {
            printf("%sLevS=%i\033[m ", list->items[name_cnt].color, lev);
          }
          else printf("LevS=%i ", lev);
        }
        if (phops->show_e == true) {
          lev = phops->lev;
          if (comp_word_exsoundex (word, list->items[name_cnt].code_e, &lev) == true) {
            printf("%sLevE=%i\033[m ", list->items[name_cnt].color, lev);
          }
          else printf("LevE=%i ", lev);
        }
        if (phops->show_c == true) {
          lev = phops->lev;
          if (comp_word_caverphone (word, list->items[name_cnt].code_c, &lev) == true) {
            printf("%sLevC=%i\033[m ", list->items[name_cnt].color, lev);
          }
          else printf("LevC=%i ", lev);
        }
        if (phops->show_d == true) {
          lev = phops->lev;
          if (comp_word_daitchmok (word, list->items[name_cnt].code_d, &lev) == true) {
            printf("%sLevD=%i\033[m ", list->items[name_cnt].color, lev);
          }
          else printf("LevD=%i ", lev);
        }
        printf("\n");
      }
    }
  }
}
