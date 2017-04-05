#ifndef PHPACK_PHONETICS
#define PHPACK_PHONETICS

// ---------------------------------------------------------------------------------------------------------------------
// Für phonetische Codes nutzen
// Kölner Phonetik erzeugt im schlimmsten Fall einen Code der doppelt so lang ist wie das Wort das ihn erzeugt
// Zwar nur in dem Fall, daß das Wort nur aus "X"en besteht, aber um Bufferüberläufe zu verhindern
// außreichend dimensionieren
#define BUFFSIZE_CODE (BUFFSIZE_WORD * 2)
typedef struct { char s[BUFFSIZE_CODE]; } phcode_t;
// ---------------------------------------------------------------------------------------------------------------------
// Phonetische Konvertierungsfunktionen
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:    Pointer auf zu konvertierendes Wort *src (String fester Größe)
//              EIN EINZELNES DEUTSCHES WORT
// Rückgabe:    phonetischen Code in *dest (String fester Größe)

// In *dest wird ein Leerstring '\0' zurückgegeben wenn:
// *src Sonderzeichen enthält (erlaubt NUR A-Z, a-z, ÄÖÜäöüß)
// *src selbst ein Leerstring ist

// Phonem liefert nicht für jedes Wort einen gültigen Code
// z.B. das Wort Ahe
// In solchen Fällen wird im Code "---" zurückgegeben
// Alle anderen Verfahren liefern für jedes deutsche Wort...
// Auch für einzelne Buchstaben einen gültigen Code

void phoneconvert_soundex (const word_t *src, phcode_t *dest);
void phoneconvert_exsoundex (const word_t *src, phcode_t *dest);
void phoneconvert_cologne (const word_t *src, phcode_t *dest);
void phoneconvert_phonem (const word_t *src, phcode_t *dest);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHPACK_PHONETICS
