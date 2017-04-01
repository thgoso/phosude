#ifndef PHONESHOW_WRITE
#define PHONESHOW_WRITE


// Ausgabefunktionen, liefern:
#define WRITE_MATCH          0    // mind. einem Fund
#define WRITE_NO_MATCH       1    // wenn kein Fund
#define WRITE_WORD_OVERSIZE  2    // Wort mit Überlänge von stdin empfangen
#define WRITE_LINE_OVERSIZE  3    // Zeile mit Überlänge von stdin empfangen


int write_out_a (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops);
int write_out_z (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops);
int write_out_w (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops);

#endif // PHONESHOW_WRITE
