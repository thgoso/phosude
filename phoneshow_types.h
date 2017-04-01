#ifndef PHONESHOW_TYPES
#define PHONESHOW_TYPES


// Boolean
typedef enum {true, false} bool;

// Für je einen übergebenen Namen
typedef struct {
  bool        is_minusname;                   // true wenn auszulassende Schreibweise
  char        name_norm[BUFFER_SIZE_WORD];    // Name wie aus Programmübergabe
  char        name_lower[BUFFER_SIZE_WORD];   // Name in Kleinbuchstaben
  char        code_k[BUFFER_SIZE_CODE];       // Phonetische Codes
  char        code_p[BUFFER_SIZE_CODE];       // des Namens
  char        code_s[BUFFER_SIZE_CODE];
  char        code_e[BUFFER_SIZE_CODE];
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

