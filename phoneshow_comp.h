#ifndef PHONESHOW_COMP
#define PHONESHOW_COMP


// Prüft ob übergebenes Wort (word)
// nach phonetischen Optionen (phops)
// mit einem Namen aus Namensliste (list)
// identisch ist
// Rückgabe:  false wenn keine phonetische Übereinstimmung gefunden wurde
//                in "idx" wird -1 zurückgegeben
//            true wenn Übereinstimmung
//                in "idx" wird der Index des zugehörigens Namens aus Namensliste zurückgegeben
bool compare_word_nameslist (const nameslist_t *list, const phops_t *phops, const char *word, int *idx);




#endif // PHONESHOW_COMP
