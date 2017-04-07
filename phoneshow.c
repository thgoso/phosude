/*
phoneshow
Führt eine Phonetische Suche in Text aus
Suchverfahren sind:
Kölner Phonetik   für deutsche Namen nach eigener Erfahrung am optimalsten
                  Code Numerisch, variable Länge
Phonem            in versch. dBase Varianten implementiert
                  Code Buchstaben, variable Länge
Soundex           Für deutsche Namen nicht ganz geeignet, aber Standard bei Genealogie(programmen)
                  Alphanumerischer Code, fixe Länge, Format Buchstabe gefolgt von 3 Ziffern
Extended Soundex  Erweiterung von Soundex
                  Numerischer Code, fixe Länge, 5 Ziffern


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
#include <stdlib.h>
#include <string.h>
#include "phonetics.h"
#include "stdin.h"
#include "string.h"
#include "phoneshow.h"


static nameslist_t    List = {0, NULL};
static phops_t        PhOps = {0, {false, false, false, false}};
static frmops_t       FrmOps = {false, true, false};
static word_t         CurWord="";


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int main (int argc, char* argv[])
{
  char        *param;
  int         op_cnt, name_cnt, code_cnt;
  int         status;
  int         number_of_names;
  int         idxstart, idxend;
  int         color_cnt=0;
  bool        only_excludes=true;
  bool        phparam;
  outops_t    outops = {false, false, false, false, true};
  

  // Wenn keine Übergabeparameter Text kodieren und schon fertig
  // Funktion liefert Rückgabekonstannte für exit_func
  if (argc == 1) {
    status = write_out_convert();
    exit_func (status);
  }

  // Optionen aus Übergabe lesen und die Vars danach setzten
  for (op_cnt=1; op_cnt < argc; op_cnt++) {
    param=argv[op_cnt];
    // Parameter Phonetische Verfahren
    phparam = false;
    for (code_cnt = 0; code_cnt < NUMBER_OF_PHCODES; code_cnt++) {
      if (strcmp(param, PhParam[code_cnt]) == 0) {
        PhOps.show[code_cnt] = true;
        phparam = true;
        break;
      }
    }
    if (phparam == true) continue;
    // Restparameter
    if (strcmp (param, "-l") == 0) PhOps.lev++;
    else if (strcmp (param, "-n") == 0) FrmOps.n = true;
    else if (strcmp (param, "-x") == 0) FrmOps.x = false;
    else if (strcmp (param, "-v") == 0) FrmOps.v = true;
    else if (strcmp (param, "-a") == 0) outops.a = true;
    else if (strcmp (param, "-z") == 0) outops.z = true;
    else if (strcmp (param, "-w") == 0) outops.w = true;
    else if (strcmp (param, "-c") == 0) outops.c = true;
    else if (strcmp (param, "-f") == 0) outops.f = false;
    else if (strcmp (param, "-h") == 0) exit_func(PHSHOW_PRINT_HELP);
    else if (strcmp (param, "-b") == 0) exit_func(PHSHOW_PRINT_EXAMPLES);
    else break;
  }

  // Wenn kein Anzeigeparameter übergeben (-a -z -w -c) ... -z einschalten
  if (outops.a == false && outops.z == false && outops.w == false && outops.c == false) outops.z = true;

  // Keine Namen übergeben = Fehler
  if (op_cnt == argc) exit_func (PHSHOW_ERR_PARAM);

  // Index setzten 1. und letzter Name in Argliste, Anzahl
  idxstart = op_cnt;
  idxend = argc - 1;
  number_of_names = (idxend - idxstart) + 1;

  // Speicher für items der Namensliste anfordern, Ende bei bei Fehler
  if (init_names_list (number_of_names) == false) exit_func(PHSHOW_ERR_MEM);

  // Namensliste füllen
  for (name_cnt = 0; name_cnt < number_of_names; name_cnt++) {
    // Name aus Übergabe holen, Ende wenn zu lang
    param = argv[name_cnt + idxstart];
    if (strlen(param) < 2) exit_func (PHSHOW_ERR_NAME_UNDERLEN);
    // Minusnamen... .exclude, .name_norm, .name_upper setzten, Rest default lassen
    if (param[0] == '_') {
      List.items[name_cnt].exclude = true;
      param++;
      strcpy(List.items[name_cnt].name_norm, param);
      strcpy(List.items[name_cnt].name_upper, param);
      str_to_upper(List.items[name_cnt].name_upper);
    }
    // Suchnamen, alles setzten, Hilfsvar only_excludes clear
    else {
      only_excludes=false;
      List.items[name_cnt].exclude = false;
      strcpy(List.items[name_cnt].name_norm, param);
      strcpy(List.items[name_cnt].name_upper, param);
      str_to_upper(List.items[name_cnt].name_upper);
      if (outops.f == true) {
        List.items[name_cnt].color = Color_Table[color_cnt % NUMBER_OF_COLORS];
        color_cnt++;
      }
      // Immer alle Codes erzeugen, egal welche per Parameter gewählt
      // Bei Fehler Codeerstellung ist Übergabename kein Wort, oder zu Lang
      for (code_cnt = 0; code_cnt < NUMBER_OF_PHCODES; code_cnt++) {
        status = phonetics_get_code (param, List.items[name_cnt].code[code_cnt], code_cnt);
        if (status == PHONETICS_ERR_OVERLEN) exit_func (PHSHOW_ERR_NAME_OVERLEN);
        if (status == PHONETICS_ERR_NO_WORD) exit_func (PHSHOW_ERR_NAME_NOT_GERMAN);
      }
    }
  }

  // Wenn Namensliste NUR aus Minusnamen besteht = Fehler... Irgendwas MUSS ja gesucht werden
  if (only_excludes == true) exit_func(PHSHOW_ERR_PARAM);
  
  // Ausgabe starten, wenn mehrere gewählt, gewinnt die mit mehr Textausgabe
  // Funktionen liefern passende Statuskonstannte für exit_func 
  if (outops.a == true) status = write_out_a();
  else if (outops.z == true) status = write_out_z();
  else if (outops.w == true) status = write_out_w();
  else status = write_out_c();

  // Ende
  exit_func(status);
  
  // Wird nie erreicht
  return 255;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgaberoutinen
// Werden von Main aufgerufen und liefern Statuskonstannte für exit_func zurück
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus a ... gesammten Text anzeigen
static int write_out_a (void)
{
  int     word_stat;
  int     line_stat;
  size_t  line_cnt=0;
  size_t  match_cnt=0;
  name_t  *match;
 
  while (1) {
    line_stat = stdin_read_line();
    line_cnt++;
    if (line_stat == STDIN_ERR_LINE_OVERLEN) return PHSHOW_ERR_LINE_OVERLEN;
    if (line_stat == STDIN_END_REACHED) break;
    stdin_carrige_return();
    if (FrmOps.n == true) printf("%zd ", line_cnt);
    while (1) {
      word_stat = stdin_get_word(CurWord);
      if (word_stat == STDIN_ERR_WORD_OVERLEN) return PHSHOW_ERR_WORD_OVERLEN;
      if (word_stat == STDIN_END_REACHED) break;
      if (word_stat == STDIN_IS_SPECIAL_CHAR) printf("%s", CurWord);
      else if (word_stat == STDIN_IS_WORD) {
        match = comp_cur_word_nameslist();
        if (match != NULL) {
          match_cnt++;
          printf("%s%s\033[m", match->color, CurWord);
        }
        else printf("%s", CurWord);
      }
    }
    printf("\n");
    // ggf. mehr Anzeigen (Alle Zeilen)
    if (FrmOps.v == true) write_out_verbose(line_cnt, true);
  }

  if (FrmOps.x == true) show_footer(match_cnt);
  if (match_cnt > 0) return PHSHOW_MATCH;
  else return PHSHOW_NO_MATCH;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus z ... nur Fundzeilen anzeigen
static int write_out_z (void)
{
  bool    show_line;
  int     word_stat;
  int     line_stat;
  size_t  line_cnt=0;
  size_t  match_cnt=0;
  name_t  *match;
  
  while (1) {
    // Zeile lesen
    line_stat = stdin_read_line();
    line_cnt++;
    if (line_stat == STDIN_ERR_LINE_OVERLEN) return PHSHOW_ERR_LINE_OVERLEN;
    if (line_stat == STDIN_END_REACHED) break;
    stdin_carrige_return();
    show_line = false;
    // Alle Worte in Zeile prüfen ob passendes Wort in Zeile, noch keine Ausgabe
    while (1) {
      word_stat = stdin_get_word(CurWord);
      if (word_stat == STDIN_ERR_WORD_OVERLEN) return PHSHOW_ERR_WORD_OVERLEN;
      if (word_stat == STDIN_END_REACHED) break;
      if (word_stat == STDIN_IS_WORD) {
        match = comp_cur_word_nameslist();
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
      if (FrmOps.n == true) printf("%zd ", line_cnt);
      while (1) {
        word_stat = stdin_get_word(CurWord);
        if (word_stat == STDIN_END_REACHED) break;
        if (word_stat == STDIN_IS_SPECIAL_CHAR) printf("%s", CurWord);
        else if (word_stat == STDIN_IS_WORD) {
          match = comp_cur_word_nameslist();
          if (match != NULL) {
            match_cnt++;
            printf("%s%s\033[m", match->color, CurWord);
          }
          else printf("%s", CurWord);
        }
      }
      printf("\n");
      // Normale Ausgabe abgeschlossen ggf. mehr Anzeigen (Fundzeilen)
      if (FrmOps.v == true) write_out_verbose(line_cnt, true);
    }
  }

  if (FrmOps.x == true) show_footer(match_cnt);
  if (match_cnt > 0) return PHSHOW_MATCH;
  else return PHSHOW_NO_MATCH;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus w ... Fundworte anzeigen
static int write_out_w (void)
{
  int     word_stat;
  int     line_stat;
  size_t  line_cnt=0;
  size_t  match_cnt=0;
  name_t  *match;
  
  while (1) {
    line_stat = stdin_read_line();
    line_cnt++;
    if (line_stat == STDIN_ERR_LINE_OVERLEN) return PHSHOW_ERR_LINE_OVERLEN;
    if (line_stat == STDIN_END_REACHED) break;
    stdin_carrige_return();
    while (1) {
      word_stat = stdin_get_word(CurWord);
      if (word_stat == STDIN_ERR_WORD_OVERLEN) return PHSHOW_ERR_WORD_OVERLEN;
      if (word_stat == STDIN_END_REACHED) break;
      if (word_stat == STDIN_IS_WORD) {
        match = comp_cur_word_nameslist ();
        if (match != NULL) {
          match_cnt++;
          if (FrmOps.n == true) printf("%zd ",line_cnt);
          printf("%s%s\033[m\n", match->color, CurWord);
          // ggf. mehr Anzeigen für einzelnes wort
          if (FrmOps.v == true) write_out_verbose(line_cnt, false);
        }
      }
    }
  }

  if (FrmOps.x == true) show_footer(match_cnt);
  if (match_cnt > 0) return PHSHOW_MATCH;
  else return PHSHOW_NO_MATCH;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus c
// Positionen der Funde anzeigen
// Läuft ab wie mode_w nur daß statt der Wörter Positionen angezeigt werden
static int write_out_c (void)
{
  size_t  pos, len;
  int     word_stat;
  int     line_stat;
  size_t  line_cnt=0;
  size_t  match_cnt=0;
  name_t  *match;
  
  while (1) {
    line_stat = stdin_read_line();
    line_cnt++;
    if (line_stat == STDIN_ERR_LINE_OVERLEN) return PHSHOW_ERR_LINE_OVERLEN;
    if (line_stat == STDIN_END_REACHED) break;
    stdin_carrige_return();
    while (1) {
      pos = stdin_get_carrige_pos();
      word_stat = stdin_get_word(CurWord);
      if (word_stat == STDIN_ERR_WORD_OVERLEN) return PHSHOW_ERR_WORD_OVERLEN;
      if (word_stat == STDIN_END_REACHED) break;
      if (word_stat == STDIN_IS_WORD) {
        match = comp_cur_word_nameslist();
        if (match != NULL) {
          match_cnt++;
          len = strlen(CurWord);
          printf("%s%zd %zd %zd\033[m\n", match->color, line_cnt, (pos+1), len);
          // ggf. mehr Anzeigen für einzelnes wort
          if (FrmOps.v == true) write_out_verbose(line_cnt, false);
        }
      }
    }
  }

  if (FrmOps.x == true) show_footer(match_cnt);
  if (match_cnt > 0) return PHSHOW_MATCH;
  else return PHSHOW_NO_MATCH;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus convert
// Textzeilen Original, in Wörter zerlegt und allen Codevarianten ausgeben
static int write_out_convert (void)
{
  int      line_stat;
  int      word_stat;
  int      code_cnt;
  phcode_t code;
      
  while (1) {
    line_stat = stdin_read_line();
    if (line_stat == STDIN_ERR_LINE_OVERLEN) return PHSHOW_ERR_LINE_OVERLEN;
    if (line_stat == STDIN_END_REACHED) break;
    // 1. Durchgang Zeile Original ausgeben
    // Einmalig WORD_OVERLEN prüfen, ab 2. Durchgang nicht mehr nötig
    printf("Zeile: ");
    stdin_carrige_return();
    while (1) {
      word_stat = stdin_get_word(CurWord);
      if (word_stat ==STDIN_ERR_WORD_OVERLEN) return PHSHOW_ERR_WORD_OVERLEN;
      if (word_stat == STDIN_END_REACHED) break;
      else printf("%s", CurWord);
    }
    printf("\n");
    // 2. Durchgang Wörter der Zeile ausgeben
    printf("Worte: ");
    stdin_carrige_return();
    while (1) {
      word_stat = stdin_get_word(CurWord);
      if (word_stat == STDIN_END_REACHED) break;
      if (word_stat == STDIN_IS_WORD) printf("%s ", CurWord);
    }
    printf("\n");
    // 3. Durchgang alle Codevarianten ausgeben
    for (code_cnt = 0; code_cnt < NUMBER_OF_PHCODES; code_cnt++) {
      printf("Code%c: ", PhCodeSign[code_cnt]);
      stdin_carrige_return();
      while (1) {
        word_stat = stdin_get_word(CurWord);
        if (word_stat == STDIN_END_REACHED) break;
        if (word_stat == STDIN_IS_WORD) {
          phonetics_get_code (CurWord, code, code_cnt);
          printf("%s ", code);
        }
      }
      printf("\n");
    }
  }  
  
  return PHSHOW_NO_MATCH;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Unterfunktionen zu den Ausgaberoutinen write_out_
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Zeigt die Legende zur Suche an
// Wird von Ausgaberoutinen aufgerufen
// Übergeben werden muß Anzahl der Funde
static void show_footer (const size_t matchcount)
{
  int   cnt;
  bool  show_excludes=false;
  bool  textmode=true; 

  printf("---------------LEGENDE---------------\n");

  //Suchnamen gibt's immer, gleich prüfen ob's auch "Minusnamen" in Liste gibt
  printf("Suche: ");
  for (cnt = 0; cnt < List.number_of_names; cnt++) {
    if (List.items[cnt].exclude == false) {
      printf("%s%s\033[m ", List.items[cnt].color, List.items[cnt].name_norm);
    }
    else show_excludes=true;
  }
  //Verbotene Namen, falls vorhanden
  if (show_excludes == true) {
    printf("\nnicht: ");
    for (cnt = 0; cnt < List.number_of_names; cnt++) {
      if (List.items[cnt].exclude == true) printf("%s ", List.items[cnt].name_norm);
    }
  }
  // Phonetische Suche oder nur Textsuche ?
  for (cnt = 0; cnt < NUMBER_OF_PHCODES; cnt++) {
    if (PhOps.show[cnt] == true) {
      textmode = false;
      break;
    }
  }
  //Optionen
  if (textmode == true) {
    if (PhOps.lev == 0) printf ("\nTyp:   Einfache Textsuche");
    else printf ("\nTyp:   Textsuche mit Levenshtein-Distanz von nicht mehr als %i", PhOps.lev);
  }
  else {
    printf("\nTyp:   Phonetische Suche ");
    for (cnt = 0; cnt < NUMBER_OF_PHCODES; cnt++) {
      if (PhOps.show[cnt] == true) printf("(%s) ", PhCodeName[cnt]);
    }
    if (PhOps.lev != 0) {
      printf("\n       mit Levenshtein-Distanz von nicht mehr als %i im phonetischen Code", PhOps.lev);
    }
  }
  //Funde
  printf("\nFunde: %zd\n", matchcount);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Zeigt genaue Analysedaten an
// Übergabe:  aktuelle Zeilennummer in line_no
//            linemode = true, Verbose Ausgabe für ganze Zeile
//            linemode = false, Verbose Ausgabe nur für Wort was mom. in "CurWord" steht
static void write_out_verbose (const size_t line_no, const bool linemode)
{
  int stat;
  
  printf("  Zeile=%zd\n", line_no);
  if (linemode == false) verbose_word();
  else {
    stdin_carrige_return();
    while (1) {
      stat = stdin_get_word(CurWord);
      // WORD_OVERLEN könnte geliefert werden...
      // Zeile wurde aber schon mal in write_out_ gelesen, hätte da schon Fehler gegeben
      if (stat == STDIN_END_REACHED) break;
      if (stat == STDIN_IS_WORD) verbose_word();
    }
  }
}
// ---------------------------------------------------------------------------------------------------------------------
// Unterfunkton für write_out_verbose
// Verbose Ausgabe für ein Wort
static void verbose_word (void)
{
  int       last_idx;
  int       code_cnt, name_cnt;
  bool      textmode=true;
  word_t    word_upper;
  phcode_t  code[NUMBER_OF_PHCODES];
  int       lev[NUMBER_OF_PHCODES];
  
  // Phonetische Suche oder nur Textsuche ?
  for (code_cnt = 0; code_cnt < NUMBER_OF_PHCODES; code_cnt++) {
    if (PhOps.show[code_cnt] == true) {
      textmode = false;
      break;
    }
  }
  // Einfache Textsuche
  if ((textmode == true) && (PhOps.lev == 0)) {
    printf("    Wort=%s\n", CurWord);
    for (name_cnt = 0; name_cnt < List.number_of_names; name_cnt++) {
      if (List.items[name_cnt].exclude == true) continue;
      strcpy(word_upper, CurWord);
      str_to_upper(word_upper);
      if (strcmp(word_upper, List.items[name_cnt].name_upper) == 0) {
        printf("      %sSuche=%s\033[m\n", List.items[name_cnt].color, List.items[name_cnt].name_norm);
      }
      else printf("      Suche=%s\n", List.items[name_cnt].name_norm);
    }
    return;
  }
  // Einfache Textsuche incl. Lev
  if ((textmode == true) && (PhOps.lev != 0)) {
    printf("    Wort=%s\n", CurWord);
    for (name_cnt = 0; name_cnt < List.number_of_names; name_cnt++) {
      if (List.items[name_cnt].exclude == true) continue;
      printf("      Suche=%s\n", List.items[name_cnt].name_norm);
      strcpy(word_upper, CurWord);
      str_to_upper(word_upper);
      lev[0] = str_lev(word_upper, List.items[name_cnt].name_upper);
      if (lev[0] <= PhOps.lev) printf("        %sLev=%i\033[m\n", List.items[name_cnt].color, lev[0]);
      else printf("        Lev=%i\n", lev[0]);
    }
    return;
  }

  // Phonetische Suche
  // letzten "Plusnamen" aus Liste bestimmen, sonst gibt's keine schöne Grafik
  // Falls der letzte Name ein "Minusname" ist
  last_idx=(List.number_of_names)-1;
  while (1) {
    if (List.items[last_idx].exclude == false) break;
    last_idx--;
  }
  // Wort und dessen Codes in einer Zeile ausgeben
  printf("    Wort=%s\n", CurWord);
  printf("    │ ");
  for (code_cnt = 0; code_cnt < NUMBER_OF_PHCODES; code_cnt++) {
    if (PhOps.show[code_cnt] == true) {
      phonetics_get_code (CurWord, code[code_cnt], code_cnt);
      printf("Code%c=%s ", PhCodeSign[code_cnt], code[code_cnt]);
    }
  }
  printf("\n");
  // Suchnamen ausgeben und Codes in einer Zeile
  for (name_cnt = 0; name_cnt <= last_idx; name_cnt++) {
    if (List.items[name_cnt].exclude == false) {
      // einen Suchnamen ausgeben, Verbindungslinien zu Wort anständig zeichnen
      if (name_cnt < last_idx) printf("    ├─Suche=%s\n", List.items[name_cnt].name_norm);
      else printf("    └─Suche=%s\n", List.items[name_cnt].name_norm);
      if (name_cnt < last_idx) printf("    │ ");
      else printf("      ");
      // Codes zu Suchnamen in eine Zeile
      for (code_cnt = 0; code_cnt < NUMBER_OF_PHCODES; code_cnt++) {
        if (PhOps.show[code_cnt] == true) {
          if (strcmp(List.items[name_cnt].code[code_cnt], code[code_cnt]) == 0) {
            printf("%sCode%c=%s\033[m ", 
              List.items[name_cnt].color, PhCodeSign[code_cnt], List.items[name_cnt].code[code_cnt]);
          }
          else printf("Code%c=%s ", PhCodeSign[code_cnt], List.items[name_cnt].code[code_cnt]);
        }
      }
      printf("\n");
      // ggf. jetzt noch alle Lev-Distanzen in eine Zeile
      if (PhOps.lev != 0) {
        if (name_cnt < last_idx) printf("    │   ");
        else printf("        ");
        for (code_cnt = 0; code_cnt < NUMBER_OF_PHCODES; code_cnt++) {
          lev[code_cnt] = str_lev(List.items[name_cnt].code[code_cnt], code[code_cnt]);
          if (PhOps.show[code_cnt] == true) {
            if (lev[code_cnt] <= PhOps.lev) {
              printf("%sLev%c=%i\033[m ", List.items[name_cnt].color, PhCodeSign[code_cnt], lev[code_cnt]);
            }
            else printf("Lev%c=%i ", PhCodeSign[code_cnt], lev[code_cnt]);
          }
        }
        printf("\n");
      }
    }
  }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Unterfunktionen
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Prüft die ganze Namensliste "List" mit Wort in "CurWord" auf phonetische Übereinstimmung
// Rückgabe: Pointer auf zugehörigen Namen in List wenn eine phonetische Übereinstimmung gefunden wurde
//           Pointer auf NULL wenn keine phonetische Übereinstimmung gefunden wurde
static name_t *comp_cur_word_nameslist (void)
{
  int    cnt;
  name_t *name = List.items;

  // Wort gegen Minusnamen prüfen, Ende bei EXAKTER Übereinstimmung
  // Natürlich mit NULL (Kein Fund), denn genau DIESE Schreibweise soll ja NICHT ausgegeben werden
  for (cnt = 0; cnt < List.number_of_names; cnt++) {
    if (List.items[cnt].exclude == true) {
      if (strcmp(List.items[cnt].name_norm, CurWord) == 0) return NULL;
    }
  }

  // Wort gegen normale Namen prüfen
  for (cnt = 0; cnt < List.number_of_names; cnt++) {
    if (List.items[cnt].exclude == false) {
      if (comp_cur_word_name (name) == true) return name;
    }
    name++;
  }

  // Liste ohne Funde durch
  return NULL;
}
// ---------------------------------------------------------------------------------------------------------------------
// Unterfunktion für "comp_cur_word_nameslist"
// Phonetische Übereinstimmung für einen Namen aus Liste mit CurWord finden
static bool comp_cur_word_name (const name_t *name)
{
  int       lev, cnt;
  phcode_t  code;
  word_t    word_upper;
  bool      textmode=true;

  // Phonetische Suche oder nur Textsuche ?
  for (cnt = 0; cnt < NUMBER_OF_PHCODES; cnt++) {
    if (PhOps.show[cnt] == true) {
      textmode = false;
      break;
    }
  }
  
  // Wenn keine phonetischen Optionen gewählt nur Textvergleich
  // Groß- Kleinschreibung ignorieren ggf. mit Lev-Dist
  if (textmode == true) {
    strcpy(word_upper, CurWord);
    str_to_upper(word_upper);
    if (PhOps.lev == 0) {
      if (strcmp(word_upper, name->name_upper) == 0) return true;
      else return false;
    }
    else {
      lev = str_lev(name->name_upper, word_upper);
      if (lev <= PhOps.lev) return true;
      else return false;
    }
  }

  // Phon Opt gewählt, also Wort in phone Code wandeln
  // mit jeweiligen phon. Code des Namens vergleichen
  // ggf. Lev-Dist auf erzeugten Code anwenden
  for (cnt = 0; cnt < NUMBER_OF_PHCODES; cnt++) {
    if (PhOps.show[cnt] == true) {
      phonetics_get_code (CurWord, code, cnt);
      if (PhOps.lev != 0) {
        lev = str_lev(code, name->code[cnt]);
        if (lev <= PhOps.lev) return true;
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
// Reserviert Speicher für items der Namensliste und füllt mit default
// Übergabe:  Anzahl Namen
// Rückgabe:  false = Fehler Speicherreservierung, sonst true
static bool init_names_list (const int number_of_names)
{
  int     name_cnt, code_cnt;
  name_t  *items;

  items = (name_t*) malloc( number_of_names * sizeof( name_t ));
  if (items == NULL) return false;

  for (name_cnt = 0; name_cnt < number_of_names; name_cnt++) {
    items[name_cnt].exclude=false;
    items[name_cnt].name_norm[0]='\0';
    items[name_cnt].name_upper[0]='\0';
    items[name_cnt].color=Color_Empty;
    for (code_cnt = 0; code_cnt < NUMBER_OF_PHCODES; code_cnt++) {
      items[name_cnt].code[code_cnt][0] = '\0';
    }
  }

  List.number_of_names = number_of_names;
  List.items = items;

  return true;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Zentrale Exitfunktion
// Übergabe:  Statuskonstannte... diese wird ans BS zurückgegeben
//            Gibt bei Fehlerstatus Fehlermeldung aus, bzw. Hilfe / Beispiele
// Gibt ggf. Speicher der Namensliste wieder frei
static void exit_func (const int status_no)
{
  if (List.items != NULL) free(List.items);
  
  if (status_no == PHSHOW_ERR_NAME_UNDERLEN) fprintf(stderr, "phoneshow-de: Name zu kurz zur sinnvollen Suche !\n");
  else if (status_no == PHSHOW_ERR_NAME_OVERLEN) fprintf(stderr, "phoneshow-de: Name zu lang !\n");
  else if (status_no == PHSHOW_ERR_NAME_NOT_GERMAN) fprintf(stderr, "phoneshow-de: Name ungültig !\n");
  else if (status_no == PHSHOW_ERR_MEM) fprintf(stderr, "phoneshow-de: Fehler bei Speicheranforderung !\n");
  else if (status_no == PHSHOW_ERR_LINE_OVERLEN) fprintf(stderr, "phoneshow-de: Suche Abgebrochen !\n"
                                                               "Zeile von stdin hat Überlänge !\n");
  else if (status_no == PHSHOW_ERR_WORD_OVERLEN) fprintf(stderr, "phoneshow-de: Suche Abgebrochen !\n"
                                                               "Wort von stdin hat Überlänge !\n");
  else if (status_no == PHSHOW_ERR_PARAM) {
    fprintf(stderr, "phoneshow-de: Falsche Aufrufparameter !\n"
      "Aufruf:    phoneshow-de\n"
      "Aufruf:    phoneshow-de [Optionen] Name[n] [_Name[n]]\n"
      "Hilfe:     phoneshow-de -h\n"
      "Beispiele: phoneshow-de -b\n\n");
  }
  
  else if (status_no == PHSHOW_PRINT_HELP) show_help();
  else if (status_no == PHSHOW_PRINT_EXAMPLES) show_examples();
      
  exit (status_no);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void show_help (void)
{
  printf(
    "\n\033[1mphoneshow-de:    Phonetische Suche von Namen/Wörtern in Text\033[m\n"
    "Aufruf           phoneshow-de\n"
    "                 Eingabetext von stdin wird weitergeleitet an stdout. Zusätzlich werden alle Wörter\n"
    "                 extrahiert und diese in alle phonetischen Verfahren \"übersetzt\" ausgegeben.\n"
    "---------------------------------------------------------------------------------------------------\n"
    "Aufruf           phoneshow-de [Optionen] Name[n] [_Name[n]]\n"
    "Übergabe         Textdaten von stdin\n"
    "Rückgabe         Textdaten formatiert in stdout\n"
    "Namen            Ein oder mehrere Namen nach denen gesucht werden soll\n"
    "_Namen           Ein oder mehrere Namen die bei der Suche ausgelassen werden sollen\n"
    "Optionen      %s Kölner Phonetik: für den deutschen Sprachraum das Beste\n"
    "Phonetische   %s Phonem: in manchen dBase Versionen enthalten auch gut für deutsche Namen\n"
    "Suchverfahren %s Soundex: ehr für englische Namen, aber Standard im Genealogiebereich\n"
    "              %s Extended Soundex: erweiterte Soundex Variante\n"
    "                 Alle vier Varianten können einzeln oder kombiniert verwendet werden.\n"
    "                 Ohne Parameterangabe zum Suchverfahren ist die phonetische Suche inaktiv.\n"
    "Option        -l Wendet Levenshtein-Filter auf die Worte an, wenn kein phonetisches Verfahren\n"
    "Levenshtein      gewählt ist, oder auf den erzeugten phonetischen Code, wenn angegeben.\n"
    "Distanz          Jedes weitere -l erhöht die zulässige Levenshtein-Distanz um eins.\n"
    "Optionen      -a ganzen Text ausgeben\n"
    "zur Ausgabe   -z nur Fundzeilen ausgeben\n"
    "              -w nur Fundworte ausgeben\n"
    "              -c zu Funden die Positionen innerhalb der Zeilen und Längen ausgeben.\n"
    "                 Fundzeile(1...) Fundposition(1...) Wortlänge(in 'char' gemessen)\n"
    "                 Eine der 4 Ausgabevarianten kann gewählt werden.\n"
    "                 Ohne Parameterangabe zur Ausgabe ist -z aktiv.\n"
    "Verbose Modus -v Textausgabe erfolgt incl. Analyseansicht\n"
    "Nummerierung  -n Zeilennummerierung einschalten\n"
    "Farbe         -f Farbausgabe abschalten         (z.B. bei Weiterleitung in eine Datei)\n"
    "Legende       -x Ausgabe der Legende abschalten (z.B. bei Weiterleitung in eine Datei)\n"
    "Hilfe         -h Zeigt diese Hilfe an\n"
    "Hilfe         -b Zeigt Aufrufbeispiele an\n"
    "---------------------------------------------------------------------------------------------------\n",
    PhParam[PHONETICS_COLOGNE], PhParam[PHONETICS_PHONEM], PhParam[PHONETICS_SOUNDEX], PhParam[PHONETICS_EXSOUNDEX]
  );
  
  printf("Exitcode       %i wenn kein phonetisch ähnlicher Name gefunden wurde\n",
                         PHSHOW_NO_MATCH);
  printf("               %i wenn mindestens ein phonetisch ähnlicher Name gefunden wurde\n",
                         PHSHOW_MATCH);
  printf("               %i wenn Name zu kurz zum sinnvollen codieren\n",
                         PHSHOW_ERR_NAME_UNDERLEN);
  printf("               %i wenn Name zu lang\n",
                         PHSHOW_ERR_NAME_OVERLEN);
  printf("               %i wenn unerlaubte Zeichen im Namen enthalten sind. Erlaubt sind nur einzelne Worte,\n"
         "                 Buchstaben deutsches Alphabet incl. Umlaute. Keine Leerzeichen oder Satzzeichen.\n",
                         PHSHOW_ERR_NAME_NOT_GERMAN);
  printf("               %i wenn falsche Aufrufparameter\n",
                         PHSHOW_ERR_PARAM);
  printf("               %i wenn von stdin Textzeile in Überlänge empfangen wurde\n",
                         PHSHOW_ERR_LINE_OVERLEN);
  printf("               %i wenn von stdin Wort in Überlänge empfangen wurde\n",
                         PHSHOW_ERR_WORD_OVERLEN);
  printf("               %i wenn es bei Speicheranfordeung zu Problemen kam\n\n",
                         PHSHOW_ERR_MEM);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static void show_examples (void)
{
  printf(
    "\n\033[1mphoneshow-de: Phonetische Suche von Namen/Wörtern in Text\033[m\n"
    "Beispiele Suche:\n"
    "\033[7mphoneshow-de Müller\033[m\n"
    "Findet NUR Müller, MÜLLER und müller Zwischen Groß- und Kleinschreibung wird nicht unterschieden.\n"
    "\033[7mphoneshow-de -l Müller\033[m\n"
    "Findet alle Worte die sich in max. einem Buchstaben unterscheiden, einen Buchstaben mehr oder\n"
    "einen weniger haben. Eine phonetische Suche findet hier nicht statt. Findet Mülker Rüller Kmüller\n"
    "\033[7mphoneshow-de -l -l Müller\033[m\n"
    "Jedes -l erhöht die Anzahl in der sich die Namen unterscheiden dürfen. Mit -l -l werden auch Worte\n"
    "gefunden die sich um zwei unterscheiden, also 2 Buchstaben mehr, weniger oder andere haben.\n"
    "Ebenfalls ohne phonetische Suche. Findet Mücker Küllar Müxxer Müllcker\n"
    "Sinnvoll sind max. 2-3 -l Mehr ist möglich aber nicht sehr zweckdienlich.\n"
    "\033[7mphoneshow-de -k Müller\033[m\n"
    "Findet alle Namen die nach Kölner Phonetik ähnlich zu Müller sind:\n"
    "Müller, Mueller, Myller, Müllar, Mülherr, Nüller, Nyller\n"
    "\033[7mphoneshow-de -p Müller\033[m\n"
    "Das selbe aber mit phonetischem Suchverfahren Phonem:\n"
    "Müller, Mueller, Myller, Müllar, Mülherr werden damit gefunden, Nüller und Nyller aber nicht.\n"
    "\033[7mphoneshow-de -k -p -s -e Müller\033[m\n"
    "Wendet alle phonetischen Verfahren an: Kölner Phonetik, Phonem, Soundex, Extended Soundex.\n"
    "Damit werden so ziemlich alle Varianten gefunden.\n"
    "\033[7mphoneshow-de -k -l Müller\033[m\n"
    "Wendet Suchverfahren Kölner Phonetik an und schiebt den erzeugten Code durch ein Levenshtein Filter.\n"
    "Der phonetische Code für Müller ist in dem Fall 657. Nun werden alle Worte gefunden deren Code sich\n"
    "an einer Stelle unterscheidet oder deren Code ein Zeichen mehr oder weniger hat.\n"
    "Somit werden auch Worte gefunden die den Code 667 oder 6578 oder 57 erzeugen. Auch hier gilt:\n"
    "Jedes zusätzliche -l erhört die Zahl an möglichen Abweichungen. 2-3 -l sind OK. mehr sinnlos.\n\n"
    "Name ausschließen mit _Name:\n"
    "Bei der Suche können EXAKTE Schreibweisen ausgeschlossen werden, wenn man ein _ voranstellt.\n"
    "\033[7mphoneshow-de -k -p Müller_Mueller\033[m\n"
    "Zeigt alle Namen die phonetisch ähnlich zu Müller sind, schließ aber die Schreibweise Mueller aus.\n"
    "Bei den verbotenen Namen wird zwischen Groß- und Kleinschreibung unterschieden. Das heiß:\n"
    "Gefunden werden Müller Möller Myller NICHT Mueller ABER mueller schon (weil Kleingeschrieben).\n"
    "\033[7mphoneshow-de Müller_Müller\033[m\n"
    "Zeigt überhaupt nichts weil die phonetische Suche inaktiv ist.\n"
    "\033[7mphoneshow-de -k Müller_Müller\033[m\n"
    "Zeigt alle phonetisch ähnlichen zu Müller, aber nicht die Schreibweise Müller selbst.\n\n"
    "Praxisbeispiele:\n"
    "\033[7mcat datei.txt | phoneshow-de -k -p Udhe\033[m soll uns alle Udhe und ähnliche zeigen.\n"
    "Allerdings findet man auch hunderte Otto, die stören und nicht benötigt werden.\n"
    "\033[7mcat datei.txt | phoneshow-de -k -p Udhe _Otto\033[m\n"
    "Filtert alle Otto aus und wir bekommen weniger Funde die schon ehr zielführend sind.\n"
    "\033[7mcat datei.txt | phoneshow-de -k -p -s -e -n -f Thielemann > fundzeilen.txt\033[m\n"
    "Nutz alle phonetischen Suchverfahren, schaltet Zeilennummerierung ein, Farbe aus und schreibt\n"
    "alle Zeilen in denen sich etwas ähnliches findet nummeriert in fundzeilen.txt.\n"
    "\033[7mcat datei.txt | phoneshow-de -k -p -s -e -w -f -x Meier Müller Schulz | sort | uniq -c\033[m\n"
    "Findet alle phonetisch ähnlichen zu Meier Müller Schulz.\n"
    "Schaltet Ausgabe von Farbe und Legende ab, sodaß lediglich die Funde ausgegeben werden.\n"
    "Leitet durch sort und läßt von uniq zählen, sodaß eine Häufigkeitsliste ausgegeben wird.\n"
    "\033[7mcat datei.txt | phoneshow-de\033[m\n"
    "Gibt Text inclusive aller phonetischer Codierverfahren aus.\n\n"
  );
}
