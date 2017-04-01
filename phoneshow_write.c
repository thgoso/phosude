/*
phoneshow_write

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
#include "phonetics.h"
#include "buffer_sizes.h"
#include "phoneshow_str.h"
#include "phoneshow_types.h"
#include "phoneshow_read.h"
#include "phoneshow_comp.h"
#include "phoneshow_write.h"


// Private Funktionen Prototypen
static void write_verbose (const nameslist_t *list, const phops_t *phops, const char *word, const size_t line_no);
static void verbose_word (const nameslist_t *list, const phops_t *phops, const char *word);
static void show_footer (const nameslist_t *list, const phops_t *phops, const size_t foundcount);


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Öffentliche Funktionen zur Ausgabe liefern:
// PH_MATCH_TRUE         mind. einem Fund
// PH_MATCH_FALSE        wenn kein Fund
// PH_ERR_WORD_OVERSIZE  Wort mit Überlänge von stdin empfangen
// PH_ERR_LINE_OVERSIZE  Zeile mit Überlänge von stdin empfangen
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus a
// gesammten Text anzeigen
int write_out_a (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops)
{
  int     line_stat;
  int     word_stat;
  char    word[BUFFER_SIZE_WORD];
  int     name_idx;
  size_t  found_cnt=0;
  size_t  line_cnt=0;

  while (1) {
    line_stat = read_line();
    line_cnt++;
    if (line_stat == READ_LINE_OVERSIZE) return WRITE_LINE_OVERSIZE;
    if (line_stat == READ_LINE_END) break;
    read_word_init();
    if (frmops->n == true) printf("%zd ", line_cnt);
    while (1) {
      word_stat = read_word(word);
      if (word_stat == READ_WORD_OVERSIZE) return WRITE_WORD_OVERSIZE;
      if (word_stat == READ_WORD_END) break;
      if (word_stat == READ_WORD_IS_NO_WORD) printf("%s", word);
      else if (word_stat == READ_WORD_IS_WORD) {
        if (compare_word_nameslist(list, phops, word, &name_idx) == true) {
          found_cnt++;
          printf("%s%s\033[m", list->items[name_idx].color_string, word);
        }
        else printf("%s", word);
      }
    }
    printf("\n");
    // ggf. mehr Anzeigen (Alle Zeilen)
    if (frmops->v == true) write_verbose(list, phops, NULL, line_cnt);
  }
    
  if (frmops->x == true) show_footer(list, phops, found_cnt);
  if (found_cnt > 0) return WRITE_MATCH;
  else return WRITE_NO_MATCH;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus z
// Nur Fundzeilen anzeigen
int write_out_z (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops)
{
  int     line_stat;
  int     word_stat;
  char    word[BUFFER_SIZE_WORD];
  int     name_idx;
  size_t  found_cnt=0;
  size_t  line_cnt=0;
  bool    show_line;
  
  while (1) {
    line_stat = read_line();
    line_cnt++;
    if (line_stat == READ_LINE_OVERSIZE) return WRITE_LINE_OVERSIZE;            // Überlange Zeile-> Raus
    if (line_stat == READ_LINE_END) break;                                      // Zeilenende
    read_word_init();                                                           // Wortlesefunktion init 
    show_line = false;
    while (1) {                                                                 // Zeile komplett prüfen
      word_stat = read_word(word);                                              // ob sich ein passendes Wort
      if (word_stat == READ_WORD_OVERSIZE) return WRITE_WORD_OVERSIZE;        
      if (word_stat == READ_WORD_END) break;                                    // Zeile durch
      if (word_stat == READ_WORD_IS_WORD) {
        if (compare_word_nameslist(list, phops, word, &name_idx) == true) {
          show_line = true;
          break;
        }
       }
    }
    if (show_line == true) {                                                    // Zeile muß ausgegeben werden, da Fund(e)
      read_word_init();                                                         // Wortlesefunktion init 
      if (frmops->n == true) printf("%zd ", line_cnt);                          // Zeilennummer ausgeben falls erwünscht
      
      while (1) {
        word_stat = read_word(word);                                            // Nächstes Wort aus Zeile holen
        if (word_stat == READ_WORD_OVERSIZE) return WRITE_WORD_OVERSIZE;        // Überlanges Wort-> Raus
        if (word_stat == READ_WORD_END) break;                                  // Alle durch
        if (word_stat == READ_WORD_IS_NO_WORD) printf("%s", word);              // Sonderzeichen ausgeben
        else if (word_stat == READ_WORD_IS_WORD) {                              // War gültiges deutsche Wort
          if (compare_word_nameslist(list, phops, word, &name_idx) == true) {   // Phonetisch Vergleichen mit Namensliste
            found_cnt++;                                                        // Wir haben einen Fund    
            printf("%s%s\033[m", list->items[name_idx].color_string, word);
          }
          else printf("%s", word);
        }
      }
      printf("\n");                                                             // Zeile ist durch
      if (frmops->v == true) write_verbose(list, phops, NULL, line_cnt);        // ggf. mehr Anzeigen (Fundzeilen)
    }
  }
    
  if (frmops->x == true) show_footer(list, phops, found_cnt);                   // Legende anzeigen
  if (found_cnt > 0) return WRITE_MATCH;                                        // Ende mit true wenn Fund(e) waren
  else return WRITE_NO_MATCH;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus w
// nur Fundworte anzeigen
int write_out_w (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops)
{
  int     line_stat;
  int     word_stat;
  char    word[BUFFER_SIZE_WORD];
  int     name_idx;
  size_t  found_cnt=0;
  size_t  line_cnt=0;
  
  while (1) {
    line_stat = read_line();
    line_cnt++;
    if (line_stat == READ_LINE_OVERSIZE) return WRITE_LINE_OVERSIZE;
    if (line_stat == READ_LINE_END) break;
    read_word_init();
    while (1) {
      word_stat = read_word(word);
      if (word_stat == READ_WORD_OVERSIZE) return WRITE_WORD_OVERSIZE;
      if (word_stat == READ_WORD_END) break;
      if (word_stat == READ_WORD_IS_WORD) {
        if (compare_word_nameslist(list, phops, word, &name_idx) == true) {
          found_cnt++;
          if (frmops->n == true) printf("%zd ",line_cnt);
          printf("%s%s\033[m\n", list->items[name_idx].color_string, word);
          // ggf. mehr Anzeigen für einzelnes wort
          if (frmops->v == true) write_verbose(list, phops, word, line_cnt);               
        }
      }
    }
  }
    
  if (frmops->x == true) show_footer(list, phops, found_cnt);
  if (found_cnt > 0) return WRITE_MATCH;
  else return WRITE_NO_MATCH;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Private Funktion
// Zeigt die Legende zur Suche an
// Wird von Ausgaberoutinen aufgerufen
// Übergeben werden muß Anzahl der Funde
static void show_footer (const nameslist_t *list, const phops_t *phops, const size_t foundcount)
{
  int cnt;
  int nolist=0;

  printf("---------------LEGENDE---------------\n");
  
  //Suchnamen
  printf("Suche: ");
  for (cnt = 0; cnt < list->number_of_names; cnt++) {
    if (list->items[cnt].is_minusname == false) {
      printf("%s%s\033[m ", list->items[cnt].color_string, list->items[cnt].name_norm);
    }
    else nolist=1;
  }

  //Verbotene Namen
  if (nolist == 1) {
    printf("\nnicht: ");
    for (cnt = 0; cnt < list->number_of_names; cnt++) {
      if (list->items[cnt].is_minusname == true) printf("%s ", list->items[cnt].name_norm);
    }
  }

  //Optionen
  if (phops->k == false && phops->p == false && phops->s == false && phops->e == false && phops->l == 0) {
    printf ("\nSuche: Einfache Textsuche");
  }
  else if (phops->k == false && phops->p == false && phops->s == false && phops->e == false && phops->l != 0) {
    printf ("\nTyp:   Textsuche mit Levenshtein-Distanz von nicht mehr als %i", phops->l);
  }
  else {
    printf("\nTyp:   Phonetische Suche ");
    if (phops->k == true) printf ("(Kölner Phonetik) ");
    if (phops->p == true) printf ("(Phonem) ");
    if (phops->s == true) printf ("(Soundex) ");
    if (phops->e == true) printf ("(Extended Soundex) ");
    if (phops->l != 0) printf ("\n       mit Levenshtein-Distanz von nicht mehr als %i im phonetischen Code", phops->l);
  }

  //Funde
  printf("\nFunde: %zd\n", foundcount);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Verbose Ausgabe
// Übergabe: NULL-Pointer ... Zusatzinfos für ganze Zeile ausgeben
//           Pointer auf Wort ... Zusatzinfos für Wort ausgeben
//           Zeilennummer aktuelle Zeile
// wird von den Ausgaberoutinen write_mode_ angesprungen
// NACHDEM eine neue Zeile gelesen/ausgegeben wurde
static void write_verbose (const nameslist_t *list, const phops_t *phops, const char *word, const size_t line_no)
{
  int   stat;
  char  tmp[BUFFER_SIZE_WORD];
  
  printf("  Zeile=%zd\n", line_no);
  if (word != NULL) verbose_word(list, phops, word);
  else {
    read_word_init(); 
    while (1) {
      stat = read_word(tmp);
      // READ_WORD_OVERSIZE könnte geliefert werden...
      // Zeile wurde aber schon mal in write_mode_ gelesen, hätte da schon Fehler gegeben
      if (stat == READ_WORD_END) break;
      if (stat == READ_WORD_IS_WORD) verbose_word(list, phops, tmp);
    }
  }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Unterfunktion von write_verbose
// Verbose Ausgabe Wort
// wird von write_verbose angesprungen
static void verbose_word (const nameslist_t *list, const phops_t *phops, const char *word)
{
  char code_k[BUFFER_SIZE_CODE];
  char code_p[BUFFER_SIZE_CODE];
  char code_s[BUFFER_SIZE_CODE];
  char code_e[BUFFER_SIZE_CODE];
  char word_lower[BUFFER_SIZE_WORD];
  int  lev_k, lev_p, lev_s, lev_e, lev;
  int  cnt, last_idx;
    
  // letzten "Plusnamen" aus Liste bestimmen
  // Sonst gibt's keine schöne Grafik
  last_idx=(list->number_of_names)-1;
  while (1) {
    if (list->items[last_idx].is_minusname == false) break;
    last_idx--;
  }
  
  // Defaultstart
  strcpy(word_lower, word);
  to_lower(word_lower);
  printf("    Wort=%s\n", word);
  if (phops->k == false && phops->p == false && phops->s == false && phops->e == false) {
    if (phops->l == 0) return;
    for (cnt = 0; cnt < list->number_of_names; cnt++) {
      if (list->items[cnt].is_minusname == false) {
        lev=levenshtein(word_lower, list->items[cnt].name_lower);
        printf("      Suche=%s ", list->items[cnt].name_norm);
        if (lev <= phops->l) printf("Lev=%s%i\033[m\n", list->items[cnt].color_string, lev);
        else printf("Lev=%i\n", lev);
      }
    }
    return;
  }
  
  // Hier nur weiter bei Textsuche mit phon. Optionen
  phoneconvert_cologne(word, code_k);
  phoneconvert_phonem(word, code_p);
  phoneconvert_soundex(word, code_s);
  phoneconvert_exsoundex(word, code_e);
  printf("    │ ");
  if (phops->k == true) printf("CodeK=%s ", code_k);
  if (phops->p == true) printf("CodeP=%s ", code_p);
  if (phops->s == true) printf("CodeS=%s ", code_s);
  if (phops->e == true) printf("CodeE=%s ", code_e);
  printf("\n");

  for (cnt = 0; cnt < list->number_of_names; cnt++) {
    if (list->items[cnt].is_minusname == false) {
      if (cnt < last_idx) printf("    ├─Suche=%s\n", list->items[cnt].name_norm);
      else printf("    └─Suche=%s\n", list->items[cnt].name_norm);
      if (cnt < last_idx) printf("    │ ");
      else printf("      ");
      if (phops->k == true) {
        if (strcmp(list->items[cnt].code_k, code_k) == 0) {
          printf("CodeK=%s%s\033[m ", list->items[cnt].color_string, list->items[cnt].code_k);
        }
        else printf("CodeK=%s ", list->items[cnt].code_k);
      }
      if (phops->p == true) {
        if (strcmp(list->items[cnt].code_p, code_p) == 0) {
          printf("CodeP=%s%s\033[m ", list->items[cnt].color_string, list->items[cnt].code_p);
        }
        else printf("CodeP=%s ", list->items[cnt].code_p);
      }
      if (phops->s == true) {
        if (strcmp(list->items[cnt].code_s, code_s) == 0) {
          printf("CodeS=%s%s\033[m ", list->items[cnt].color_string, list->items[cnt].code_s);
        }
        else printf("CodeS=%s ", list->items[cnt].code_s);
      }
      if (phops->e == true) {
        if (strcmp(list->items[cnt].code_e, code_e) == 0) {
          printf("CodeE=%s%s\033[m ", list->items[cnt].color_string, list->items[cnt].code_e);
        }  
        else printf("CodeE=%s ", list->items[cnt].code_e);
      }
      printf("\n");
      
      // Hier mur weiter wenn phon. Suche incl. Lev-Dist
      if (phops->l > 0) {
        lev_k=levenshtein(list->items[cnt].code_k, code_k);
        lev_p=levenshtein(list->items[cnt].code_p, code_p);
        lev_s=levenshtein(list->items[cnt].code_s, code_s);
        lev_e=levenshtein(list->items[cnt].code_e, code_e);
        if (cnt < last_idx) printf("    │   ");
        else printf("        ");
        if (phops->k == true) {
          if (lev_k <= phops->l) printf("LevK=%s%i\033[m ", list->items[cnt].color_string, lev_k);
          else printf("LevK=%i ", lev_k);
        }
        if (phops->p == true) {
          if (lev_p <= phops->l) printf("LevP=%s%i\033[m ", list->items[cnt].color_string, lev_p);
          else printf("LevP=%i ", lev_p);
          }
        if (phops->s == true) {
          if (lev_s <= phops->l) printf("LevS=%s%i\033[m ", list->items[cnt].color_string, lev_s);
          else printf("LevS=%i ", lev_s);
        }
        if (phops->e == true) {
          if (lev_e <= phops->l) printf("LevE=%s%i\033[m ", list->items[cnt].color_string, lev_e);
          else printf("LevE=%i ", lev_e);
        }
        printf("\n"); 
      }  
    }
  }
}
