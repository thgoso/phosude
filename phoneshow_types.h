#ifndef PHONESHOW_TYPES
#define PHONESHOW_TYPES

// Zentrale Definitionen/Deklarationen für phoneshow

// Rückgabekonstannten von Funktionen
// 0...9 werden ggf. bis ans Betriebssystem durchgereicht
#define PHS_MATCH                   0
#define PHS_NO_MATCH                1
#define PHS_ERR_NAME_UNDERSIZE      2
#define PHS_ERR_NAME_OVERSIZE       3
#define PHS_ERR_NAME_NOT_GERMAN     4
#define PHS_ERR_PARAM               5
#define PHS_ERR_REC_LINE_OVERSIZE   6
#define PHS_ERR_REC_WORD_OVERSIZE   7
#define PHS_ERR_MEM                 8

#define PHS_REC_COMPLEETE           10
#define PHS_REC_LINE_SUCCESS        11
#define PHS_LINE_END_RACHED         12
#define PHS_IS_WORD                 13
#define PHS_IS_SPECIAL_CHAR         14

// Boolean
typedef enum {true, false} bool;

// Für je einen übergebenen Namen
typedef struct {
  bool        is_minusname;                   // true wenn auszulassende Schreibweise
  char        name_norm[BUFFER_SIZE_WORD];    // Name in Originalschreibweise
  char        name_lower[BUFFER_SIZE_WORD];   // Name in Kleinbuchstaben
  char        code_k[BUFFER_SIZE_WORD];       // Phonetische Codes
  char        code_p[BUFFER_SIZE_WORD];       // des Namens
  char        code_s[BUFFER_SIZE_WORD];
  char        code_e[BUFFER_SIZE_WORD];
  const char  *color_string;                  // Pointer auf Farbstring
} name_t;

// Für Namensliste
typedef struct {
  int         number_of_names;
  name_t      *items;
} nameslist_t;

// Für Phonetik Optionen
typedef struct {
  bool  k;                                    // Kölner Phonetik
  bool  p;                                    // Phonem Phonetik
  bool  s;                                    // Soundex Phonetic
  bool  e;                                    // Exsoundex Phonetik
  int   l;                                    // erlaubte Lev-Dist
} phops_t;

// Für Formatierungsoptionen
typedef struct {
  bool  n;                                    // Zeilennummerierung
  bool  x;                                    // Ausgabe Legende
  bool  v;                                    // Verbose Mode
} frmops_t;


#endif //PHONESHOW_TYPES
