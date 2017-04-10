#ifndef PHOSUDE_PHONETICS
#define PHOSUDE_PHONETICS

// ---------------------------------------------------------------------------------------------------------------------
// Phonetische Codierfunktionen
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:    Gültiges deutsches Wort in src
//              code_no (0 bis PH_NUMBER_OF_CODES - 1) bzw. Konstannte PH_COLOGNE o.ä
// Rückgabe:    phonetischen Code in dest

// Phonem liefert nicht für jedes Wort einen gültigen Code, z.B. das Wort Ahe
// In solchen Fällen wird im Code "---" zurückgegeben
// Alle anderen Verfahren liefern für jedes deutsche Wort, auch für einzelne Buchstaben, einen gültigen Code.
void phonetics_get_code (const word_t src, phcode_t dest, const int code_no);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHOSUDE_PHONETICS
