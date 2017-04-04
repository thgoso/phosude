#ifndef PHPACK_TYPES
#define PHPACK_TYPES

// Pufferüberläufe werden abgefangen und Fehlermeldung ausgegeben
// Puffergrößen ggf. einfach erhöhen, sollte aber für normale Textdateien reichen
// Wörter über 64 Zeichen sind ehr selten
#define BUFFSIZE_LINE             4096
#define BUFFSIZE_WORD             64
// Kölner Phonetik erzeugt im Schlimmsten Fall
// Einen Code der doppelt so lang ist wie das Übergabewort
// Daher Verhältniss beibehalten
#define BUFFSIZE_CODE             (BUFFSIZE_WORD * 2)

// passend dazu Feststringtypen
// Für einen Namen / Wort
typedef struct {
  char s[BUFFSIZE_WORD];
} word_t;

// Für einen phonetischen Code
typedef struct {
  char s[BUFFSIZE_CODE];
} code_t;

// Boolean
typedef enum {true, false} bool;

// Für je einen übergebenen Namen aus Programmübergabe
typedef struct {
  bool        is_minusname;       // true wenn auszulassende Schreibweise
  word_t      name_norm;          // Name in Originalschreibweise
  word_t      name_upper;         // Name in ASCII Großbuchstaben
  code_t      codek;              // Phonetische Codes
  code_t      codep;              // des Namens
  code_t      codes;
  code_t      codee;
  const char  *color;             // Pointer auf Farbstring
} name_t;

// Für Namensliste
typedef struct {
  int         number_of_names;
  name_t      *items;
} nameslist_t;

// Für Phonetik Optionen
typedef struct {
  bool  k;                        // Kölner Phonetik
  bool  p;                        // Phonem Phonetik
  bool  s;                        // Soundex Phonetic
  bool  e;                        // Exsoundex Phonetik
  int   l;                        // erlaubte Lev-Dist
} phops_t;

// Für Formatierungsoptionen
typedef struct {
  bool  n;                        // Zeilennummerierung
  bool  x;                        // Ausgabe Legende
  bool  v;                        // Verbose Mode
} frmops_t;


#endif //PHPACK_TYPES
