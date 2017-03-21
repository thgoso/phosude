#ifndef PHONETICS
#define PHONETICS

// Buffer-Größen für Strings zu Namen und phonetischen Code
#define BUFFER_NAME_SIZE		64
#define BUFFER_CODE_SIZE		(BUFFER_NAME_SIZE * 4)

// Definitionen zur Funktionsübergabe
#define CONV_MODE_COLOGNE		0			// Modus Kölner Phonetik
#define CONV_MODE_PHONEM		1			// Modus Phonem
#define CONV_MODE_SOUNDEX		2			// Modus Soudex
#define CONV_MODE_EXSOUNDEX		3			// Modus Extendet Soundex

// Definitionen zur Funktionsrückgabe
#define CONV_STAT_OK			0			// Gültigen phonetischer Code generiert
#define CONV_STAT_NO_CODE		1			// Wort/Name erzeugt keinen phonetischen Code (z.B. bei Wort "Ahe" mit Phonem)
#define CONV_STAT_NO_WORD		2			// Wort/Name ist KEIN deutsches Wort, enthält Sonderzeichen oder Spaces
#define CONV_STAT_TO_LONG		3			// Wort/Name ist zu Lang (> BUFFER_NAME_SIZE)

// Konvertierungsfunktion
// Übergabe:	Pointer auf zu konvertierendes Wort [*src]
//				Pointer auf Rückgabestring [*dest]
//				Konvertierungsmodus (nach obigen Definitionen) [phonemode]
// Rückgabe:	Status (nach obigen Definitionen)
//				phonetischen Code in [*dest]
//
// *dest muß außreichend groß dimensioniert sein (>= BUFFER_CODE_SIZE)
int phoneconvert (const char *src, char *dest, const int phonemode);
 

#endif //PHONETICS
