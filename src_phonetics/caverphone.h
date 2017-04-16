#ifndef PHOSUDE_CAVERPHONE
#define PHOSUDE_CAVERPHONE

// ---------------------------------------------------------------------------------------------------------------------
// Caverphone Definitionen
// ---------------------------------------------------------------------------------------------------------------------
// Ein Caverphone Code ist immer 10 Zeichen lang (+ Nullbyte)
typedef char caverphone_t[11];
// ---------------------------------------------------------------------------------------------------------------------
// Caverphone Funktionen
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:    Gültiges deutsches Wort in src
// Rückgabe:    phonetischen Code in dest
void make_caverphone (const word_t src, caverphone_t dest);
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:    Gültiges deutsches Wort in word
//              Vergleichscode in code
//              Erlaubte Lev-Dist in lev
// Rückgabe:    true wenn "wort" dem phonetischen Code "code" entspricht
//              false wenn NICHT
//              Tatsächliche Lev-Dist in lev
bool comp_word_caverphone (const word_t word, const caverphone_t code, int *lev);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHOSUDE_CAVERPHONE
