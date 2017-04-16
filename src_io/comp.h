#ifndef PHOSUDE_COMP
#define PHOSUDE_COMP

// ---------------------------------------------------------------------------------------------------------------------
// Phonetische Vergleichsfunktion Namensliste / Wort
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:  Namensliste, Wort, Phonetische Optionen
// Rückgabe:  Pointer auf Namen in list der phonetisch gleich mit word ist
//            Falls keine gefunden, wird NULL-Pointer zurückgegeben
name_t *comp_nameslist_word (const nameslist_t *list, const word_t word, const phops_t *phops);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHOSUDE_COMP
