/*
Basisfunktionen Textkonvertierung und phonetische Verfahren

	Copyright (C) 2015, Thomas Gollmer, th_goso@freenet.de
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
#include "basis_func.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Prüft ob Text ein deutsches Wort ist, das ist es wenn ausschließlich deutsche Buchstaben im String
//enthalten sind NICHTS anderes (Auch kein Space) !!! Falls OK wird "TEXT_OK" zurückgegeben, sonst  "TEXT_NICHT_OK"
int pruefe_wort_deutsch (char *text)
{
	int	cnt=0;
	char	letter[3];
	int	ulcnt;
	int	gd;
	const	UL_FORM ul_liste[]={{"Ä"},{"Ö"},{"Ü"},{"ä"},{"ö"},{"ü"},{"ß"}};

	while (text[cnt] != '\0')
	{
		//A-Z, a-z
		if ((text[cnt] >= 'A' && text[cnt] <= 'Z') || (text[cnt] >= 'a' && text[cnt] <= 'z'))
		{
			cnt++;
			continue;
		}
		//Alle Umlaute 0-7
		gd=0;
		for (ulcnt=0; ulcnt <=6; ulcnt++)
		{
			strcpy(letter, ul_liste[ulcnt]);
			if (text[cnt] == letter[0] && text[cnt + 1] == letter[1])
			{
				cnt+=2;
				gd=1;
				break;
			}
		}
		if (gd == 1) continue;
		//Verbotenes Zeichen
		return (TEXT_NICHT_OK);
	}
	return (TEXT_OK);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Konvertiert alle Umlaute in Einzelbuchstaben
void conv_umlaute (char *text)
{
	int	cnt=0;
	char	letter[3];
	int	ulcnt;
	const	UL_FORM ul_liste[]        = {{"Ä"}, {"Ö"}, {"Ü"}, {"ä"}, {"ö"}, {"ü"}, {"ß"}};
	const	UL_FORM ul_ersatz_liste[] = {{"AE"},{"OE"},{"UE"},{"ae"},{"oe"},{"ue"},{"ss"}};

	while (text[cnt] != '\0')
	{
		for (ulcnt = 0; ulcnt <= 6; ulcnt++)
		{
			strcpy(letter, ul_liste[ulcnt]);
			if (text[cnt] == letter[0] && text[cnt + 1] == letter[1])
			{
				strcpy(letter, ul_ersatz_liste[ulcnt]);
				text[cnt] = letter[0];
				text[cnt + 1] = letter[1];
				cnt++;
				break;
			}
		}
		cnt++;
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Wandelt alle Kleinbuchstaben im String in Großbuchstaben (a-z), Umlaute, ß wird zu SS
void conv_grossbuchstaben (char *text)
{
	int	cnt=0;
	char	letter[3];
	int	ulcnt;
	const	UL_FORM ul_liste[]        = {{"ä"},{"ö"},{"ü"},{"ß"}};
	const	UL_FORM ul_ersatz_liste[] = {{"Ä"},{"Ö"},{"Ü"},{"SS"}};

	while (text[cnt] != '\0')
	{
		//a-z
		if (text[cnt] >= 'a' && text[cnt] <= 'z')
		{
			text[cnt] -= 32;
			cnt++;
			continue;
		}
		//kleine Umlaute 0-3
		for (ulcnt = 0; ulcnt <= 3; ulcnt++)
		{
			strcpy(letter, ul_liste[ulcnt]);
			if (text[cnt] == letter[0] && text[cnt + 1] == letter[1])
			{
				strcpy(letter, ul_ersatz_liste[ulcnt]);
				text[cnt] = letter[0];
				text[cnt + 1] = letter[1];
				cnt++;
				break;
			}
		}
		cnt++;
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Wandelt alle Großbuchstaben im String in Kleinbuchstaben (A-Z), Umlaute
void conv_kleinbuchstaben (char *text)
{
	int	cnt=0;
	char	letter[3];
	int	ulcnt;
	const	UL_FORM ul_liste[]        = {{"Ä"},{"Ö"},{"Ü"}};
	const	UL_FORM ul_ersatz_liste[] = {{"ä"},{"ö"},{"ü"}};

	while (text[cnt] != '\0')
	{
		//A-Z
		if (text[cnt] >= 'A' && text[cnt] <= 'Z')
		{
			text[cnt] += 32;
			cnt++;
			continue;
		}
		//Große Umlaute 0-2
		for (ulcnt = 0; ulcnt <= 2; ulcnt++)
		{
			strcpy(letter, ul_liste[ulcnt]);
			if (text[cnt] == letter[0] && text[cnt + 1] == letter[1])
			{
				strcpy(letter, ul_ersatz_liste[ulcnt]);
				text[cnt] = letter[0];
				text[cnt + 1] = letter[1];
				cnt++;
				break;
			}
		}
		cnt++;
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//entfernt einzelne Zeichen im Text die dem übergebenen Zeichen entsprechen, sodaß dieses Zeichen gelöscht wird
//Text darf KEINE Mehrbytezeichen enthalten
void entferne_einbytezeichen (char *text, int zeichen)
{
	int	cnt=0;
	char	tmp[MAX_WORT_LAENGE];
	char	letter[2];

	letter[1]='\0';
	tmp[0]='\0';

	do
	{
		if (text[cnt] != zeichen)
		{
			letter[0] = text[cnt];
			strcat (tmp, letter);
		}
		cnt++;
	}
	while (text[cnt] != '\0');
	strcpy(text, tmp);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//entfernt alle mehrfach hintereinander auftretenden Zeichen im Text, sodaß nur noch je eines vorhanden ist
//Text darf KEINE Mehrbytezeichen enthalten
void entferne_doppelte_einbytezeichen (char *text)
{
	int	cnt=0;
	int	letzter=0;
	char	tmp[MAX_WORT_LAENGE];
	char	letter[2];

	letter[1]='\0';
	tmp[0]='\0';

	do
	{
		if (text[cnt] != letzter)
		{
			letter[0] = text[cnt];
			strcat (tmp, letter);
			letzter = text[cnt];
		}
		cnt++;
	}
	while (text[cnt] != '\0');
	strcpy(text, tmp);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Kölner Phonetik Verfahren
//Wort wird in Gruppen von 3 Buchstaben gelesen, da es Regeln für "NACH" und "VOR" Buchstabe gibt
//Der Referenzbuchstabe ist immer der in der Mitte der 3er Gruppe
void conv_koelner(char *quelle_koelner, char *ziel_koelner)
{
	char	text[MAX_WORT_LAENGE]="\0";
	char	code[MAX_WORT_LAENGE]="\0";
	char	gruppe[3];
	int	laenge;
	int	cnt;

	//Wegen lesen in 3er Gruppen erweitern wir den übergebenen String
	//nachdem er in text kopiert ist hinten um vorn um ein Zeichen
	strcat (text, " ");
	strcat (text, quelle_koelner);
	strcat (text, " ");
	laenge = strlen(text);

	for (cnt=1; cnt<=laenge - 2; cnt++)
	{
		//In 3er Gruppen lesen
		gruppe[0]=text[cnt - 1];
		gruppe[1]=text[cnt];
		gruppe[2]=text[cnt + 1];

		//Sonderfälle Wortanfang
		if (cnt==1)
		{
			if (gruppe[1] == 'c')
			{
				if (gruppe[2] == 'a' || gruppe[2] == 'h' || gruppe[2] == 'k' ||
				    gruppe[2] == 'l' || gruppe[2] == 'o' || gruppe[2] == 'q' ||
				    gruppe[2] == 'r' || gruppe[2] == 'u' || gruppe[2] == 'x')
				{
					strcat(code, "4");
					continue;
				}
				else
				{
					strcat(code, "8");
					continue;
				}
			}
		}

		//Normale codierung KEIN Wortanfang
		if (gruppe[1] == 'p' && gruppe[2] == 'h')
		{
			strcat (code, "3");
			continue;
		}
		if ((gruppe[1] == 'd' && gruppe[2] == 's') || (gruppe[1] == 'd' && gruppe[2] == 'c') ||
		    (gruppe[1] == 'd' && gruppe[2] == 'z') || (gruppe[1] == 't' && gruppe[2] == 's') ||
		    (gruppe[1] == 't' && gruppe[2] == 'c') || (gruppe[1] == 't' && gruppe[2] == 'z'))
		{
			strcat(code, "8");
			continue;
		}
		if (gruppe[1] == 'd' || gruppe[1] == 't')
		{
			strcat(code, "2");
			continue;
		}
		if ((gruppe[0] == 'c' && gruppe[1] == 'x') || (gruppe[0] == 'k' && gruppe[1] == 'x') ||
		    (gruppe[0] == 'q' && gruppe[1] == 'x'))
		{
			strcat(code, "8");
			continue;
		}
		if (gruppe[1] == 'x')
		{
			strcat(code, "48");
			continue;
		}
		if ((gruppe[0] == 's' && gruppe[1] == 'c') || (gruppe[0] == 's' && gruppe[1] == 'z'))
		{
			strcat(code, "8");
			continue;
		}
		if (gruppe[1] == 'c')
		{
			if (gruppe[2] == 'a' || gruppe[2] == 'o' || gruppe[2] == 'u' || gruppe[2] == 'h' ||
			    gruppe[2] == 'k' || gruppe[2] == 'x' || gruppe[2] == 'q')
			{
				strcat(code, "4");
				continue;
			}
			else
			{
				strcat(code, "8");
				continue;
			}
		}
		if (gruppe[1] == 'h')
		{
			strcat(code, "h");
			continue;
		}
		if (gruppe[1] == 'a' || gruppe[1] == 'e' || gruppe[1] == 'i' || gruppe[1] == 'j' ||
		    gruppe[1] == 'y' || gruppe[1] == 'o' || gruppe[1] == 'u')
		{
			strcat(code, "0");
			continue;
		}
		if (gruppe[1] == 'b' || gruppe[1] == 'p')
		{
			strcat(code, "1");
			continue;
		}
		if (gruppe[1] == 'f' || gruppe[1] == 'v' || gruppe[1] == 'w')
		{
			strcat(code, "3");
			continue;
		}
		if (gruppe[1] == 'g' || gruppe[1] == 'k' || gruppe[1] == 'q')
		{
			strcat(code, "3");
			continue;
		}
		if (gruppe[1] == 'l')
		{
			strcat(code, "5");
			continue;
		}
		if (gruppe[1] == 'r')
		{
			strcat(code, "7");
			continue;
		}
		if (gruppe[1] == 'm' || gruppe[1] == 'n')
		{
			strcat(code, "6");
			continue;
		}
		if (gruppe[1] == 's' || gruppe[1] == 'z')
		{
			strcat(code, "8");
			continue;
		}
	}
	//doppelte entfernen, h entfernen
	entferne_doppelte_einbytezeichen (code);
	entferne_einbytezeichen (code, 'h');
	//0 außer am Anfang entfernen Rückgabe füllen
	ziel_koelner[0]='\0';
	if (code[0] == '0')
	{
		ziel_koelner[0]='0';
		ziel_koelner[1]='\0';
	}
	entferne_einbytezeichen (code, '0');
	strcat (ziel_koelner, code);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Phonem Verfahren
void conv_phonem(char *quelle_phonem, char *ziel_phonem)
{
	int	cnt=0;
	char	txt[MAX_WORT_LAENGE];
	char	tmp[5];

	//1. Durchgang = Buchstabenpaare kopieren/nach Regeln codieren
	//               Da es mit dem ö zu Problemen kommt, wird es vorerst als 0 kodiert
	txt[0]='\0';
	while (quelle_phonem[cnt] != '\0')
	{
		//ae
		if (quelle_phonem[cnt] == 'a' && quelle_phonem[cnt + 1] == 'e')
		{
			strcat(txt, "e");
			cnt = cnt + 2;
		}
		//pf
		else if (quelle_phonem[cnt] == 'p' && quelle_phonem[cnt + 1] == 'f')
		{
			strcat(txt, "v");
			cnt = cnt + 2;
		}
		//ks
		else if (quelle_phonem[cnt] == 'k' && quelle_phonem[cnt + 1] == 's')
		{
			strcat(txt, "x");
			cnt = cnt + 2;
		}
		//qu
		else if (quelle_phonem[cnt] == 'q' && quelle_phonem[cnt + 1] == 'u')
		{
			strcat(txt, "kw");
			cnt = cnt + 2;
		}
		//oe
		else if (quelle_phonem[cnt] == 'o' && quelle_phonem[cnt + 1] == 'e')
		{
			strcat(txt, "0");
			cnt = cnt + 2;
		}
		//eu
		else if (quelle_phonem[cnt] == 'e' && quelle_phonem[cnt + 1] == 'u')
		{
			strcat(txt, "oy");
			cnt = cnt + 2;
		}
		//ou
		else if (quelle_phonem[cnt] == 'o' && quelle_phonem[cnt + 1] == 'u')
		{
			strcat(txt, "u");
			cnt = cnt + 2;
		}
		//ei,ey
		else if ((quelle_phonem[cnt] == 'e' && quelle_phonem[cnt + 1] == 'i') ||
			 (quelle_phonem[cnt] == 'e' && quelle_phonem[cnt + 1] == 'y'))
		{
			strcat(txt, "ay");
			cnt = cnt + 2;
		}
		//sc,sz,cz,tz,ts
		else if ((quelle_phonem[cnt] == 's' && quelle_phonem[cnt + 1] == 'c') ||
			 (quelle_phonem[cnt] == 's' && quelle_phonem[cnt + 1] == 'z') ||
		    	 (quelle_phonem[cnt] == 'c' && quelle_phonem[cnt + 1] == 'z') ||
			 (quelle_phonem[cnt] == 't' && quelle_phonem[cnt + 1] == 'z') ||
		    	 (quelle_phonem[cnt] == 't' && quelle_phonem[cnt + 1] == 's'))
		{
			strcat(txt, "c");
			cnt = cnt + 2;
		}

		else
		{
			//Wenn Buchstabenpaar nicht nach Regeln kodiert werden konnte
			//nur einen Buchstaben kopieren und weiter ab dem nächsten mit Paarsuche
			tmp[0]=quelle_phonem[cnt];
			tmp[1]='\0';
			strcat (txt, tmp);
			cnt = cnt + 1;
		}
	}

	//2. Durchgang einzelne Buchstaben ersetzten/kodieren
	cnt=0;
	while (txt[cnt] != '\0')
	{
		//z,k,g,q
		if (txt[cnt] == 'z' || txt[cnt] == 'k' || txt[cnt] == 'g' || txt[cnt] == 'q') txt[cnt] = 'c';
		//u,i,j
		else if (txt[cnt] == 'u' || txt[cnt] == 'i' || txt[cnt] == 'j') txt[cnt] = 'y';
		//f,w,v
		else if (txt[cnt] == 'f' || txt[cnt] == 'w') txt[cnt] = 'v';
		//a
		else if (txt[cnt] == 'a') txt[cnt] = 'e';
		//p
		else if (txt[cnt] == 'p') txt[cnt] = 'b';
		//t
		else if (txt[cnt] == 't') txt[cnt] = 'd';
		cnt++;
	}

	//3. Durchgang alle doppelten Zeichen entfernen
	entferne_doppelte_einbytezeichen (txt);

	//4. Durchgang alles in Rückgabestring kopieren, unerlaubte Zeichen dabei außlassen & 0 zu Ö wandeln, Großschreiben
	cnt=0;
	ziel_phonem[0]='\0';
	while (txt[cnt] != '\0')
	{
		if (txt[cnt] == 'a' || txt[cnt] == 'b' || txt[cnt] == 'c' || txt[cnt] == 'd' || txt[cnt] == 'l' ||
		    txt[cnt] == 'm' || txt[cnt] == 'n' || txt[cnt] == 'o' || txt[cnt] == 'r' || txt[cnt] == 's' ||
		    txt[cnt] == 'u' || txt[cnt] == 'v' || txt[cnt] == 'w' || txt[cnt] == 'x' || txt[cnt] == 'y')
		{
			tmp[0] = txt[cnt];
			tmp[1] = '\0';
			strcat (ziel_phonem, tmp);
		}
		else if (txt[cnt] == '0') strcat (ziel_phonem, "Ö\0");
		cnt++;
	}
	conv_grossbuchstaben (ziel_phonem);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Soundex Verfahren
void conv_soundex(char *quelle_soundex, char *ziel_soundex)
{
	int	cnt=1;
	char	txt[MAX_WORT_LAENGE];
	int	laenge;

	//1. Zeichen einfach kopieren in Rückgabe, Großschreiben
	ziel_soundex[0] = quelle_soundex[0];
	ziel_soundex[1] = '\0';
	conv_grossbuchstaben (ziel_soundex);

	//ab 2. Zeichen alles rüberkopieren nach txt
	while (quelle_soundex[cnt] != '\0')
	{
		txt[cnt - 1] = quelle_soundex[cnt];
		cnt++;
	}
	txt[cnt - 1]='\0';

	//Ersetzungsregeln auf txt anwenden
	cnt=0;
	while (txt[cnt] != '\0')
	{
		if (txt[cnt] == 'a' || txt[cnt] == 'e' || txt[cnt] == 'h' || txt[cnt] == 'i' ||
		    txt[cnt] == 'o' || txt[cnt] == 'u' || txt[cnt] == 'w' || txt[cnt] == 'y') txt[cnt] = '0';
		else if (txt[cnt] == 'b' || txt[cnt] == 'f' || txt[cnt] == 'p' || txt[cnt] == 'v') txt[cnt] = '1';
		else if (txt[cnt] == 'c' || txt[cnt] == 'g' || txt[cnt] == 'j' || txt[cnt] == 'k' ||
			 txt[cnt] == 'q' || txt[cnt] == 's' || txt[cnt] == 'x' || txt[cnt] == 'z') txt[cnt] = '2';
		else if (txt[cnt] == 'd' || txt[cnt] == 't') txt[cnt] = '3';
		else if (txt[cnt] == 'l') txt[cnt] = '4';
		else if (txt[cnt] == 'm' || txt[cnt] == 'n') txt[cnt] = '5';
		else if (txt[cnt] == 'r') txt[cnt] = '6';
		cnt++;
	}

	//Doppelte entfernen, dann 0en entfernen, an Rückgabe hängen, auf 4 Stellen kürzen/0en anhängen
	entferne_doppelte_einbytezeichen (txt);
	entferne_einbytezeichen (txt, '0');
	strcat (ziel_soundex, txt);
	laenge=strlen(ziel_soundex);

	if (laenge == 1) strcat (ziel_soundex, "000");
	else if (laenge == 2) strcat (ziel_soundex, "00");
	else if (laenge == 3) strcat (ziel_soundex, "0");
	else if (laenge > 4) ziel_soundex[4]='\0';
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Extended Soundex Verfahren
void conv_exsoundex(char *quelle_exsoundex, char *ziel_exsoundex)
{
	int	cnt=0;
	char	txt[MAX_WORT_LAENGE];

	//Quelle kopieren und Zeichen kodieren/ersetzten
	strcpy (txt, quelle_exsoundex);
	while (txt[cnt] != '\0')
	{
		if (txt[cnt] == 'a' || txt[cnt] == 'e' || txt[cnt] == 'h' || txt[cnt]== 'i' ||
		    txt[cnt] == 'o' || txt[cnt] == 'u' || txt[cnt] == 'w' || txt[cnt]== 'y') txt[cnt] = '0';
		else if (txt[cnt] == 'b' || txt[cnt] == 'p') txt[cnt] = '1';
		else if (txt[cnt] == 'v' || txt[cnt] == 'f') txt[cnt] = '2';
		else if (txt[cnt] == 'c' || txt[cnt] == 'k' || txt[cnt] == 's') txt[cnt] = '3';
		else if (txt[cnt] == 'g' || txt[cnt] == 'j') txt[cnt] = '4';
		else if (txt[cnt] == 'q' || txt[cnt] == 'x' || txt[cnt] == 'z') txt[cnt] = '5';
		else if (txt[cnt] == 'd' || txt[cnt] == 't') txt[cnt] = '6';
		else if (txt[cnt] == 'l') txt[cnt] = '7';
		else if (txt[cnt] == 'm' || txt[cnt] == 'n') txt[cnt] = '8';
		else if (txt[cnt] == 'r') txt[cnt] = '9';
		cnt++;
	}

	//Doppelte entfernen, 0en entfernen
	entferne_doppelte_einbytezeichen (txt);
	entferne_einbytezeichen (txt, '0');

	//Ausgabe mit 5 Stellen 0 füllen, txt rüberkopieren auf 5 Stellen kürzen
	strcpy (ziel_exsoundex, "00000");
	cnt=0;
	while (txt[cnt] != '\0')
	{
		ziel_exsoundex[cnt] = txt[cnt];
		cnt++;
	}
	ziel_exsoundex[5] = '\0';
}


