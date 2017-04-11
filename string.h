#ifndef PHOSUDE_STRING
#define PHOSUDE_STRING

// ---------------------------------------------------------------------------------------------------------------------
// Stringfunktionen
// ---------------------------------------------------------------------------------------------------------------------
// Wandelt einen übergebenen String in Typ word_t (String fixer Größe)
// Prüft ob String ein deutsches Wort ist, also nur aus A-Z, a-z, ÄÖÜäöüß besteht
// Leerstrings oder andere Buchstaben/Zeichen im String sind nicht erlaubt
// Rückgabe:  STAT_SUCCESS            bei Erfolg
//            STAT_ERR_NO_WORD        wenn kein gültiges deutsches Wort
//            STAT_ERR_OVERLEN_WORD   wenn src nicht in dest paßt
int str_to_word (const char *src, word_t dest);
// ---------------------------------------------------------------------------------------------------------------------
// String in ASCII GROSSBUCHSTABEN wandeln, deutsche Umlaute ersetzten
// Ä,ä = AE, Ö,ö = OE, Ü,ü = UE, ß = SS
void str_to_ascii_upper (char *s);
// ---------------------------------------------------------------------------------------------------------------------
// Gibt Anzahl deutscher Buchstaben im String zurück
// Da in UTF-8 deutsche Umlaute 2 chars sind ist die Funktion zum richtigen Zählen der Buchstaben nötig
size_t str_count_german_letters (const char *s);
// ---------------------------------------------------------------------------------------------------------------------
// entfernt einzelne Zeichen Typ chars im String die dem übergebenen Zeichen (c) entsprechen,
// sodaß diese Zeichen gelöscht werden
void str_del_chars (char *s, const char c);
// ---------------------------------------------------------------------------------------------------------------------
// entfernt alle mehrfach hintereinander auftretenden Zeichen Typ char im String, sodaß nur noch je eines vorhanden ist
void str_del_multiple_chars (char *s);
// ---------------------------------------------------------------------------------------------------------------------
// Ersetzt Zeichenketten im String "s" die "from" entsprechen durch "to"
// Such- und Ersatzeichenfolge müssen die selbe Größe haben
void str_replace_same_size (char *s, const char *from, const char *to);
// ---------------------------------------------------------------------------------------------------------------------
// Ersetzt in "s" Gruppen gleicher Buchstaben "from" durch einzelnen "to"
// str_replace_group_with_one ("Schifffahrtshafenschiffer", 'f', 'X');
//                                  ^^^       ^      ^^
// ergibt "SchiXahrtshafenschiXer"
//             ^       ^      ^
void str_replace_group_with_one (char *s, const char from, const char to);
// ---------------------------------------------------------------------------------------------------------------------
// Levenshtein-Distanz aus http://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance
// Berechnet die Levenshtein-Distanz zweier übergebener Strings
// Rückgabe:  Distanz als int (immer positiv)
int str_lev (const char *s1, const char *s2);
// ---------------------------------------------------------------------------------------------------------------------








#endif // PHOSUDE_STRING
