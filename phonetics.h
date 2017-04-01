#ifndef PHONETICS
#define PHONETICS

// Konvertierungsfunktionen
// Übergabe:    Pointer auf zu konvertierendes Wort [*src]
//              EIN EINZELNES DEUTSCHES WORT
// Rückgabe:    phonetischen Code in [*dest]
//
// *dest muß außreichend groß dimensioniert sein (BUFFER_SIZE_CODE)

// Falls das zu wandelnde Wort zu lang ist (BUFFER_SIZE_WORD)
// oder Sonderzeichen enthält (erlaubt NUR A-z, a-z, ÄÖÜäöüß) 
// wird in *dest ein Leerstring '\0' zurückgegeben

// Phonem liefert nicht für jedes Wort einen gültigen Code
// z.B. das Wort Ahe
// In solchen fällen wird im Code "---" zurückgegeben
// Alle anderen Verfahren liefern für jedes deutsche Wort einen gültigen Code

void phoneconvert_soundex   (const char *src, char *dest);
void phoneconvert_exsoundex (const char *src, char *dest);
void phoneconvert_cologne   (const char *src, char *dest);
void phoneconvert_phonem    (const char *src, char *dest);
 

#endif //PHONETICS
