#ifndef PHONEPACK_STDIN
#define PHONEPACK_STDIN

// ---------------------------------------------------------------------------------------------------------------------
// Lesefunktionen für Text von stdin
// ---------------------------------------------------------------------------------------------------------------------
// Rückgabekonstannten
#define STDIN_SUCCESS           0
#define STDIN_IS_WORD           1
#define STDIN_IS_SPECIAL_CHAR   2
#define STDIN_END_REACHED       3
#define STDIN_ERR_LINE_OVERLEN  4
#define STDIN_ERR_WORD_OVERLEN  5

// Typedef String für Wort
#define BUFFSIZE_WORD           64
typedef char                    word_t[BUFFSIZE_WORD];
// ---------------------------------------------------------------------------------------------------------------------
// Liest eine Zeile von stdin ein und puffert lokal
int stdin_read_line (void);
// Rückgabe:
// STDIN_SUCCESS                Zeile gelesen und lokal gepuffert
// STDIN_END_REACHED            Datenempfang abgeschlossen
// STDIN_ERR_LINE_OVERLEN       Zeile mit Überlänge empfangen
// ---------------------------------------------------------------------------------------------------------------------
// Init für stdin_get_word, Springt an den Zeilenanfang des lokalen Zeilenpuffers
void stdin_carrige_return (void);
// ---------------------------------------------------------------------------------------------------------------------
// Holt das nächste gültige deutsche Wort (Einzelbuchstabe zählt auch als Wort) 
// oder einzelnes Sonderzeichen aus lokalem Zeilenpuffer und gibt es in dest zurück
int stdin_get_word (word_t dest);
// Rückgabe:
// STDIN_IS_WORD                wenn es sich um ein gültiges deutsches Wort handelt
// STDIN_IS_SPECIAL_CHAR        wenn es sich um ein einzelnes Sonderzeichen handelt
// STDIN_END_REACHED            wenn Zeile komplett durch ist (Neustart mit read_word_init)
// STDIN_ERR_WORD_OVERLEN       wenn Wort zu lang ist um in dest zu passen
// ---------------------------------------------------------------------------------------------------------------------
// Gibt aktuelle Position des Lesezeigers im lokalen Zeilenpuffer zurück
// Kann vor stdin_get_word aufgerufen werden um zu erfahren an welcher Position
// in der Zeile sich das folgende Wort/Sonderzeichen befindet
size_t stdin_get_carrige_pos (void);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHONEPACK_STDIN
