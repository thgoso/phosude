#ifndef PHOSUDE_COLOGNE
#define PHOSUDE_COLOGNE

// ---------------------------------------------------------------------------------------------------------------------
// Kölner Phonetik Definitionen
// ---------------------------------------------------------------------------------------------------------------------
// Ein Kölner Phonetik Code kann maximal doppelt so Lang sein, wie das Wort, was ihn erzeugt
typedef char cologne_t[(BUFFSIZE_WORD *2)];
// ---------------------------------------------------------------------------------------------------------------------
// Kölner Phonetik Funktionen
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:    Gültiges deutsches Wort in src
// Rückgabe:    phonetischen Code in dest
void make_cologne (const word_t src, cologne_t dest);
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:    Gültiges deutsches Wort in word
//              Vergleichscode in code
//              Erlaubte Lev-Dist in lev
// Rückgabe:    true wenn "wort" dem phonetischen Code "code" entspricht
//              false wenn NICHT  
//              Tatsächliche Lev-Dist in lev
bool comp_word_cologne (const word_t word, const cologne_t code, int *lev);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHOSUDE_COLOGNE
