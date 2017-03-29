#ifndef PHONESHOW
#define PHONESHOW


// Statuskonstannten / Fehlercodes
#define STAT_FOUND            0   // Es wurde Übereinstimmung im Text gefunden
#define STAT_NO_FOUND         1   // Es wurde keine Übereinstimmung im Text gefunden
#define STAT_TO_SHORT         2   // Name/Wort zur kurz zum sinnvollen Codieren
#define STAT_TO_LONG          3   // Name/Wort zu Lang
#define STAT_NO_CODE          4   // Name/Wort erzeugt keinen phonetischen Code
                                  // oder ist ungültig (enthält Sonderzeichen usw.)
#define STAT_PARAM_ERR        5   // ungültige Übergabeparameter
#define STAT_STDIN_ERR        6   // Überlange Textzeilen von stdin erhalten
#define STAT_MEM_ERR          7   // Fehler bei Speicheranforderung
#define STAT_SHOW_EXAMPLES  254   // Beispiele anzeigen
#define STAT_SHOW_HELP      255   // Hilfe angezeigen

// Puffergröße lesen von stdin, Wenn Textzeilen größerer Länge eintreffen, wird Fehler ausgelöst
#define LINE_BUFFER_SIZE   4096

// Farbstrings
#define COLORSTRING_SIZE     16
#define NUMBER_OF_COLORS     28
static const char color_table[NUMBER_OF_COLORS][COLORSTRING_SIZE] = {
  {"\033[1;37m\033[41m"}, {"\033[1;37m\033[42m"}, {"\033[1;37m\033[43m"}, {"\033[1;37m\033[44m"},
  {"\033[1;37m\033[45m"}, {"\033[1;37m\033[46m"}, {"\033[1;37m\033[47m"}, {"\033[1;33m\033[41m"},
  {"\033[1;33m\033[42m"}, {"\033[1;33m\033[43m"}, {"\033[1;33m\033[44m"}, {"\033[1;33m\033[45m"},
  {"\033[1;33m\033[46m"}, {"\033[1;33m\033[47m"}, {"\033[1;36m\033[41m"}, {"\033[1;36m\033[42m"},
  {"\033[1;36m\033[43m"}, {"\033[1;36m\033[44m"}, {"\033[1;36m\033[45m"}, {"\033[1;36m\033[46m"},
  {"\033[1;36m\033[47m"}, {"\033[1;32m\033[41m"}, {"\033[1;32m\033[42m"}, {"\033[1;32m\033[43m"},
  {"\033[1;32m\033[44m"}, {"\033[1;32m\033[45m"}, {"\033[1;32m\033[46m"}, {"\033[1;32m\033[47m"}
};
static const char color_empty[]="";

// Boolean
typedef enum {true, false} bool;

// Für je einen übergebenen Namen
typedef struct {
  bool        is_minusname;                   // true wenn auszulassende Schreibweise
  char        name_norm[BUFFER_WORD_SIZE];    // Name wie aus Programmübergabe
  char        name_lower[BUFFER_WORD_SIZE];   // Name in Kleinbuchstaben
  char        code_k[BUFFER_CODE_SIZE];       // Phonetische Codes
  char        code_p[BUFFER_CODE_SIZE];       // des Namens
  char        code_s[BUFFER_CODE_SIZE];
  char        code_e[BUFFER_CODE_SIZE];
  const char  *p_color;                       // Pointer auf String aus obiger Farbtabelle bzw. auf color_empty
} name_t;

// Für Optionen Phonetik & Formatierung (true=Funktion ein)
typedef struct {
  bool  k;                                    // Kölner Phonetik
  bool  p;                                    // Phonem Phonetik
  bool  s;                                    // Soundex Phonetic
  bool  e;                                    // Exsoundex Phonetik
  bool  n;                                    // Zeilennummerierung
  bool  f;                                    // Farbe
  bool  x;                                    // Ausgabe Legende
	int   l;                                    // erlaubte Lev-Dist
} phoneops_t;

// Für Ausgabeformat (true=Funktion ein)
typedef struct {          
  bool  a;																		// Alles ausgeben
  bool  z;																		// Nur Fundzeilen ausgeben
  bool  w;																		// Nur Fundworte ausgeben
} outops_t;




#endif //PHONESHOW



