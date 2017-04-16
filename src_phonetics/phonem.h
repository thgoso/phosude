#ifndef PHOSUDE_PHONEM
#define PHOSUDE_PHONEM

// ---------------------------------------------------------------------------------------------------------------------
// Phonem Definitionen
// ---------------------------------------------------------------------------------------------------------------------
// Ein Phonem Code kann maximal so Lang sein, wie das Wort, was ihn erzeugt
typedef char phonem_t[BUFFSIZE_WORD];
// ---------------------------------------------------------------------------------------------------------------------
// Phonem Funktionen
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:    Gültiges deutsches Wort in src
// Rückgabe:    phonetischen Code in dest
void make_phonem (const word_t src, phonem_t dest);
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:    Gültiges deutsches Wort in word
//              Vergleichscode in code
//              Erlaubte Lev-Dist in lev
// Rückgabe:    true wenn "wort" dem phonetischen Code "code" entspricht
//              false wenn NICHT
//              Tatsächliche Lev-Dist in lev
bool comp_word_phonem (const word_t word, const phonem_t code, int *lev);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHOSUDE_PHONEM
