#ifndef PHONESHOW_WRITE
#define PHONESHOW_WRITE

// Ausgabefunktionen, liefern:
// PHS_MATCH                    mind. ein Fund
// PHS_NO_MATCH                 wenn kein Fund
// PHS_ERR_REC_LINE_OVERSIZE    Wort mit Überlänge von stdin empfangen
// PHS_ERR_REC_WORD_OVERSIZE    Zeile mit Überlänge von stdin empfangen


int write_out_a (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops);
int write_out_z (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops);
int write_out_w (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops);
int write_out_c (void);


#endif // PHONESHOW_WRITE
