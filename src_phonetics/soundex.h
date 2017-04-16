#ifndef PHOSUDE_SOUNDEX
#define PHOSUDE_SOUNDEX

// ---------------------------------------------------------------------------------------------------------------------
// Soundex Definitionen
// ---------------------------------------------------------------------------------------------------------------------
// Ein Soundexcode ist immer 4 Zeichen lang (+ Nullbyte)
typedef char soundex_t[5];
// ---------------------------------------------------------------------------------------------------------------------
// Soundex Funktionen
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:    Gültiges deutsches Wort in src
// Rückgabe:    phonetischen Code in dest
void make_soundex (const word_t src, soundex_t dest);
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:    Gültiges deutsches Wort in word
//              Vergleichscode in code
//              Erlaubte Lev-Dist in lev
// Rückgabe:    true wenn "wort" dem phonetischen Code "code" entspricht
//              false wenn NICHT  
//              Tatsächliche Lev-Dist in lev
bool comp_word_soundex (const word_t word, const soundex_t code, int *lev);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHOSUDE_SOUNDEX
