#ifndef PHPACK_PHONETICS
#define PHPACK_PHONETICS

// Konvertierungsfunktionen
// Übergabe:    Pointer auf zu konvertierendes Wort [*src]
//              EIN EINZELNES DEUTSCHES WORT
// Rückgabe:    phonetischen Code in [*dest]

// In *dest wird ein Leerstring '\0' zurückgegeben wenn:
// *src Sonderzeichen enthält (erlaubt NUR A-Z, a-z, ÄÖÜäöüß)
// *src selbst ein Leerstring ist

// Phonem liefert nicht für jedes Wort einen gültigen Code
// z.B. das Wort Ahe
// In solchen Fällen wird im Code "---" zurückgegeben
// Alle anderen Verfahren liefern für jedes deutsche Wort...
// Auch für einzelne Buchstaben einen gültigen Code

void phoneconvert_soundex (const word_t *src, code_t *dest);
void phoneconvert_exsoundex (const word_t *src, code_t *dest);
void phoneconvert_cologne (const word_t *src, code_t *dest);
void phoneconvert_phonem (const word_t *src, code_t *dest);


#endif // PHPACK_PHONETICS
