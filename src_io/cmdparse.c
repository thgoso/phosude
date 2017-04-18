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
#include "../phosude.h"

// Farbstringtabelle zur bunten Ausgabe
#define NUMBER_OF_COLORS 28
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
// Gibt Anzahl übergebener Namen zurück
// Als Name zählt alles was nicht mit "-" beginnt
int parser_count_names (const int argc, char* argv[])
{
  int   excludes_cnt=0;
  int   name_cnt=0;
  int   param_cnt;
  
  // Anzahl Namen gesammt und "_Namen" ermitteln
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
int parser_fill_nameslist (const int argc, char* argv[], nameslist_t *list)
{
  char    *param;
  int     param_cnt;
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
      // Minusnamen... Ein Buchstabe ist Minimum, Maximum = BUFFSIZE_WORD
      // .exclude = true, .name_norm, .name_upper setzten Rest leer lassen
      if (param[0] == '_') {
        param++;
        if (strlen(param) == 0) return STAT_ERR_NAME_UNDERLEN;
        stat = str_to_word (param, item->name_norm); 
        if (stat == STR_TO_WORD_ERR_NO_WORD) return STAT_ERR_NAME_NO_WORD;
        if (stat == STR_TO_WORD_ERR_OVERLEN) return STAT_ERR_NAME_OVERLEN;
        strcpy (item->name_upper, param);
        str_to_ascii_upper (item->name_upper);
        item->exclude = true;
        item++;
        continue;
      }
      // Plusnamen ... Zwei Buchstaben sind Minimum, Maximum = BUFFSIZE_WORD
      // Alles setzen
      else {
        if (strlen(param) < 2) return STAT_ERR_NAME_UNDERLEN;
        stat = str_to_word (param, item->name_norm); 
        if (stat == STR_TO_WORD_ERR_NO_WORD) return STAT_ERR_NAME_NO_WORD;
        if (stat == STR_TO_WORD_ERR_OVERLEN) return STAT_ERR_NAME_OVERLEN;
        strcpy (item->name_upper, param);
        str_to_ascii_upper (item->name_upper);
        item->exclude = false;
        make_cologne (item->name_norm, item->code_k);
        make_soundex (item->name_norm, item->code_s);
        make_exsoundex (item->name_norm, item->code_e);
        make_phonem (item->name_norm, item->code_p);
        make_caverphone (item->name_norm, item->code_c);
        make_daitchmok (item->name_norm, item->code_d);
        if (color_on == false) item->color = Color_Empty;
        else {
          item->color = Color_Table[color_cnt % NUMBER_OF_COLORS];
          color_cnt++;
        }
        item++;
        continue;
      }
    }
  }
  
  return STAT_SUCCESS;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Optionen Parsen
// Übergabe:  CMD Args
//            Pointer auf Optionen
// Rückgabe:  Statuskonstannte
//            Optionen gefüllt
int parse_options (const int argc, char* argv[], phops_t *phops, frmops_t *frmops, outops_t *outops)
{
  int   param_cnt;
  char  *param;
  bool  max_set = false;
  bool  min_set = false;
  
  // Startinitit Optionen
  phops->lev = 0; phops->min = 0; phops->max = 0;
  phops->show_k = false; phops->show_p = false; phops->show_s = false;
  phops->show_e = false; phops->show_c = false; phops->show_d = false;
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
    // Lev Dist
    if (strcmp (param, "l") == 0) phops->lev++;
    // Optionen Phonetik
    else if (strcmp (param, "K") == 0) phops->show_k = true;
    else if (strcmp (param, "P") == 0) phops->show_p = true;
    else if (strcmp (param, "S") == 0) phops->show_s = true;
    else if (strcmp (param, "E") == 0) phops->show_e = true;
    else if (strcmp (param, "C") == 0) phops->show_c = true;
    else if (strcmp (param, "D") == 0) phops->show_d = true;
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
  
  // Prüfen on reine Textsuche oder phonetische Suche... phops.textmode danach setzten
  if ((phops->show_k == false) && (phops->show_p == false) && (phops->show_s == false) &&
    (phops->show_e == false) && (phops->show_c == false) && (phops->show_d == false)) phops->textmode = true;
  else phops->textmode = false;

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
