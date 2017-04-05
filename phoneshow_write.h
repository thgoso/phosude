#ifndef PHONESHOW_WRITE
#define PHONESHOW_WRITE

// ---------------------------------------------------------------------------------------------------------------------
// Ausgabefunktionen, liefern:
// PHSHOW_MATCH                mind. ein Fund
// PHSHOW_NO_MATCH             wenn kein Fund
// PHSHOW_ERR_LINE_OVERLEN     Zeile mit Überlänge von stdin empfangen
// PHSHOW_ERR_WORD_OVERLEN     Wort mit Überlänge von stdin empfangen
int write_out_a (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops);
int write_out_z (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops);
int write_out_w (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops);
int write_out_c (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops);
// ---------------------------------------------------------------------------------------------------------------------
// Liefert:
// PHSHOW_SUCCESS              Fehlerfrei
// PHSHOW_ERR_LINE_OVERLEN     Zeile mit Überlänge von stdin empfangen
// PHSHOW_ERR_WORD_OVERLEN     Wort mit Überlänge von stdin empfangen
int write_out_convert (void);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHONESHOW_WRITE
