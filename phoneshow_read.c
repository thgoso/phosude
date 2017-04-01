/*
phoneshow_read

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
#include "buffer_sizes.h"
#include "phoneshow_read.h"


static char     Cur_Line[BUFFER_SIZE_LINE];
static size_t   Cur_Read_Pos=0;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Liest eine Zeile von stdin ein und speichert in Modulvariable Cur_Line
int read_line (void)
{
  size_t  size;
    
  Cur_Line[0]='\0';                                                   // Ziel leeren
  
  if(fgets(Cur_Line, BUFFER_SIZE_LINE, stdin) != NULL) {              // Block lesen
    size=strlen(Cur_Line);
    if (size == BUFFER_SIZE_LINE - 1) return READ_LINE_OVERSIZE;      // Wenn Block zu lang Ende
    else {                                                            // Wenn Block keine Überlänge hat gültige Zeile
      if (Cur_Line[size-1] == '\n') Cur_Line[size-1]='\0';            // Zeilenendzeichen entfernen
      return READ_LINE_SUCCESS;                                       // Alles klar
    }
  }
  return READ_LINE_END;                                               // Wenn fgets NULL-Pointer liefert
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Init für Funktion read_word
// Setzt modulare Variable Cur_Read_Pos auf 0
// Somit kann man die Zeile auch mehrmals durchlaufen
void read_word_init (void)
{
  Cur_Read_Pos=0;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Holt das nächste gültige deutsche Wort oder einzelnes Sonderzeichen aus Cur_Line und gibt es in dest zurück
int read_word (char *dest)
{ 
  const char      uml[7][3] = {{"Ä"}, {"Ö"}, {"Ü"}, {"ä"}, {"ö"}, {"ü"}, {"ß"}};
  size_t          writepos=0;
  int             word_mode=0;
  int             cnt;
  
  // Ausgabe leeren
  dest[0]='\0';
  // Text komplett durch
  if (Cur_Line[Cur_Read_Pos] == '\0') return READ_WORD_END;
  
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
    return READ_WORD_IS_NO_WORD;
  }

NextChar:
  if (writepos >= BUFFER_SIZE_WORD) return READ_WORD_OVERSIZE;
  // So lange Zeichen rüberkopieren wie Buchstaben kommen
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
  return READ_WORD_IS_WORD;
}
