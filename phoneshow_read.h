#ifndef PHONESHOW_READ
#define PHONESHOW_READ


// Liest eine Zeile von stdin ein und puffert lokal
// Rückgabe:
// PHS_REC_LINE_SUCCESS       Zeile gelesen und lokal gepuffert
// PHS_REC_COMPLEETE          Datenempfang abgeschlossen
// PHS_ERR_REC_LINE_OVERSIZE  Zeile mit Überlänge empfangen
int read_line (void);

// Init für read_word, Springt an den Zeilenanfang des lokalen Puffers
void read_word_init (void);

// Holt das nächste gültige deutsche Wort oder einzelnes Sonderzeichen aus lokal Puffer
// und gibt es in dest zurück
// dest muß ausreichend dimensioniert sein (BUFFER_SIZE_WORD)
// Rückgabe:
// PHS_IS_WORD                wenn es sich um ein gültiges deutsches Wort handelt
// PHS_IS_SPECIAL_CHAR        wenn es sich um ein einzelnes Sonderzeichen handelt
// PHS_LINE_END_RACHED        wenn Zeile komplett durch ist (Neustart mit read_word_init)
// PHS_ERR_REC_WORD_OVERSIZE  wenn Wort zu lang ist (BUFFER_SIZE_WORD)
int read_word (char *dest);

// Gibt aktuelle Position des Lesezeigers im lokalen Zeilenpuffer zurück
// Kann vor read_word aufgerufen werden um zu erfahren an welcher Position
// in der Zeile sich das folgende Wort/Sonderzeichen befindet
size_t read_cur_pos (void);
 

#endif // PHONESHOW_READ
