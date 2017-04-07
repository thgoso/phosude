#ifndef PHONEPACK_PHONESHOW
#define PHONEPACK_PHONESHOW

// ---------------------------------------------------------------------------------------------------------------------
// Definitionen, Deklarationen für "phoneshow.c"
// ---------------------------------------------------------------------------------------------------------------------
// Rückgabekonstannten ans Betriebssystem
#define PHSHOW_NO_MATCH             0
#define PHSHOW_MATCH                1
#define PHSHOW_ERR_NAME_UNDERLEN    2
#define PHSHOW_ERR_NAME_OVERLEN     3
#define PHSHOW_ERR_NAME_NOT_GERMAN  4
#define PHSHOW_ERR_PARAM            5
#define PHSHOW_ERR_LINE_OVERLEN     6
#define PHSHOW_ERR_WORD_OVERLEN     7
#define PHSHOW_ERR_MEM              8
#define PHSHOW_PRINT_HELP           10
#define PHSHOW_PRINT_EXAMPLES       11
// ---------------------------------------------------------------------------------------------------------------------
// Farbstringtabelle zur bunten Ausgabe
#define NUMBER_OF_COLORS            28
static const char Color_Table[NUMBER_OF_COLORS][16] = {
  {"\033[1;37m\033[41m"}, {"\033[1;37m\033[42m"}, {"\033[1;37m\033[43m"}, {"\033[1;37m\033[44m"},
  {"\033[1;37m\033[45m"}, {"\033[1;37m\033[46m"}, {"\033[1;37m\033[47m"}, {"\033[1;33m\033[41m"},
  {"\033[1;33m\033[42m"}, {"\033[1;33m\033[43m"}, {"\033[1;33m\033[44m"}, {"\033[1;33m\033[45m"},
  {"\033[1;33m\033[46m"}, {"\033[1;33m\033[47m"}, {"\033[1;36m\033[41m"}, {"\033[1;36m\033[42m"},
  {"\033[1;36m\033[43m"}, {"\033[1;36m\033[44m"}, {"\033[1;36m\033[45m"}, {"\033[1;36m\033[46m"},
  {"\033[1;36m\033[47m"}, {"\033[1;32m\033[41m"}, {"\033[1;32m\033[42m"}, {"\033[1;32m\033[43m"},
  {"\033[1;32m\033[44m"}, {"\033[1;32m\033[45m"}, {"\033[1;32m\033[46m"}, {"\033[1;32m\033[47m"}
};
// Farbstring bei unbunter Ausgabe
static const char Color_Empty[]="";
// ---------------------------------------------------------------------------------------------------------------------
// Boolean
typedef enum {true, false} bool;
// ---------------------------------------------------------------------------------------------------------------------
// Anzahl der Codierverfahren in "phonetics.h"
// Stringdefinitionen zur Ausgabe
// Index passend zur Codenummer in "phonetics.h"
#define NUMBER_OF_PHCODES   4
static const char PhCodeSign[NUMBER_OF_PHCODES] = {'K', 'P', 'S', 'E'};
static const char PhCodeName[NUMBER_OF_PHCODES][24] = { 
  "Kölner Phonetik", "Phonem", "Soundex", "Extended Soundex"
};
// Stringdefinitionen zum Vergleich mit Übergabeparameter (Index wieder passend)
static const char PhParam[NUMBER_OF_PHCODES][3] ={"-k", "-p", "-s", "-e"};
// ---------------------------------------------------------------------------------------------------------------------
// Für je einen Namen aus Programmübergabe
typedef struct {
  bool        exclude;                  // true wenn auszulassende Schreibweise
  word_t      name_norm;                // Name in Originalschreibweise
  word_t      name_upper;               // Name in ASCII Großbuchstaben
  const char  *color;                   // Pointer auf Farbstring
  phcode_t    code[NUMBER_OF_PHCODES];  // Index passend zu Codenummer aus "phonetics.h"
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
  int   lev;                            // erlaubte Lev-Dist
  bool  show[NUMBER_OF_PHCODES];        // Codeverfahren anzeigen = true, nicht = false
} phops_t;                              // Index passend zu Codenummer aus "phonetics.h"
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
  bool  f;                              // Farbige Ausgabe
} outops_t;
// ---------------------------------------------------------------------------------------------------------------------
// Hauptfunktionen Ausgabe
static int write_out_a (void);
static int write_out_z (void);
static int write_out_w (void);
static int write_out_c (void);
static int write_out_convert (void);
// ---------------------------------------------------------------------------------------------------------------------
// Unterfunktionen Ausgabe
static void show_footer (const size_t matchcount);
static void write_out_verbose (const size_t line_no, const bool linemode);
static void verbose_word (void);
// ---------------------------------------------------------------------------------------------------------------------
// Rest
static void show_help (void);
static void show_examples (void);
static void exit_func (const int status_no);
static bool init_names_list (const int number_of_names);
static bool comp_cur_word_name (const name_t *name);
static name_t *comp_cur_word_nameslist (void);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHONEPACK_PHONESHOW
