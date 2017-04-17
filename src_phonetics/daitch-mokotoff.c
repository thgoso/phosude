/*
phosude daitch-mokotoff.c
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
#include "daitch-mokotoff.h"

// Codierungsregeln
// Regel ohne "/"    ... Es gibt nur eine Variante
// Regel enthält "/" ... Es gibt 2 Varianten
// Regel ist ""      ... Keine Codierung
#define NUMBER_OF_RULES 115

static const struct {
  char  phrase[8];
  char  rule_start[8];
  char  rule_before_vowel[8];
  char  rule_else[8];
} Rules[NUMBER_OF_RULES] = {
  // Phrase   Starregel vor Vokal  Sonst
  {  "AI",      "0",      "1",      ""      },
  {  "AJ",      "0",      "1",      ""      },
  {  "AY",      "0",      "1",      ""      },
  {  "AU",      "0",      "7",      ""      },
  {  "A",       "0",      "",       ""      },
  {  "B",       "7",      "7",      "7"     },
  {  "CHS",     "5",      "54",     "54"    },
  {  "CSZ",     "4",      "4",      "4"     },
  {  "CZS",     "4",      "4",      "4"     },
  {  "CH",      "5/4",    "5/4",    "5/4"   },
  {  "CK",      "5/45",   "5/45",   "5/45"  },
  {  "CZ",      "4",      "4",      "4"     },
  {  "CS",      "4",      "4",      "4"     },
  {  "C",       "5/4",    "5/4",    "5/4"   },
  {  "DRZ",     "4",      "4",      "4"     },
  {  "DRS",     "4",      "4",      "4"     },
  {  "DSH",     "4",      "4",      "4"     },
  {  "DSZ",     "4",      "4",      "4"     },
  {  "DZH",     "4",      "4",      "4"     },
  {  "DZS",     "4",      "4",      "4"     },
  {  "DS",      "4",      "4",      "4"     },
  {  "DZ",      "4",      "4",      "4"     },
  {  "DT",      "3",      "3",      "3"     },
  {  "D",       "3",      "3",      "3"     },
  {  "EI",      "0",      "1",      ""      },
  {  "EJ",      "0",      "1",      ""      },
  {  "EY",      "0",      "1",      ""      },
  {  "EU",      "1",      "1",      ""      },
  {  "E",       "0",      "",       ""      },
  {  "FB",      "7",      "7",      "7"     },
  {  "F",       "7",      "7",      "7"     },
  {  "G",       "5",      "5",      "5"     },
  {  "H",       "5",      "5",      ""      },
  {  "IA",      "1",      "",       ""      },
  {  "IE",      "1",      "",       ""      },
  {  "IO",      "1",      "",       ""      },
  {  "IU",      "1",      "",       ""      },
  {  "I",       "0",      "",       ""      },
  {  "J",       "1/4",    "1/4",    "1/4"   },
  {  "KS",      "5",      "54",     "54"    },
  {  "KH",      "5",      "5",      "5"     },
  {  "K",       "5",      "5",      "5"     },
  {  "L",       "8",      "8",      "8"     },
  {  "MN",      "66",     "66",     "66"    },
  {  "M",       "6",      "6",      "6"     },
  {  "NM",      "66",     "66",     "66"    },
  {  "N",       "6",      "6",      "6"     },
  {  "OI",      "0",      "1",      ""      },
  {  "OJ",      "0",      "1",      ""      },
  {  "OY",      "0",      "1",      ""      },
  {  "O",       "0",      "",       ""      },
  {  "PF",      "7",      "7",      "7"     },
  {  "PH",      "7",      "7",      "7"     },
  {  "P",       "7",      "7",      "7"     },
  {  "Q",       "5",      "5",      "5"     },
  {  "RZ",      "94/4",   "94/4",   "94/4"  },
  {  "RS",      "94/4",   "94/4",   "94/4"  },
  {  "R",       "9",      "9",      "9"     },
  {  "SCHTSCH", "2",      "4",      "4"     },
  {  "SCHTSH",  "2",      "4",      "4"     },
  {  "SCHTCH",  "2",      "4",      "4"     },
  {  "STSCH",   "2",      "4",      "4"     },
  {  "SHTSH",   "2",      "4",      "4"     },
  {  "SHTCH",   "2",      "4",      "4"     },
  {  "SHCH",    "2",      "4",      "4"     },
  {  "STCH",    "2",      "4",      "4"     },
  {  "STRZ",    "2",      "4",      "4"     },
  {  "STRS",    "2",      "4",      "4"     },
  {  "STSH",    "2",      "4",      "4"     },
  {  "SZCZ",    "2",      "4",      "4"     },
  {  "SZCS",    "2",      "4",      "4"     },
  {  "SCHT",    "2",      "43",     "43"    },
  {  "SCHD",    "2",      "43",     "43"    },
  {  "SHT",     "2",      "43",     "43"    },
  {  "SZT",     "2",      "43",     "43"    },
  {  "SHD",     "2",      "43",     "43"    },
  {  "SZD",     "2",      "43",     "43"    },
  {  "SCH",     "4",      "4",      "4"     },
  {  "SH",      "4",      "4",      "4"     },
  {  "SC",      "2",      "4",      "4"     },
  {  "ST",      "2",      "43",     "43"    },
  {  "SD",      "2",      "43",     "43"    },
  {  "SZ",      "4",      "4",      "4"     },
  {  "S",       "4",      "4",      "4"     },
  {  "TSCH",    "4",      "4",      "4"     },
  {  "TCH",     "4",      "4",      "4"     },
  {  "TRS",     "4",      "4",      "4"     },
  {  "TRZ",     "4",      "4",      "4"     },
  {  "TSH",     "4",      "4",      "4"     },
  {  "TZS",     "4",      "4",      "4"     },
  {  "TSZ",     "4",      "4",      "4"     },
  {  "TC",      "4",      "4",      "4"     },
  {  "TS",      "4",      "4",      "4"     },
  {  "TZ",      "4",      "4",      "4"     },
  {  "TH",      "3",      "3",      "3"     },
  {  "T",       "3",      "3",      "3"     },
  {  "UI",      "0",      "1",      ""      },
  {  "UJ",      "0",      "1",      ""      },
  {  "UY",      "0",      "1",      ""      },
  {  "UE",      "0",      "",       ""      },
  {  "U",       "0",      "",       ""      },
  {  "V",       "7",      "7",      "7"     },
  {  "W",       "7",      "7",      "7"     },
  {  "X",       "5",      "54",     "54"    },
  {  "Y",       "1",      "",       ""      },
  {  "ZHDZH",   "2",      "4",      "4"     },
  {  "ZDZH",    "2",      "4",      "4"     },
  {  "ZSCH",    "4",      "4",      "4"     },
  {  "ZSH",     "4",      "4",      "4"     },
  {  "ZHD",     "2",      "43",     "43"    },
  {  "ZDZ",     "2",      "4",      "4"     },
  {  "ZD",      "2",      "43",     "43"    },
  {  "ZH",      "4",      "4",      "4"     },
  {  "ZS",      "4",      "4",      "4"     },
  {  "Z",       "4",      "4",      "4"     }
};

// Für ein Sammlung von Codevarianten
typedef struct {
  int   number_of_codes;
  char  version[64][8];
} collection_t;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Private Funktionen
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Entpackt einen Codestring vom Format "daitchmok_t" in ein Array "collection_t"
static void daitchmok_to_collection (const daitchmok_t src, collection_t *dest)
{
  daitchmok_t tmp;
  char        *token;
  int         cnt=0;
  
  strcpy(tmp, src);
  token = strtok(tmp, "/");
  while (token != NULL) {
    strcpy(dest->version[cnt], token);
    cnt++;
    token = strtok(NULL, "/");
  }
  dest->number_of_codes = cnt;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Fügt EINE Codestringversion EINEM Element in Codearray zu
// Codestring kann aus einem oder zwei Ziffern bestehen
// Hilfsfunktion zu "collection_add_string"
static inline void collection_add_version (const char *s, collection_t *dest, const int elem_no)
{
  size_t  len;
  
  // Ende wenn Element schon voll
  len = strlen(dest->version[elem_no]);
  if (len >= 6) return;

  // 1. Zeichen einfügen
  dest->version[elem_no][len] = s[0];
  dest->version[elem_no][len+1] = '\0';
  
  // 2. Zeichen wenn vorhanden und Platz
  if ((len <= 4) && (s[1] != '\0')) {
    dest->version[elem_no][len+1] = s[1];
    dest->version[elem_no][len+2] = '\0';
  }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Fügt dem Codearray einen String zu
// 1. Übergabestring ist leer, Ende ohne Änderung
// 2. Übergabestring enthält eine Regel z.B. "4"
//    An jeden String im Array wird eine "4" angehangen
// 3. Übergabestring enthält zwei Regeln z.B. "4/54"
//    Vorhandene Strings im Array werden verdoppelt
//    an die erste Hälfte wird eine "4" an die zweite Hälfte eine "54" angehangen
// An einen String werden nur Zeichen angefügt, so lange dieser noch nicht voll ist (6Stellen)
static void collection_add_string (const char *s, collection_t *dest)
{
  char  vgem[8];
  char  *token;
  char  v1[4] = "";
  char  v2[4] = "";
  int   cnt, read_cnt, write_cnt;

  if (s[0] == '\0') return;
    
  // 1. Variante extrahieren nach v1
  // Mögliche zweite Variante nach v2 (oder leer lassen) 
  strcpy (vgem, s);
  token = strtok(vgem, "/");
  token = strtok(NULL, "/");
  if (token == NULL) strcpy(v1, vgem);
  else {
    strcpy(v1, vgem);
    strcpy(v2, token);
  }

  // Noch keine Elemente in dest.code... 
  // eine oder zwei Versionen zufügen, Anzahl korrigieren, Ende
  if (dest->number_of_codes == 0) {
    if (v2[0] == '\0') {
      strcpy(dest->version[0], v1);
      dest->number_of_codes = 1;
    }
    else {
      strcpy(dest->version[0], v1);
      strcpy(dest->version[1], v2);
      dest->number_of_codes = 2;
    }
    return;
  }
  
  // Schon Elemente in dest.code... Eine Version zufügen, Ende
  if (v2[0] == '\0') {
    for (cnt = 0; cnt < dest->number_of_codes; cnt++) collection_add_version (v1, dest, cnt);
    return;
  }
  
  // Schon Elemente in dest.code... Zwei Versionen zufügen... Listeneinträge verdoppeln
  // Version1 in erster Hälfte Version2 in zweite Hälfte zufügen, Ende
  write_cnt = dest->number_of_codes;
  for (read_cnt = 0; read_cnt < dest->number_of_codes; read_cnt++) {
    strcpy(dest->version[write_cnt], dest->version[read_cnt]);
    write_cnt++;
  }
  dest->number_of_codes *=2;
  for (cnt = 0; cnt < (dest->number_of_codes / 2); cnt++) collection_add_version (v1, dest, cnt);
  for (; cnt < dest->number_of_codes; cnt++) collection_add_version (v2, dest, cnt);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Alle Codevarianten temporär in Array erstellen
// Am Ende zu Einzelstring zusammengefasst in dest zurückgeben
void make_daitchmok (const word_t src, daitchmok_t dest)
{
  collection_t  list;
  word_t        tmp;
  char          *p;
  int           rule_cnt, write_cnt, read_cnt;
  char          item[8];
   
  // src nach tmp kopieren, in ASCII Groß wandeln, Doppelte Buchstaben gleich vereinzeln
  strcpy(tmp, src);
  str_to_ascii_upper(tmp);
  str_del_multiple_chars(tmp);
  
  // list "0"en, p zeigt auf Start von tmp (diehnt als Lesezeiger)
  list.number_of_codes = 0;
  p = tmp;

NextPrase:
  if (*p == '\0') goto Done;
  // Loop über alle Regeln
  for (rule_cnt = 0; rule_cnt < NUMBER_OF_RULES; rule_cnt++) {
    // Passende Phrase gefunden
    if (strncmp(p, Rules[rule_cnt].phrase, (strlen(Rules[rule_cnt].phrase))) == 0) {
      // Wortstart... rule_start anwenden
      if (p == tmp) {
        collection_add_string (Rules[rule_cnt].rule_start, &list);
        p+=strlen(Rules[rule_cnt].phrase);
        goto NextPrase;
      }
      // Kein Wortstart nächsten Buchstaben nach phrase prüfen
      else {
        p+=strlen(Rules[rule_cnt].phrase);
        // rule_before_vowel anwenden
        // Recherche Internet sagt: Vowel meint bei Daitch-Mokotoff AEIOU UND Y
        if ((*p == 'A') || (*p == 'E') || (*p == 'I') || (*p == 'O') || (*p == 'U') || (*p == 'Y')) {
          collection_add_string (Rules[rule_cnt].rule_before_vowel, &list);
        }
        // rule_else anwenden
        else collection_add_string (Rules[rule_cnt].rule_else, &list);
        goto NextPrase;
      }
    }
  }
  // Keine Phrase aus rules gefunden... nur einen Buchstaben weiter
  p++;
  goto NextPrase;  

Done:
  // Codes auf 6 Stellen mit "0"en füllen
  for (read_cnt = 0; read_cnt < list.number_of_codes; read_cnt++) {
    while (strlen(list.version[read_cnt]) < 6) strcat(list.version[read_cnt], "0");
  }

  // Codes können ggf. doppelt vorhanden sein... sortieren
  for (read_cnt = 0; read_cnt < list.number_of_codes; read_cnt++) {
    for (write_cnt = 0; write_cnt < (list.number_of_codes - 1); write_cnt++) {
      if (strcmp(list.version[read_cnt], list.version[write_cnt]) < 0) {
        strcpy(item, list.version[read_cnt]);
        strcpy(list.version[read_cnt], list.version[write_cnt]);
        strcpy(list.version[write_cnt], item);
      }
    }
  }
    
  // Alles aus list in dest kopieren, doppelte auslassen
  item[0]='\0';
  dest[0]='\0';
  for (read_cnt = 0; read_cnt < list.number_of_codes; read_cnt++) {
    if (strcmp(item, list.version[read_cnt]) != 0) {
      strcpy(item, list.version[read_cnt]);
      strcat(dest, item);
      strcat(dest, "/");
      continue;
    }
  }
  dest[(strlen(dest)-1)] = '\0';
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool comp_word_daitchmok (const word_t word, const daitchmok_t code, int *lev)
{
  daitchmok_t   tmp;
  collection_t  codes_word, codes_input;
  int           cnt_wc, cnt_ic;
  int           cur_dist;
  int           min_dist=255;
  
  // Übergabewort und Übergabecode in Array Typ collection_t wandeln
  make_daitchmok (word, tmp);
  daitchmok_to_collection (tmp, &codes_word);
  daitchmok_to_collection (code, &codes_input);
  
  // Normaler Textvergleich über alle Codes wenn *lev = 0 in Übergabe
  // Raus sobald Fund
  if (*lev == 0) {
    for (cnt_wc = 0; cnt_wc < codes_word.number_of_codes; cnt_wc++) {
      for (cnt_ic = 0; cnt_ic < codes_input.number_of_codes; cnt_ic++) {
        if (strcmp(codes_input.version[cnt_ic], codes_word.version[cnt_wc]) == 0) return true;
      }
    }
    return false;
  }
  
  // ALLE Codes per Lev-Dist Vergleichen wenn *lev in Übergabe > 0
  // Minimalste Distanz speichern
  for (cnt_wc = 0; cnt_wc < codes_word.number_of_codes; cnt_wc++) {
    for (cnt_ic = 0; cnt_ic < codes_input.number_of_codes; cnt_ic++) {
      cur_dist = str_lev(codes_input.version[cnt_ic], codes_word.version[cnt_wc]);
      if (cur_dist < min_dist) min_dist = cur_dist;
    }
  }
  
  // Geringste Dist zwischen allen Codevarianten in *lev zurückgeben
  if (min_dist <= *lev) {
    *lev = min_dist;
    return true;
  }
  else {
    *lev = min_dist;
    return false;
  }
}
