#ifndef PHOSUDE_DAITCHMOKOTOFF
#define PHOSUDE_DAITCHMOKOTOFF

// ---------------------------------------------------------------------------------------------------------------------
// Daitch-Mokotoff Definitionen
// ---------------------------------------------------------------------------------------------------------------------
// Verfahren kann mehrere Codevarianten erzeugen... ein Code ist je 6 Zeichen lang
// Da es teilweise zwei Varianten pro Zeichen geben kann, werden im schlimmsten Fall für ein Wort 64 Versionen erzeugt
// Kommt zwar Praktisch nie vor, Buffer aber dennoch ausreichend dimensionieren
// Die Codevarianten werden im String durch "/" getrennt
// ((6 Zeichen + 1 Trenner) * 64 Versionen) + Nullbyte 
typedef char daitchmok_t[512];
// ---------------------------------------------------------------------------------------------------------------------
// Daitch-Mokotoff Funktionen
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:    Gültiges deutsches Wort in src
// Rückgabe:    phonetischen Code in dest
void make_daitchmok (const word_t src, daitchmok_t dest);
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:    Gültiges deutsches Wort in word
//              Vergleichscode in code
//              Erlaubte Lev-Dist in lev
// Rückgabe:    true wenn "wort" dem phonetischen Code "code" entspricht
//              false wenn NICHT  
//              Tatsächliche Lev-Dist in lev
bool comp_word_daitchmok (const word_t word, const daitchmok_t code, int *lev);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHOSUDE_DAITCHMOKOTOFF
