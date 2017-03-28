#ifndef PHONETICS
#define PHONETICS

// Buffer-Größen für Strings zu Worten und phonetischen Codes
#define BUFFER_WORD_SIZE		64
#define BUFFER_CODE_SIZE		(BUFFER_WORD_SIZE * 4)

// Konvertierungsfunktionen
// Übergabe:	Pointer auf zu konvertierendes Wort [*src]
//				EIN EINZELNES DEUTSCHES WORT
// Rückgabe:	phonetischen Code in [*dest]
//
// *dest muß außreichend groß dimensioniert sein (BUFFER_CODE_SIZE)

// Falls das zu wandelnde Wort zu lang ist (BUFFER_WORD_SIZE)
// oder Sonderzeichen enthält (erlaubt NUR A-z, a-z, ÄÖÜäöüß) 
// oder das Wort überhaupt keinen phonetischen Code erzeugen kann
// wird in *dest ein Leerstring '\0' zurückgegeben


void phoneconvert_soundex 	(const char *src, char *dest);
void phoneconvert_exsoundex	(const char *src, char *dest);
void phoneconvert_cologne	(const char *src, char *dest);
void phoneconvert_phonem	(const char *src, char *dest);
 

#endif //PHONETICS
