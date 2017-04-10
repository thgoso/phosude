/*
phosude cmdparse.c
Copyright (C) 2015-2017, Thomas Gollmer, th_goso@freenet.de
Dieses Programm ist freie Software. Sie können es unter den Bedingungen der GNU General Public License,
wie von der Free Software Foundation veröffentlicht, weitergeben und/oder modifizieren,
entweder gemäß Version 3 der Lizenz oder (nach Ihrer Option) jeder späteren Version.

Die Veröffentlichung dieses Programms erfolgt in der Hoffnung, daß es Ihnen von Nutzen sein wird,
aber OHNE IRGENDEINE GARANTIE, sogar ohne die implizite Garantie der
MARKTREIFE oder der VERWENDBARKEIT FÜR EINEN BESTIMMTEN ZWECK.
Details finden Sie in der GNU General Public License. Sie sollten ein Exemplar der GNU General Public License
zusammen mit diesem Programm erhalten haben. Falls nicht, siehe <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <string.h>
#include "phosude.h"
#include "phonetics.h"
#include "string.h"

// Farbstringtabelle zur bunten Ausgabe
#define NUMBER_OF_COLORS            28
static const char Color_Table[NUMBER_OF_COLORS][16] = {
  {"\033[1;37m\033[41m"}, {"\033[1;37m\033[42m"}, {"\033[1;37m\033[43m"}, {"\033[1;37m\033[44m"},
  {"\033[1;37m\033[45m"}, {"\033[1;37m\033[46m"}, {"\033[1;37m\033[47m"}, {"\033[1;33m\033[41m"},
  {"\033[1;33m\033[42m"}, {"\033[1;33m\033[43m"}, {"\033[1;33m\033[44m"}, {"\033[1;33m\033[45m"},
  {"\033[1;33m\033[46m"}, {"\033[1;33m\033[47m"}, {"\033[1;36m\033[41m"}, {"\033[1;36m\033[42m"},
  {"\033[1;36m\033[43m"}, {"\033[1;36m\033[44m"}, {"\033[1;36m\033[45m"}, {"\033[1;36m\033[46m"},
  {"\033[1;36m\033[47m"}, {"\033[1;32m\033[41m"}, {"\033[1;32m\033[42m"}, {"\033[1;32m\033[43m"},
  {"\033[1;32m\033[44m"}, {"\033[1;32m\033[45m"}, {"\033[1;32m\033[46m"}, {"\033[1;32m\033[47m"}
};
// Farbstring bei unbunter Ausgabe
static const char Color_Empty[]="";

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Private Funktionen
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Gibt Anzahl übergebener Namen zurück
// Als Name zählt alles was nicht mit "-" beginnt
static int parser_count_names (const int argc, char* argv[])
{
  int   excludes_cnt=0;
  int   name_cnt=0;
  int   param_cnt;
  
  // Anzahl Namen und "_Namen" ermitteln
  for (param_cnt=1; param_cnt < argc; param_cnt++) {
    if (argv[param_cnt][0] != '-') {
      name_cnt++;
      if (argv[param_cnt][0] == '_') excludes_cnt++;
    }
  }
  // Falls nur Minusnamen wird 0 zurückgegeben, nach irgendwas muß ja gesucht werden
  if (name_cnt == excludes_cnt) return 0;
  else return name_cnt;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Füllt Namensliste (Speicher muß vorher in passender Größe angefordert sein)
// Übergabe:  CMD Params
//            Pointer auf Namensliste
// Rückgabe:  Statuskonstannte
//            list.items gefüllt 
static int parser_fill_nameslist (const int argc, char* argv[], nameslist_t *list)
{
  char    *param;
  int     code_cnt, param_cnt;
  int     stat;
  int     color_cnt=0;
  bool    color_on = true;
  name_t  *item = list->items;
   
  // Parameter -f ermitteln (Farbe aus), Voreinstellung an
  for (param_cnt=1; param_cnt < argc; param_cnt++) {
    param = argv[param_cnt];
    if (strcmp(param, "-f") == 0) {
      color_on = false;
      break;
    }
  }
 
  // Namensliste füllen, Ende wenn zu lang, kurz, undeutsch usw.
  for (param_cnt=1; param_cnt < argc; param_cnt++) {
    param = argv[param_cnt];
    if (param[0] != '-') {
      // Minusnamen
      if (param[0] == '_') {
        param++;
        // dem "_" folgt nichts ? (Ein Buchstabe ist Minimum)
        if (strlen(param) == 0) return STAT_ERR_UNDERLEN_WORD;
        // Name nach Typ word_t wandeln, Ende wenn kein Wort, zu Lang
        stat = str_to_word (param, item->name_norm); 
        if (stat != STAT_SUCCESS) return stat;
        // Name gültig... name_upper, exclude setzten
        strcpy (item->name_upper, param);
        str_to_ascii_upper (item->name_upper);
        item->exclude = true;
        // Codes mit Leerstring füllen (Sicherheitshalber)
        for (code_cnt = 0; code_cnt < PH_NUMBER_OF_CODES; code_cnt++) {
          item->code[code_cnt][0] = '\0';
        }
        // Farbe auf Leerstring zeigen lassen 
        item->color = Color_Empty;
        // Item auf nächsten Namen, nächsten Parameter prüfen
        item++;
        continue;
      }
      // Plusnamen
      else {
        // 2 Buchstaben Minumum für Name
        if (strlen(param) < 2) return STAT_ERR_UNDERLEN_WORD;
        // Name nach Typ word_t wandeln, Ende wenn kein Wort, zu Lang
        stat = str_to_word (param, item->name_norm); 
        if (stat != STAT_SUCCESS) return stat;
        // Name gültig... name_upper, exclude setzten
        strcpy (item->name_upper, param);
        str_to_ascii_upper (item->name_upper);
        item->exclude = false;
        // Alle Codes erstellen, egal was für Parameter gewählt sind
        for (code_cnt = 0; code_cnt < PH_NUMBER_OF_CODES; code_cnt++) {
          phonetics_get_code (item->name_norm, item->code[code_cnt], code_cnt);
        }
        // Farbe setzten
        if (color_on == false) item->color = Color_Empty;
        else {
          item->color = Color_Table[color_cnt % NUMBER_OF_COLORS];
          color_cnt++;
        }
        // Item auf nächsten Namen, nächsten Parameter prüfen
        item++;
        continue;
      }
    }
  }
  
  return STAT_SUCCESS;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Öffentliche Funktionen
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Namensliste erzeugen
// Übergabe:  CMD Args
//            Pointer auf Namensliste list
// Rückgabe:  Statuskonstannte
//            list gefüllt
int parse_nameslist (const int argc, char* argv[], nameslist_t *list)
{
  list->number_of_names = 0;
  list->items = NULL;
  
  // Anzahl Namen aus Übergabe ermitteln, Ende wenn keine Namen überrgeben
  // Speicher für items in Namesliste anfordern, Ende bei MEM Error
  list->number_of_names = parser_count_names(argc, argv);
  if (list->number_of_names == 0) return STAT_ERR_PARAM;
//  list->items = (name_t*) malloc( (list->number_of_names) * sizeof( name_t ));

  list->items = malloc ((list->number_of_names) * sizeof (name_t));


  if (list->items == NULL) return STAT_ERR_MEM;
  
  // Namensliste füllen, Ende bei Fehler
  return (parser_fill_nameslist (argc, argv, list));
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Optionen Parsen
// Übergabe:  CMD Args
//            Pointer auf Optionen
// Rückgabe:  Statuskonstannte
//            Optionen gefüllt
int parse_options (const int argc, char* argv[], phops_t *phops, frmops_t *frmops, outops_t *outops)
{
  int   code_cnt;
  int   param_cnt;
  char  *param;
  bool  phparam;
  bool  max_set = false;
  bool  min_set = false;
  
  // Startinitit Optionen
  phops->lev = 0; phops->min = 0; phops->max = 0;
  for (code_cnt = 0; code_cnt < PH_NUMBER_OF_CODES; code_cnt++) phops->show[code_cnt] = false;
  frmops->n = false; frmops->x = true; frmops->v = false;
  outops->a = false; outops->z = false; outops->w = false; outops->c = false; 

  // Optionen aus Übergabe lesen und die Vars danach setzten
  // Optionen ist alles was mit '-' beginnt
  for (param_cnt=1; param_cnt < argc; param_cnt++) {
    param = argv[param_cnt];
    if (param[0] != '-') continue;
    param++;
    
    // Hilfe
    if (strcmp (param, "h") == 0) return STAT_ERR_HELP;
    
    // -f wird hier nicht ausgewertet, sondern beim Namensliste erstellen
    if (strcmp (param, "f") == 0) continue;
    
    // Parameter phonetische Codevarianten
    phparam = false;
    for (code_cnt = 0; code_cnt < PH_NUMBER_OF_CODES; code_cnt++) {
      if (strcmp(param, PhCodeSign[code_cnt]) == 0) {
        phops->show[code_cnt] = true;
        phparam = true;
        break;
      }
    }
    if (phparam == true) continue;
    
    // Minimal-, Maximallängen Worte,
    // bool Hilfsvar setzten wenn gewählt
    if (strncmp (param, "-min=", 5) == 0) {
      param+=5;
      phops->min = atoi (param);
      min_set = true;
      continue;
    }
    if (strncmp (param, "-max=", 5) == 0) {
      param+=5;
      phops->max = atoi (param);
      max_set = true;
      continue;
    }
    
    // Lev gehört mit zu phonetik Param
    if (strcmp (param, "l") == 0) phops->lev++;
    
    // Formatparam
    else if (strcmp (param, "n") == 0) frmops->n = true;
    else if (strcmp (param, "x") == 0) frmops->x = false;
    else if (strcmp (param, "v") == 0) frmops->v = true;
    
    // Outops
    else if (strcmp (param, "a") == 0) outops->a = true;
    else if (strcmp (param, "z") == 0) outops->z = true;
    else if (strcmp (param, "w") == 0) outops->w = true;
    else if (strcmp (param, "c") == 0) outops->c = true;
    
    // Unbekannter Parameter
    else return STAT_ERR_PARAM;
  }
  
  // Wenn keine Ausgabeop gesetzt, -z einschalten (Zeilen ausgeben)
  if (outops->a == false && outops->z == false && outops->w == false && outops->c == false) outops->z = true;

  // in phonops bleiben .min / .max auf 0 gesetzt, wenn der jeweilige Parameter nicht übergeben wurde
  // Andernfalls sind sie mit den Übergaben gefüllt.. Logische Prüfung
  if (min_set == true) {
    if (phops->min <= 0) return STAT_ERR_PARAM;
  }
  if (max_set == true) {
    if (phops->max < phops->min) return STAT_ERR_PARAM;
  }
  
  return STAT_SUCCESS;
}
