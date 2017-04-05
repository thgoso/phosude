#ifndef PHPACK_STRING
#define PHPACK_STRING

// ---------------------------------------------------------------------------------------------------------------------
// Für Worte / Namen nutzen
// Buffergröße ggf. erhöhen, jedoch gibt es selten Wörter/Namen die länger sind
#define BUFFSIZE_WORD 64
typedef struct { char s[BUFFSIZE_WORD]; } word_t;
// ---------------------------------------------------------------------------------------------------------------------
// Stringfunktionen
// ---------------------------------------------------------------------------------------------------------------------
// Kopiert String dynamischer Länge *src in String fester Länge *dest
// sofern String nicht zu lang ist
// Falls zu lang wird false zurückgegeben und in *dest '\0'
// Bei Erfolg wird true zurückgegeben
bool str_to_word_type (const char *src, word_t *dest);
// ---------------------------------------------------------------------------------------------------------------------
// Wandelt einen übergebenen String nach ASCII GROSSBUCHSTABEN
// Umlaute werden ersetzt zu:
// Ä,ä = AE ; Ü,ü = UE ; Ö,ö = OE ; ß = SS
// Falls sich danach andere Zeichen als A-Z im String befinden
// wird dieser geleert '\0' und false zurückgegeben
// Besteht der String danach nur noch aus A-Z wird true zurückgegeben
bool str_to_ascii_upper_word (char *s);
// ---------------------------------------------------------------------------------------------------------------------
// entfernt einzelne Einbytezeichen im String die dem übergebenen Zeichen (c) entsprechen,
// sodaß diese Zeichen gelöscht werden
void str_del_ascii_chars (char *s, const char c);
// ---------------------------------------------------------------------------------------------------------------------
// entfernt alle mehrfach hintereinander auftretenden Einbytezeichen im String,
// sodaß nur noch je eines vorhanden ist
void str_del_multiple_ascii_chars (char *s);
// ---------------------------------------------------------------------------------------------------------------------
// Levenshtein-Distanz zweier Strings ermitteln
int str_lev(const char *s1, const char *s2);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHPACK_STRING
