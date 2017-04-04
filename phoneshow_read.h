#ifndef PHONESHOW_READ
#define PHONESHOW_READ

// Liest eine Zeile von stdin ein und puffert lokal
int read_line (void);               // Rückgabe:
#define PHSREAD_LINE_SUCCESS      0 // Zeile gelesen und lokal gepuffert
#define PHSREAD_COMPLEETE         1 // Datenempfang abgeschlossen
#define PHSREAD_LINE_OVERLENGTH   2 // Zeile mit Überlänge empfangen

// Init für read_word, Springt an den Zeilenanfang des lokalen Puffers
void read_word_init (void);

// Holt das nächste gültige deutsche Wort oder einzelnes Sonderzeichen aus lokal Puffer
// und gibt es in dest zurück
int read_word (word_t *dest);       // Rückgabe:
#define PHSREAD_IS_WORD           0 // wenn es sich um ein gültiges deutsches Wort handelt
#define PHSREAD_IS_SPECIAL_CHAR   1 // wenn es sich um ein einzelnes Sonderzeichen handelt
#define PHSREAD_LINE_END          2 // wenn Zeile komplett durch ist (Neustart mit read_word_init)
#define PHSREAD_WORD_OVERLENGTH   3 // wenn Wort zu lang ist (BUFFER_SIZE_WORD)


// Gibt aktuelle Position des Lesezeigers im lokalen Zeilenpuffer zurück
// Kann vor read_word aufgerufen werden um zu erfahren an welcher Position
// in der Zeile sich das folgende Wort/Sonderzeichen befindet
size_t read_cur_pos (void);
 

#endif // PHONESHOW_READ
