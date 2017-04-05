#ifndef PHSHOW_TYPES
#define PHSHOW_TYPES

// ---------------------------------------------------------------------------------------------------------------------
// Statuskonstannten Rückgabe von Funktionen
// 0 bis 9 werden bis zu "phoneshow.c" durchgereicht
// und diehnen gleichzeitig als Rückgabe ans Betriebssystem
#define PHSHOW_SUCCESS              0
#define PHSHOW_MATCH                1
#define PHSHOW_NO_MATCH             2
#define PHSHOW_ERR_NAME_UNDERLEN    3
#define PHSHOW_ERR_NAME_OVERLEN     4
#define PHSHOW_ERR_NAME_NOT_GERMAN  5
#define PHSHOW_ERR_PARAM            6
#define PHSHOW_ERR_LINE_OVERLEN     7
#define PHSHOW_ERR_WORD_OVERLEN     8
#define PHSHOW_ERR_MEM              9

// ab 10 für interne Funktionsrückgaben
#define PHSHOW_END_REACHED          10
#define PHSHOW_IS_WORD              11
#define PHSHOW_IS_SPECIAL_CHAR      12
// ---------------------------------------------------------------------------------------------------------------------
// Für je einen Namen aus Programmübergabe
typedef struct {
  bool        is_minusname;         // true wenn auszulassende Schreibweise
  word_t      name_norm;            // Name in Originalschreibweise
  word_t      name_upper;           // Name in ASCII Großbuchstaben
  phcode_t    code_k;               // Phonetische Codes
  phcode_t    code_p;               // des Namens
  phcode_t    code_s;
  phcode_t    code_e;
  const char  *color;               // Pointer auf Farbstring
} name_t;
// ---------------------------------------------------------------------------------------------------------------------
// Für Namensliste
typedef struct {
  int         number_of_names;
  name_t      *items;
} nameslist_t;
// ---------------------------------------------------------------------------------------------------------------------
// Für Phonetik Optionen
typedef struct {
  bool  k;                          // Kölner Phonetik
  bool  p;                          // Phonem Phonetik
  bool  s;                          // Soundex Phonetic
  bool  e;                          // Exsoundex Phonetik
  int   l;                          // erlaubte Lev-Dist
} phops_t;
// ---------------------------------------------------------------------------------------------------------------------
// Für Formatierungsoptionen
typedef struct {
  bool  n;                          // Zeilennummerierung
  bool  x;                          // Ausgabe Legende
  bool  v;                          // Verbose Mode
} frmops_t;
// ---------------------------------------------------------------------------------------------------------------------

#endif //PHSHOW_TYPES
