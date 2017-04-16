#ifndef PHOSUDE_EXSOUNDEX
#define PHOSUDE_EXSOUNDEX

// ---------------------------------------------------------------------------------------------------------------------
// Extended Soundex Definitionen
// ---------------------------------------------------------------------------------------------------------------------
// Ein Extended Soundexcode ist immer 5 Zeichen lang (+ Nullbyte)
typedef char exsoundex_t[6];
// ---------------------------------------------------------------------------------------------------------------------
// Extended Soundex Funktionen
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:    Gültiges deutsches Wort in src
// Rückgabe:    phonetischen Code in dest
void make_exsoundex (const word_t src, exsoundex_t dest);
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:    Gültiges deutsches Wort in word
//              Vergleichscode in code
//              Erlaubte Lev-Dist in lev
// Rückgabe:    true wenn "wort" dem phonetischen Code "code" entspricht
//              false wenn NICHT
//              Tatsächliche Lev-Dist in lev
bool comp_word_exsoundex (const word_t word, const exsoundex_t code, int *lev);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHOSUDE_EXSOUNDEX
