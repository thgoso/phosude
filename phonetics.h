#ifndef PHPACK_PHONETICS
#define PHPACK_PHONETICS

// ---------------------------------------------------------------------------------------------------------------------
// Phonetische Codierfunktionen
// ---------------------------------------------------------------------------------------------------------------------
// Übergabekonstannten
#define PHONETICS_COLOGNE           0
#define PHONETICS_PHONEM            1
#define PHONETICS_SOUNDEX           2
#define PHONETICS_EXSOUNDEX         3

// Rückgabekonstannten
#define PHONETICS_SUCCESS           0
#define PHONETICS_ERR_NO_WORD       1
#define PHONETICS_ERR_OVERLEN       2

// Typedef String für phonetischen Code... Bei der Buffergröße ausreichend für
// Codierung von Worten mit bis zu 64 chars (Umlaute und X/x zählen doppelt)
#define BUFFSIZE_CODE               64
typedef char                        phcode_t[BUFFSIZE_CODE];
// ---------------------------------------------------------------------------------------------------------------------
int phonetics_get_code (const char *src, phcode_t dest, const int code_no);
// Übergabe:    Wort in src ... EIN EINZELNES DEUTSCHES WORT
//              code_no nach obigen Konstanntendefinitionen
// Rückgabe:    PHONETICS_SUCCESS       und phonetischen Code in dest (String fester Größe)
//              PHONETICS_ERR_OVERLEN   src Wort zu lang zum codieren
//              PHONETICS_ERR_NO_WORD   src ist gar kein Wort, erlaubt NUR A-Z, a-z, ÄÖÜäöüß
//
// Phonem liefert nicht für jedes Wort einen gültigen Code, z.B. das Wort Ahe
// In solchen Fällen wird im Code "---" zurückgegeben
// Alle anderen Verfahren liefern für jedes deutsche Wort, auch für einzelne Buchstaben, einen gültigen Code.
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHPACK_PHONETICS
