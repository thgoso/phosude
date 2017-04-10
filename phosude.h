#ifndef PHOSUDE
#define PHOSUDE

// ---------------------------------------------------------------------------------------------------------------------
// Statuskonstannten Rückgabe von Funktionen bis ins Main und weiter ans BS (0-9 nutzen)
#define STAT_SUCCESS                      0   // Fehlerfreier Durchlauf
#define STAT_ERR_UNDERLEN_WORD            1   // Übergebener Name ist zu kurz
#define STAT_ERR_OVERLEN_WORD             2   // Übergebener Name ist zu lang
#define STAT_ERR_NO_WORD                  3   // Übergebener Name ist kein deutsches Wort
#define STAT_ERR_PARAM                    4   // Fehlerhafte Übergabeparameter von Commandline
#define STAT_ERR_REC_OVERLEN_LINE         5   // Zeile mit Überlänge von stdin empfangen
#define STAT_ERR_REC_OVERLEN_WORD         6   // Wort mit Überlänge von stdin empfangen
#define STAT_ERR_MEM                      7   // Fehler beim Speicher anfordern
#define STAT_ERR_HELP                     8   // Parameter -h wurde übergeben (Hilfe anzeigen)
// ---------------------------------------------------------------------------------------------------------------------
// Rückgabekonstannten Funktionen die nicht bis zum BS laufen (ab 10 nutzen)
#define STAT_IS_WORD                      10  // Beim lesen von stdin gültiges Wort erhalten
#define STAT_IS_SPECIAL_CHAR              11  // Beim lesen von stdin Sonderzeichen erhalten
#define STAT_END_REACHED                  12  // Beim lesen von stdin Zeilenende/Übertragungsende erreicht
// ---------------------------------------------------------------------------------------------------------------------
// Phonetische Verfahren
#define PH_COLOGNE                        0
#define PH_PHONEM                         1
#define PH_SOUNDEX                        2
#define PH_EXSOUNDEX                      3
#define PH_NUMBER_OF_CODES                4
// ---------------------------------------------------------------------------------------------------------------------
// Kurzzeichen für phonetische Verfahren, Name und Kurzbeschreibung
// Index passend zu obigen Konstannten
static const char PhCodeSign[PH_NUMBER_OF_CODES][2] = {{"K"}, {"P"}, {"S"}, {"E"}};
static const char PhCodeName[PH_NUMBER_OF_CODES][32] = {
  {"Kölner Phonetik"}, 
  {"Phonem"},
  {"Soundex"}, 
  {"Extended Soundex"}
};
static const char PhCodeDescription[PH_NUMBER_OF_CODES][64] = {
  {"für deutsche Namen am Besten"}, 
  {"gut für deutsche Namen"},
  {"weniger für deutsche Namen geeignet"}, 
  {"erweiterte Soundex Variante"}
};
// ---------------------------------------------------------------------------------------------------------------------
// Buffergröße für Zeile kann angepaßt werden, dürfte aber für normale Textdateien ausreichen
// Buffergröße für Wörter, kann angepaßt werden, aber 64char dürften selbst für Worte mit Umlauten (2char) reichen
#define BUFFSIZE_LINE                     4096
#define BUFFSIZE_WORD                     64
// ---------------------------------------------------------------------------------------------------------------------
// Dazu passende Fixstrings
// Buffer für phonetische Codes IMMER doppelt so Groß wie Wortbuffer
// Da Kölner Phonetik u.U. einen Code produziert, der doppelt so Lang ist wie das Wort, was ihn erzeugt
typedef char                              word_t[BUFFSIZE_WORD];
typedef char                              phcode_t[(BUFFSIZE_WORD * 2)];
// ---------------------------------------------------------------------------------------------------------------------
// Boolean
typedef enum {true, false}                bool;
// ---------------------------------------------------------------------------------------------------------------------
// Für je einen Namen aus Programmübergabe
typedef struct {
  bool        exclude;                    // true wenn auszulassende Schreibweise
  word_t      name_norm;                  // Name in Originalschreibweise
  word_t      name_upper;                 // Name in ASCII Großbuchstaben
  const char  *color;                     // Pointer auf Farbstring
  phcode_t    code[PH_NUMBER_OF_CODES];   // Index passend zu Codenummer aus "phonetics.h"
} name_t;
// ---------------------------------------------------------------------------------------------------------------------
// Für Namensliste
typedef struct {
  int         number_of_names;            // Anzahl Namen in Liste
  name_t      *items;                     // Namen items
} nameslist_t;
// ---------------------------------------------------------------------------------------------------------------------
// Für Phonetik Optionen
typedef struct {
  int         lev;                        // erlaubte Lev-Dist (ist 0 wenn Option nicht übergeben)
  int         min;                        // erlaubte Mindestanzahl Buchstaben (ist 0 wenn Option nicht übergeben)
  int         max;                        // erlaubte Höchstanzahl Buchstaben (ist 0 wenn Option nicht übergeben)
  bool        show[PH_NUMBER_OF_CODES];   // Codeverfahren anzeigen = true, nicht = false
} phops_t;                                // Index passend zu Codenummer aus "phonetics.h"
// ---------------------------------------------------------------------------------------------------------------------
// Für Formatierungsoptionen
typedef struct {
  bool        n;                          // Zeilennummerierung
  bool        x;                          // Ausgabe Legende
  bool        v;                          // Verbose Mode
} frmops_t;
// ---------------------------------------------------------------------------------------------------------------------
// Für Ausgabeoptionen
typedef struct {
  bool        a;                          // Alles ausgeben
  bool        z;                          // Zeilen ausgeben
  bool        w;                          // Wörter ausgeben
  bool        c;                          // Wortpositionen / Längen ausgeben
} outops_t;
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHOSUDE
