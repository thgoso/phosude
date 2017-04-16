#ifndef PHOSUDE_CMDPARSE
#define PHOSUDE_CMDPARSE

// ---------------------------------------------------------------------------------------------------------------------
// Parserfunktionen Commandline
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:  CMD-Argumente
// Rückgabe:  STAT_SUCCESS oder Fehlerkonstannte
//            Namensliste list wird gefüllt
int parse_nameslist (const int argc, char* argv[], nameslist_t *list);
// ---------------------------------------------------------------------------------------------------------------------
// Übergabe:  CMD-Argumente
// Rückgabe:  STAT_SUCCESS oder Fehlerkonstannte
//            Phonetische, Format-, Ausgabeoptionen werden gefüllt
int parse_options (const int argc, char* argv[], phops_t *phops, frmops_t *frmops, outops_t *outops);
// ---------------------------------------------------------------------------------------------------------------------

#endif // PHOSUDE_CMDPARSE
