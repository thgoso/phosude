#ifndef PHONESHOW_WRITE
#define PHONESHOW_WRITE

// Ausgabefunktionen, liefern:
#define PHSWRITE_MATCH                0 // mind. ein Fund
#define PHSWRITE_NO_MATCH             1 // wenn kein Fund
#define PHSWRITE_LINE_OVERLENGTH      2 // Wort mit Überlänge von stdin empfangen
#define PHSWRITE_WORD_OVERLENGTH      3 // Zeile mit Überlänge von stdin empfangen

int write_out_a (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops);
int write_out_z (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops);
int write_out_w (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops);
int write_out_c (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops);


// Liefert bei Durchlauf ohne Fehler immer PHWRITE_NO_MATCH
// Sonst Fehlerkonstannte wie oben
int write_out_convert (void);


#endif // PHONESHOW_WRITE
