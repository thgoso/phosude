#ifndef PHOSUDE_STDIN
#define PHOSUDE_STDIN

// ---------------------------------------------------------------------------------------------------------------------
// Lesefunktionen für Text von stdin
// ---------------------------------------------------------------------------------------------------------------------
// Liest eine Zeile von stdin ein und puffert lokal
// Rückgabe:  STAT_SUCCESS                Zeile gelesen und gepuffert
//            STAT_ERR_REC_OVERLEN_LINE   Zeile ist zu Lang für Buffer
//            STAT_END_REACHED            Textempfang abgeschlossen
int stdin_read_line (void);
// ---------------------------------------------------------------------------------------------------------------------
// Init für stdin_get_word, Springt an den Zeilenanfang des lokalen Zeilenpuffers
void stdin_carrige_return (void);
// ---------------------------------------------------------------------------------------------------------------------
// Holt das nächste gültige deutsche Wort (Einzelbuchstabe zählt auch als Wort) 
// oder einzelnes Sonderzeichen aus lokalem Zeilenpuffer und gibt es in dest zurück
// Rückgabe:  STAT_IS_WORD                In dest befindet sich ein gültiges deustchen Wort
//            STAT_IS_SPECIAL_CHAR        In dest befindet sich ein einzelnes Sonderzeichen
//            STAT_END_REACHED            Zeilenende erreicht (Zurück zum Zeilenanfang mit "stdin_carrige_return")
//            STAT_ERR_REC_OVERLEN_WORD   Wort hat Überlänge und paßt nicht in dest
int stdin_get_word (word_t dest);
// ---------------------------------------------------------------------------------------------------------------------
// Gibt aktuelle Position des Lesezeigers im lokalen Zeilenpuffer zurück
// Kann vor stdin_get_word aufgerufen werden um zu erfahren an welcher Position
// in der Zeile sich das folgende Wort/Sonderzeichen befindet
size_t stdin_get_carrige_pos (void);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHOSUDE_STDIN
