/*
phosude stdin.c
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

#include <stdio.h>
#include <string.h>
#include "../phosude.h"

// Zeilenbuffer, Größe ggf erhöhen, sollte aber für normale Textdaten reichen
#define BUFFSIZE_LINE 4096

static char     Cur_Line[BUFFSIZE_LINE];    // Zeilenpuffer
static size_t   Cur_Read_Pos=0;             // Aktuelle Leseposition für get_word

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Init für Funktion stdin_get_word... setzt modulare Variable Cur_Read_Pos auf 0
// Somit kann man die Zeile auch mehrmals durchlaufen um Worte zu extrahieren
void stdin_carrige_return (void)
{
  Cur_Read_Pos=0;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Gibt Cur_Read_Pos zurück
size_t stdin_get_carrige_pos (void)
{
  return Cur_Read_Pos;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Liest eine Zeile von stdin ein und speichert in Modulvariable Cur_Line
int stdin_read_line (void)
{
  size_t  size;

  // Buffer leeren
  Cur_Line[0]='\0';

  if(fgets(Cur_Line, BUFFSIZE_LINE, stdin) == NULL) return STAT_END_REACHED;
  else {
    size=strlen(Cur_Line);
    if (size == BUFFSIZE_LINE - 1) return STAT_ERR_REC_OVERLEN_LINE;
    else {
      // Zeilenumbruch entfernen
      if (Cur_Line[size-1] == '\n') Cur_Line[size-1]='\0';
      return STAT_SUCCESS;
    }
  }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Holt das nächste gültige deutsche Wort oder einzelnes Sonderzeichen aus Cur_Line und gibt es in dest zurück
int stdin_get_word (word_t dest)
{
  const char  uml[7][3] = {{"Ä"}, {"Ö"}, {"Ü"}, {"ä"}, {"ö"}, {"ü"}, {"ß"}};
  size_t      writepos=0;
  int         word_mode=0;
  int         cnt;

  // Rückgabe leeren
  dest[0]='\0';
  
  // Zeile komplett durch ?
  if (Cur_Line[Cur_Read_Pos] == '\0') return STAT_END_REACHED;

  // 1. Zeichen bestimmen Buchstabe oder Sonderzeichen
  if ((Cur_Line[Cur_Read_Pos] >= 'a' && Cur_Line[Cur_Read_Pos] <= 'z') ||
    (Cur_Line[Cur_Read_Pos] >= 'A' && Cur_Line[Cur_Read_Pos] <= 'Z')) {
      word_mode = 1;
  }
  else {
    for (cnt=0; cnt<=6; cnt++) {
      if ((Cur_Line[Cur_Read_Pos] == uml[cnt][0]) && (Cur_Line[Cur_Read_Pos+1] == uml[cnt][1])) {
        word_mode = 1;
        break;
      }
    }
  }

  // War Sonderzeichen, dieses in dest schieben und raus
  if (word_mode == 0) {
    dest[0]=Cur_Line[Cur_Read_Pos];
    dest[1]='\0';
    Cur_Read_Pos++;
    return STAT_IS_SPECIAL_CHAR;
  }

NextChar:
  if (writepos >= (BUFFSIZE_WORD - 2)) return STAT_ERR_REC_OVERLEN_WORD;
  // So lange Zeichen rüberkopieren wie deutsche Buchstaben kommen
  if ((Cur_Line[Cur_Read_Pos] >= 'a' && Cur_Line[Cur_Read_Pos] <= 'z') ||
    (Cur_Line[Cur_Read_Pos] >= 'A' && Cur_Line[Cur_Read_Pos] <= 'Z')) {
      dest[writepos] = Cur_Line[Cur_Read_Pos];
      dest[writepos+1] = '\0';
      Cur_Read_Pos++;
      writepos++;
      goto NextChar;
  }
  for (cnt=0; cnt<=6; cnt++) {
    if ((Cur_Line[Cur_Read_Pos] == uml[cnt][0]) && (Cur_Line[Cur_Read_Pos+1] == uml[cnt][1])) {
      dest[writepos] = Cur_Line[Cur_Read_Pos];
      dest[writepos+1] = Cur_Line[Cur_Read_Pos+1];
      dest[writepos+2] = '\0';
      Cur_Read_Pos+=2;
      writepos+=2;
      goto NextChar;
    }
  }
  return STAT_IS_WORD;
}
