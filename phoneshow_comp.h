#ifndef PHONESHOW_COMP
#define PHONESHOW_COMP

// ---------------------------------------------------------------------------------------------------------------------
// Phonetische Vergleichsfunktionen Namensliste mit Wort aus Text 
// ---------------------------------------------------------------------------------------------------------------------
// Prüft ob übergebenes Wort (word)
// nach phonetischen Optionen (phops)
// mit einem Namen aus Namensliste (list)
// identisch ist
// Rückgabe: Pointer auf passenden Namen in list wenn eine phonetische Übereinstimmung gefunden wurde
//           Pointer auf NULL wenn keine phonetische Übereinstimmung gefunden wurde
name_t *compare_word_nameslist (const nameslist_t *list, const phops_t *phops, const word_t *word);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHONESHOW_COMP
