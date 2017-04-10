#ifndef PHOSUDE_TEXTPRINT
#define PHOSUDE_TEXTPRINT

// ---------------------------------------------------------------------------------------------------------------------
// Textausgabefunktionen
// Holen Daten von stdin ab und geben sie formatiert in stdout aus
// Rückgabe:  STAT_SUCCESS oder Fehlerkonstannte STAT_ERR_____
int printout_a (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops);
int printout_z (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops);
int printout_w (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops);
int printout_c (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops);
// ---------------------------------------------------------------------------------------------------------------------
// Textausgabe ohne phonetische Namenssuche, sondern nur Datenstrom kodieren
// Rückgabe:  STAT_SUCCESS oder Fehlerkonstannte STAT_ERR_____
int printout_convert (void);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHOSUDE_TEXTPRINT
