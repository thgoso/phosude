#ifndef PHOSUDE_CMDPARSE
#define PHOSUDE_CMDPARSE

// ---------------------------------------------------------------------------------------------------------------------
// Parserfunktionen Commandline
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:  CMD-Argumente
// Rückgabe:  Anzahl Namen in CMD-Argumenten (0 wenn keine Namen übergeben, sonst Anzahl)
int parser_count_names (const int argc, char* argv[]);
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:  CMD-Argumente
//            in list muß VOR Aufruf die Anzahl der Namen gesetzt sein
//            und das Array items passend dimensioniert sein
// Rückgabe:  list.items komplett ausgefüllt
int parser_fill_nameslist (const int argc, char* argv[], nameslist_t *list);
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:  CMD-Argumente
// Rückgabe:  STAT_SUCCESS oder Fehlerkonstannte
//            Phonetische, Format-, Ausgabeoptionen werden gefüllt
int parse_options (const int argc, char* argv[], phops_t *phops, frmops_t *frmops, outops_t *outops);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHOSUDE_CMDPARSE
