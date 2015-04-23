/* Basisfunktionen */

#ifndef BASIS_FUNC
#define BASIS_FUNC

//Definitionen Rückgabewerte Textabwicklung
#define TEXT_OK 0
#define TEXT_NICHT_OK 1
#define TEXT_UNDEFINIERT 2
#define TEXT_ZU_KURZ 3
#define TEXT_KEIN_CODE 4
#define TEXT_UNGLEICH 5
#define MEM_ERR 6
//Maximale Länge Wort/Name
#define MAX_WORT_LAENGE 128
//Maximale Länge einer Zeile
#define MAX_ZEILE_LAENGE 2048

//Typedef für Umlaute
typedef char UL_FORM[3];


int  pruefe_wort_deutsch (char *text);
void conv_umlaute (char *text);
void conv_grossbuchstaben (char *text);
void conv_kleinbuchstaben (char *text);
void entferne_einbytezeichen (char *text, int zeichen);
void entferne_doppelte_einbytezeichen (char *text);
void conv_koelner(char *quelle_koelner, char *ziel_koelner);
void conv_phonem(char *quelle_phonem, char *ziel_phonem);
void conv_soundex(char *quelle_soundex, char *ziel_soundex);
void conv_exsoundex(char *quelle_exsoundex, char *ziel_exsoundex);


#endif //BASIS_FUNC

