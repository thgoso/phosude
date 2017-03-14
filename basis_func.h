/* Basisfunktionen */

#ifndef BASIS_FUNC
#define BASIS_FUNC

//Definitionen Rückgabewerte Textabwicklung
#define TEXT_OK 				0
#define TEXT_ERROR 				1
#define TEXT_UNDEF 				2
#define TEXT_TO_SHORT 			3
#define TEXT_NO_CODE 			4
#define TEXT_NOT_EQUAL 			5
#define MEM_ERR 				6

//Maximale Länge Wort/Name und Zeile
#define MAX_WORD_LEN 			128
#define MAX_LINE_LEN 			2048

//Typedef für Umlaute
typedef char uml_t[3];

// Funktionen
int  check_word_german			(char *text);					// Prüft ob gültiges deutsches Wort
void kill_onebytechars			(char *text, int character);	// Entfernt Einbytezeichen aus String
void kill_double_onebytechars	(char *text);					// Macht aus mehreren Einbytezeichen EINES
void conv_uml 					(char *text);					// Ersetzt Umlaute im String durch Einzelzeichen
void conv_to_upper 				(char *text);					// Wandelt in Großbuchstaben
void conv_to_lower 				(char *text);					// Wandelt in Kleinbuchstaben
// Phonetische Funktionen
// target = Quellstring für den der Code erzeugt werden soll, dest = Zielstring in dem der Code gespeichert wird
// String in target MUSS IMMER vorbereitet sein, in der Form, daß NUR KLEINBUCHSTABEN und KEINE UMLAUTE enthalten sind
void conv_to_cologne			(char *target, char *dest);
void conv_to_phonem				(char *target, char *dest);
void conv_to_soundex			(char *target, char *dest);
void conv_to_exsoundex			(char *target, char *dest);




#endif //BASIS_FUNC

