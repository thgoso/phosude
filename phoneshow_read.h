#ifndef PHONESHOW_READ
#define PHONESHOW_READ

// ---------------------------------------------------------------------------------------------------------------------
// Lesefunktionen für Text von stdin
// ---------------------------------------------------------------------------------------------------------------------
// Liest eine Zeile von stdin ein und puffert lokal
int read_line (void);
// Rückgabe:
// PHSHOW_SUCCESS           Zeile gelesen und lokal gepuffert
// PHSHOW_END_REACHED       Datenempfang abgeschlossen
// PHSHOW_ERR_LINE_OVERLEN  Zeile mit Überlänge empfangen
// ---------------------------------------------------------------------------------------------------------------------
// Init für read_word, Springt an den Zeilenanfang des lokalen Puffers
void read_word_init (void);
// ---------------------------------------------------------------------------------------------------------------------
// Holt das nächste gültige deutsche Wort oder einzelnes Sonderzeichen aus lokal Puffer
// und gibt es in dest zurück
int read_word (word_t *dest);
// Rückgabe:
// PHSHOW_IS_WORD           wenn es sich um ein gültiges deutsches Wort handelt
// PHSHOW_IS_SPECIAL_CHAR   wenn es sich um ein einzelnes Sonderzeichen handelt
// PHSHOW_END_REACHED       wenn Zeile komplett durch ist (Neustart mit read_word_init)
// PHSHOW_ERR_WORD_OVERLEN  wenn Wort zu lang ist um in dest zu passen
// ---------------------------------------------------------------------------------------------------------------------
// Gibt aktuelle Position des Lesezeigers im lokalen Zeilenpuffer zurück
// Kann vor read_word aufgerufen werden um zu erfahren an welcher Position
// in der Zeile sich das folgende Wort/Sonderzeichen befindet
size_t read_cur_pos (void);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHONESHOW_READ
