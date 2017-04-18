#ifndef PHOSUDE
#define PHOSUDE

// ---------------------------------------------------------------------------------------------------------------------
// Wird alles global benötigt
#include <stdbool.h>
#include "src_phonetics/string.h"
#include "src_phonetics/cologne.h"
#include "src_phonetics/soundex.h"
#include "src_phonetics/exsoundex.h"
#include "src_phonetics/phonem.h"
#include "src_phonetics/caverphone.h"
#include "src_phonetics/daitch-mokotoff.h"
// ---------------------------------------------------------------------------------------------------------------------
// Statuskonstannten Rückgabe von Funktionen bis ins Main und weiter ans BS (0-9 nutzen)
#define STAT_SUCCESS                0   // Fehlerfreier Durchlauf
#define STAT_ERR_NAME_UNDERLEN      1   // Übergebener Name ist zu kurz
#define STAT_ERR_NAME_OVERLEN       2   // Übergebener Name ist zu lang
#define STAT_ERR_NAME_NO_WORD       3   // Übergebener Name ist kein deutsches Wort
#define STAT_ERR_PARAM              4   // Fehlerhafte Übergabeparameter von Commandline
#define STAT_ERR_REC_OVERLEN_LINE   5   // Zeile mit Überlänge von stdin empfangen
#define STAT_ERR_REC_OVERLEN_WORD   6   // Wort mit Überlänge von stdin empfangen
#define STAT_ERR_HELP               7   // Parameter -h wurde übergeben (Hilfe anzeigen)
// ---------------------------------------------------------------------------------------------------------------------
// Rückgabekonstannten Funktionen die nicht bis zum BS laufen (ab 10 nutzen)
#define STAT_IS_WORD                10  // Beim lesen von stdin gültiges Wort erhalten
#define STAT_IS_SPECIAL_CHAR        11  // Beim lesen von stdin Sonderzeichen erhalten
#define STAT_END_REACHED            12  // Beim lesen von stdin Zeilenende/Übertragungsende erreicht
// ---------------------------------------------------------------------------------------------------------------------
// Für je einen Namen aus Programmübergabe
typedef struct {
  bool          exclude;                // true wenn auszulassende Schreibweise
  word_t        name_norm;              // Name in Originalschreibweise
  word_t        name_upper;             // Name in ASCII Großbuchstaben
  const char    *color;                 // Pointer auf Farbstring
  cologne_t     code_k;                 // Phonetische Codes
  phonem_t      code_p;                 // des Namens
  soundex_t     code_s;
  exsoundex_t   code_e;
  caverphone_t  code_c;
  daitchmok_t   code_d;
} name_t;
// ---------------------------------------------------------------------------------------------------------------------
// Für Namensliste
typedef struct {
  int     number_of_names;              // Anzahl Namen in Liste
  name_t  *items;                       // Namen items
} nameslist_t;
// ---------------------------------------------------------------------------------------------------------------------
// Für Phonetik Optionen
typedef struct {
  int   lev;                            // erlaubte Lev-Dist (ist 0 wenn Option nicht übergeben)
  int   min;                            // erlaubte Mindestanzahl Buchstaben (ist 0 wenn Option nicht übergeben)
  int   max;                            // erlaubte Höchstanzahl Buchstaben (ist 0 wenn Option nicht übergeben)
  bool  show_k;                         // true wenn Codierverfahren gewählt
  bool  show_p;                         // sonst false
  bool  show_s;
  bool  show_e;
  bool  show_c;
  bool  show_d;
  bool  textmode;                       // true wenn ALLE show_x == false
} phops_t;
// ---------------------------------------------------------------------------------------------------------------------
// Für Formatierungsoptionen
typedef struct {
  bool  n;                              // Zeilennummerierung
  bool  x;                              // Ausgabe Legende
  bool  v;                              // Verbose Mode
} frmops_t;
// ---------------------------------------------------------------------------------------------------------------------
// Für Ausgabeoptionen
typedef struct {
  bool  a;                              // Alles ausgeben
  bool  z;                              // Zeilen ausgeben
  bool  w;                              // Wörter ausgeben
  bool  c;                              // Wortpositionen / Längen ausgeben
} outops_t;
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHOSUDE
