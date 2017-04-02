#ifndef PHONETICS
#define PHONETICS

// Konvertierungsfunktionen
// Übergabe:    Pointer auf zu konvertierendes Wort [*src]
//              EIN EINZELNES DEUTSCHES WORT
// Rückgabe:    phonetischen Code in [*dest]
//
// *dest muß außreichend groß dimensioniert sein (BUFFER_SIZE_WORD)

// In *dest wird ein Leerstring '\0' zurückgegeben wenn:
// Stringlänge *src >= BUFFER_SIZE_WORD
// Stringlänge erzeugter Code >= BUFFER_SIZE_WORD
// *src Sonderzeichen enthält (erlaubt NUR A-Z, a-z, ÄÖÜäöüß)

// Phonem liefert nicht für jedes Wort einen gültigen Code
// z.B. das Wort Ahe
// In solchen Fällen wird im Code "---" zurückgegeben
// Alle anderen Verfahren liefern für jedes deutsche Wort...
// Auch für einzelne Buchstaben einen gültigen Code

void phoneconvert_soundex   (const char *src, char *dest);
void phoneconvert_exsoundex (const char *src, char *dest);
void phoneconvert_cologne   (const char *src, char *dest);
void phoneconvert_phonem    (const char *src, char *dest);


#endif //PHONETICS
