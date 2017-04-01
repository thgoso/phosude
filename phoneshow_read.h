#ifndef PHONESHOW_READ
#define PHONESHOW_READ



// Liest eine Zeile von stdin ein und puffert lokal
int read_line (void);               // Rückgabe:  
#define READ_LINE_SUCCESS       0   // Zeile gelesen
#define READ_LINE_END           1   // Datenempfang abgeschlossen
#define READ_LINE_OVERSIZE      2   // Zeile mit Überlänge empfangen


// Init für read_word, Springt an den Anfang der aktuellen Zeile
void read_word_init (void);          


// Holt das nächste gültige deutsche Wort oder einzelnes Sonderzeichen aus lokal Puffer
// und gibt es in dest zurück
// dest muß ausreichend dimensioniert sein (BUFFER_SIZE_WORD)
int read_word (char *dest);         // Rückgabe:
#define READ_WORD_IS_WORD       0   // wenn es sich um ein gültiges deutsches Wort handelt
#define READ_WORD_IS_NO_WORD    1   // wenn es sich um ein einzelnes Sonderzeichen handelt
#define READ_WORD_END           2   // wenn Zeile komplett durch ist (Neustart mit read_word_init)
#define READ_WORD_OVERSIZE      3   // wenn Wort zu lang ist (BUFFER_SIZE_WORD)



#endif // PHONESHOW_READ
