/*
Spezialfunktionen phonetische Suche in Text

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
#include <stdlib.h>
#include <string.h>
#include "basis_func.h"
#include "phoneshow_func.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Liest Daten von stdin und gibt Pointer darauf zurück.
//Bei Problemen mit Speicheranforderung wird NULL zurückgegeben
char *lies_eingabe(void)
{
	char	puffer[PUFFER_GROESSE];
	size_t	daten_laenge = 1;
	char	*daten = malloc(sizeof(char) * PUFFER_GROESSE);

	if(daten == NULL) return (NULL);

	//Über Puffer Eingabe von stdin lesen und komplett in daten speichern
	daten[0]='\0';
	while(fgets(puffer, PUFFER_GROESSE, stdin))
	{
		char *alt = daten;
		daten_laenge += strlen(puffer);
		daten=realloc(daten, daten_laenge);
		if(daten == NULL)
		{
			free(alt);
			return (NULL);
		}
		strcat(daten, puffer);
	}
	if(ferror(stdin))
	{
		free(daten);
		return (NULL);
	}
	return (daten);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Levenshtein-Distanz aus http://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance
//Berechnet die Levenshtein-Distanz zweier übergebener Strings
//Was in dem Fall auf das Wort direkt oder den erzeugten phonetischen Code angewandt werden kann
//Übergabe:	Wort1 und Wort2
//Rückgabe:	Distanz als Integer
int levenshtein(char *s1, char *s2)
{
	unsigned int x, y, s1len, s2len;
	s1len = strlen(s1);
	s2len = strlen(s2);
	unsigned int matrix[s2len+1][s1len+1];
	matrix[0][0] = 0;
	for (x = 1; x <= s2len; x++)
		matrix[x][0] = matrix[x-1][0] + 1;
	for (y = 1; y <= s1len; y++)
		matrix[0][y] = matrix[0][y-1] + 1;
	for (x = 1; x <= s2len; x++)
		for (y = 1; y <= s1len; y++)
			matrix[x][y] = MIN3(matrix[x-1][y] + 1, matrix[x][y-1] + 1, matrix[x-1][y-1] + (s1[y-1] == s2[x-1] ? 0 : 1));

	return(matrix[s2len][s1len]);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Holt das nächste gültige Wort aus "textdaten" und gibt TEXT_OK zurück
//Falls als nächstes kein Wort sondern ein "verbotenes" / nicht deutsches Zeichen kommt
//Wird das Zeichen allein zurückgegeben (plus Nullbyte) und Rückgabe = TEXT_NICHT_OK
//Ist das Datenende mal erreicht wird immer wieder '\0' in wort zurückgegeben
//Übergabe:	Pointer auf Textdaten
//Rückgabe:	Wort
//		TEXT_OK oder TEXT_NICHT_OK
//		aktualisiert auch txtposcnt wenn kein NULL-Zeiger übergeben
int hole_wort (char *textdaten, char *wort, long *txtposcnt)
{
	const		UL_FORM ul_liste[] = {{"Ä"}, {"Ö"}, {"Ü"}, {"ä"}, {"ö"}, {"ü"}, {"ß"}};
	static long	cnt=0;
	int		gd;
	char		tmp[4];
	int		ulcnt;

	wort[0]='\0';
	//textdaten Ende erreicht
	if (textdaten[cnt] == '\0') return (TEXT_NICHT_OK);

	while (1)
	{
		//Erlaubtes Zeichen an "wort" anfügen (A-Z, a-z)
		if ((textdaten[cnt] >= 'a' && textdaten[cnt] <= 'z') || (textdaten[cnt] >= 'A' && textdaten[cnt] <= 'Z'))
		{
			tmp[0]=textdaten[cnt];
			tmp[1]='\0';
			strcat(wort, tmp);
			cnt++;
			continue;
		}
		//Erlaubtes DoppelbyteZeichen an "wort" anfügen (Umlaute)
		gd=0;
		for (ulcnt=0; ulcnt<= 6; ulcnt++)
		{
			strcpy (tmp, ul_liste[ulcnt]);
			if (textdaten[cnt] == tmp[0] && textdaten[cnt + 1] == tmp[1])
			{
				strcat(wort, tmp);
				cnt+=2;
				gd=1;
				break;
			}
		}
		//Weiter wenn gültiges Zeichen war
		if (gd == 1) continue;
		else break;
	}
	//Verbotenes Zeichen angetroffen
	//aber Wort noch nicht zurückgegeben
	if (wort[0] != '\0')
	{
		if (txtposcnt != NULL) *txtposcnt = cnt;
		return (TEXT_OK);
	}
	else
	//einzelnes Zeichen
	{
		wort[0] = textdaten[cnt];
		wort[1] = '\0';
		if (txtposcnt != NULL) *txtposcnt = cnt;
		cnt++;
		return (TEXT_NICHT_OK);
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Prüft ob übergebener Name mit übergebenem Wort identisch ist (je nach phon. Opt)
//Übergabe:	Name
//		Wort
//		Optionen zur Phonetik
//Rückgabe:	TEXT_OK wenn mind. eine Übereinstimmung in eine der phon. Optionen
int pruefe_phongleich (NAME_FORM name, char *wort, OPT_FORM optionen)
{
	char	sw[MAX_WORT_LAENGE];
	char	sc[MAX_WORT_LAENGE];
	char	sww[MAX_WORT_LAENGE];
	int	lvst_dist;

	//Wenn keine phonetischen Optionen gewählt
	//Nur Lev Dist auf Name & Wort anwenden
	if (optionen.k == OP_AUS && optionen.p == OP_AUS && optionen.s == OP_AUS && optionen.e == OP_AUS)
	{
		strcpy (sw, name.Name);
		strcpy (sww, wort);
		conv_kleinbuchstaben (sw);
		conv_kleinbuchstaben (sww);
		lvst_dist = levenshtein (sw, sww);
		if (lvst_dist <= optionen.l) return (TEXT_OK);
		else return (TEXT_NICHT_OK);
	}

	//Phon Opt gewählt, also Wort in phone Code wandeln
	//mit jeweiligen phon. Code des Namens vergleichen
	strcpy (sw, wort);
	conv_kleinbuchstaben (sw);
	conv_umlaute (sw);

	if (optionen.k == OP_EIN)
	{
		conv_koelner (sw, sc);
		if (optionen.l != 0)
		{
			lvst_dist = levenshtein (sc, name.Code_k);
			if (lvst_dist <= optionen.l) return (TEXT_OK);
		}
		else if (strcmp (sc, name.Code_k) == 0) return (TEXT_OK);
	}
	if (optionen.p == OP_EIN)
	{
		conv_phonem (sw, sc);
		if (optionen.l != 0)
		{
			lvst_dist = levenshtein (sc, name.Code_p);
			if (lvst_dist <= optionen.l) return (TEXT_OK);
		}
		else if (strcmp (sc, name.Code_p) == 0) return (TEXT_OK);
	}
	if (optionen.s == OP_EIN)
	{
		conv_soundex (sw, sc);
		if (optionen.l != 0)
		{
			lvst_dist = levenshtein (sc, name.Code_s);
			if (lvst_dist <= optionen.l) return (TEXT_OK);
		}
		else if (strcmp (sc, name.Code_s) == 0) return (TEXT_OK);
	}
	if (optionen.e == OP_EIN)
	{
		conv_exsoundex (sw, sc);
		if (optionen.l != 0)
		{
			lvst_dist = levenshtein (sc, name.Code_e);
			if (lvst_dist <= optionen.l) return (TEXT_OK);
		}
		else if (strcmp (sc, name.Code_e) == 0) return (TEXT_OK);
	}

	return (TEXT_NICHT_OK);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Namen überprüfen ob gültige deutsche Worte,
//Ob sie nach den gewählten Optionen phon. Code generieren
//Ob sie Länger als 1 Buchstabe sind
//Bei Problemen wird Name/Option in stderr ausgegeben
//In Ergänzung zu "zeige_fehler" ist die Ausgabe dann passend
//Gibt Fehlercode zurück oder "TEXT_OK"

//Übergabe:	Namen Liste (Namen müssen ausgefüllt sein)
//		Anzahl Namen in Liste
//		Phonetik Optionen
//Rückgabe:	TEXT_OK oder entsprechender Fehlercode
//		In der Namen Liste werden die phon. Codes zum Namen "eingetragen"
//		und der Farbstring für den Namen (wenn opt.f = OP_AUS) Farbstring = '\0'

int pruefe_namen_liste (NAME_FORM namen_liste[], int anz_namen, OPT_FORM optionen)
{
	int	cnt;
	int	frbcnt=0;
	int	nur_minusnamen=1;
	char	txt[MAX_WORT_LAENGE];

	typedef char frb_frm[FARB_STRING_LAENGE];
	const	int anzahl = 28;	//Anzahl und Farbstrings hier anpassen
	const	frb_frm farben[] =     {{"\033[1;37m\033[41m"}, {"\033[1;37m\033[42m"},
					{"\033[1;37m\033[43m"}, {"\033[1;37m\033[44m"}, {"\033[1;37m\033[45m"},
					{"\033[1;37m\033[46m"}, {"\033[1;37m\033[47m"}, {"\033[1;33m\033[41m"},
					{"\033[1;33m\033[42m"}, {"\033[1;33m\033[43m"}, {"\033[1;33m\033[44m"},
					{"\033[1;33m\033[45m"}, {"\033[1;33m\033[46m"}, {"\033[1;33m\033[47m"},
					{"\033[1;36m\033[41m"}, {"\033[1;36m\033[42m"}, {"\033[1;36m\033[43m"},
					{"\033[1;36m\033[44m"}, {"\033[1;36m\033[45m"}, {"\033[1;36m\033[46m"},
					{"\033[1;36m\033[47m"}, {"\033[1;32m\033[41m"}, {"\033[1;32m\033[42m"},
					{"\033[1;32m\033[43m"}, {"\033[1;32m\033[44m"}, {"\033[1;32m\033[45m"},
					{"\033[1;32m\033[46m"}, {"\033[1;32m\033[47m"}};



	for (cnt = 0; cnt < anz_namen; cnt++)
	{
		strcpy(txt, namen_liste[cnt].Name);
		//Bei Minusname "_" entfernen und Ist_Minusname = OP_EIN
		//Keine phon. Codes erzeugen, keine Farbe erzeugen
		//Normale SuchNamen Ist_Minusname = OP_AUS setzten
		//phon. Codes erzeugen, Farbstring erzeugen
		if (txt[0] == '_')
		{
			entferne_einbytezeichen (txt, '_');
			strcpy(namen_liste[cnt].Name, txt);
			namen_liste[cnt].Ist_Minusname = OP_EIN;
		}
		else
		{
			namen_liste[cnt].Ist_Minusname = OP_AUS;
			if (optionen.f == OP_EIN) strcpy (namen_liste[cnt].Farbstring, farben[frbcnt % anzahl]);
			else namen_liste[cnt].Farbstring[0] = '\0';
			frbcnt++;
			nur_minusnamen=0;
		}
		//Länge < 2 = Fehler
		conv_kleinbuchstaben (txt);
		conv_umlaute (txt);
		if (strlen (txt) < 2)
		{
			fprintf (stderr, "%s ", txt);
			return (TEXT_ZU_KURZ);
		}
		//Nicht deutsches Wort = Fehler
		if (pruefe_wort_deutsch (txt) == TEXT_NICHT_OK)
		{
			fprintf (stderr, "%s ", txt);
			return (TEXT_NICHT_OK);
		}
		//Codes zum Namen erstellen je nach Optionen Phonetik
		//Fehler wenn ungültiger Code
		if ((optionen.k == OP_EIN) && (namen_liste[cnt].Ist_Minusname == OP_AUS))
		{
			conv_koelner (txt, namen_liste[cnt].Code_k);
			if (strlen (namen_liste[cnt].Code_k) < 1)
			{
				fprintf (stderr, "-k %s ", namen_liste[cnt].Name);
				return (TEXT_KEIN_CODE);
			}
		}
		if ((optionen.p == OP_EIN) && (namen_liste[cnt].Ist_Minusname == OP_AUS))
		{
			conv_phonem (txt, namen_liste[cnt].Code_p);
			if (strlen (namen_liste[cnt].Code_p) < 1)
			{
				fprintf (stderr, "-p %s ", namen_liste[cnt].Name);
				return (TEXT_KEIN_CODE);
			}
		}
		if ((optionen.s == OP_EIN) && (namen_liste[cnt].Ist_Minusname == OP_AUS))
		{
			conv_soundex (txt, namen_liste[cnt].Code_s);
			if (strlen (namen_liste[cnt].Code_s) < 1)
			{
				fprintf (stderr, "-s %s ", namen_liste[cnt].Name);
				return (TEXT_KEIN_CODE);
			}
		}
		if ((optionen.e == OP_EIN) && (namen_liste[cnt].Ist_Minusname == OP_AUS))
		{
			conv_exsoundex (txt, namen_liste[cnt].Code_e);
			if (strlen (namen_liste[cnt].Code_e) < 1)
			{
				fprintf (stderr, "-e %s ", namen_liste[cnt].Name);
				return (TEXT_KEIN_CODE);
			}
		}
	}
	if (nur_minusnamen == 1) return (TEXT_UNDEFINIERT);
	else return (TEXT_OK);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Ausgabe Option -a = ganzen Text ausgeben
//Übergabe:	Namenliste
//		Anzahl Namen in Liste
//		Optionen
//		Pointer auf Textdaten
//Rückgabe:	TEXT_OK wenn mind. ein Fund, wenn gar kein Fund TEXT_UNGLEICH
int ausgabe_a (NAME_FORM namen_liste[], int anz_namen, OPT_FORM optionen, char *textdaten)
{
	char	wort[MAX_WORT_LAENGE];
	char	line[MAX_ZEILE_LAENGE]="\0";
	int	namecnt;
	int	verboten;
	int	rv;
	long	funde=0;
	long	zeilecnt=1;

	while (1)
	{
		//Nächstes Wort abholen
		rv = hole_wort (textdaten, wort, NULL);
		//Wenn Textende erreichet Ende
		if (wort[0] == '\0') break;
		//Wenn Zeilenende erreicht line ausgeben
		else if (wort[0] == '\n')
		{
			if (optionen.n == OP_EIN) printf ("%ld %ld %s\n", zeilecnt, funde, line);
			else printf ("%s\n", line);
			line[0] = '\0';
			zeilecnt++;
		}
		//anderes Sonderzeichen
		else if (rv == TEXT_NICHT_OK) strcat (line, wort);

		//Wenn Wort
		else if (rv == TEXT_OK)
		{
			//Wenn Wort eines der verbotenen nicht weiter prüfen aber ausgeben
			verboten=0;
			for (namecnt=0; namecnt < anz_namen; namecnt++)
			{
				if (namen_liste[namecnt].Ist_Minusname == OP_EIN)
				{
					if (strcmp (wort, namen_liste[namecnt].Name) == 0)
					{
						verboten = 1;
						break;
					}
				}
			}
			if (verboten == 1)
			{
				strcat (line, wort);
				continue;
			}
			//Kein verbotenes Wort
			for (namecnt=0; namecnt < anz_namen; namecnt++)
			{
				//Name = verbotener Name, nicht prüfen
				if (namen_liste[namecnt].Ist_Minusname == OP_EIN) continue;
				//Prüfen ob phongleich
				if (pruefe_phongleich (namen_liste[namecnt], wort, optionen) == TEXT_OK)
				{
					//Farbstring in line schieben
					strcat (line, namen_liste[namecnt].Farbstring);
					funde++;
					break;
				}
			}
			//Wort an line anfügen, Farbausgabe aus
			strcat (line, wort);
			if (optionen.f == OP_EIN) strcat (line, "\033[m");
			continue;
		}
	}

	//Legende ausgeben
	zeige_legende (namen_liste, anz_namen, optionen, funde);

	if (funde != 0) return (TEXT_OK);
	else return (TEXT_UNGLEICH);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Ausgabe Option -z = nur Zeilen ausgeben
//Übergabe:	Namenliste
//		Anzahl Namen in Liste
//		Optionen
//		Pointer auf Textdaten
//Rückgabe:	TEXT_OK wenn mind. ein Fund, wenn gar kein Fund TEXT_UNGLEICH
int ausgabe_z (NAME_FORM namen_liste[], int anz_namen, OPT_FORM optionen, char *textdaten)
{
	char	wort[MAX_WORT_LAENGE];
	char	line[MAX_ZEILE_LAENGE]="\0";
	int	namecnt;
	int	verboten;
	int	rv;
	long	funde=0;
	long	zeilecnt=1;
	int	isinline=0;

	while (1)
	{
		//Nächstes Wort abholen
		rv = hole_wort (textdaten, wort, NULL);
		//Wenn Textende erreichet Ende
		if (wort[0] == '\0') break;
		//Wenn Zeilenende erreicht line ausgeben
		else if (wort[0] == '\n')
		{
			//Aber nur wenn Fund in Zeile
			if (isinline == 1)
			{
				if (optionen.n == OP_EIN) printf ("%ld %ld %s\n", zeilecnt, funde, line);
				else printf ("%s\n", line);
				isinline = 0;
			}
			//Zeile leeren
			line[0] = '\0';
			zeilecnt++;
		}
		//anderes Sonderzeichen
		else if (rv == TEXT_NICHT_OK) strcat (line, wort);

		//Wenn Wort
		else if (rv == TEXT_OK)
		{
			//Wenn Wort eines der verbotenen nicht weiter prüfen aber ausgeben
			verboten=0;
			for (namecnt=0; namecnt < anz_namen; namecnt++)
			{
				if (namen_liste[namecnt].Ist_Minusname == OP_EIN)
				{
					if (strcmp (wort, namen_liste[namecnt].Name) == 0)
					{
						verboten = 1;
						break;
					}
				}
			}
			if (verboten == 1)
			{
				strcat (line, wort);
				continue;
			}
			//Kein verbotenes Wort
			for (namecnt=0; namecnt < anz_namen; namecnt++)
			{
				//Name = verbotener Name, nicht prüfen
				if (namen_liste[namecnt].Ist_Minusname == OP_EIN) continue;
				//Prüfen ob phongleich
				if (pruefe_phongleich (namen_liste[namecnt], wort, optionen) == TEXT_OK)
				{
					//Farbstring in line schieben
					strcat (line, namen_liste[namecnt].Farbstring);
					funde++;
					isinline=1;
					break;
				}
			}
			//Wort an line anfügen, Farbausgabe aus
			strcat (line, wort);
			if (optionen.f == OP_EIN) strcat (line, "\033[m");
			continue;
		}
	}

	//Legende ausgeben
	zeige_legende (namen_liste, anz_namen, optionen, funde);

	if (funde != 0) return (TEXT_OK);
	else return (TEXT_UNGLEICH);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Ausgabe Option -w = nur Worte ausgeben
//Übergabe:	Namenliste
//		Anzahl Namen in Liste
//		Optionen
//		Pointer auf Textdaten
//Rückgabe:	TEXT_OK wenn mind. ein Fund, wenn gar kein Fund TEXT_UNGLEICH
int ausgabe_w (NAME_FORM namen_liste[], int anz_namen, OPT_FORM optionen, char *textdaten)
{
	char	wort[MAX_WORT_LAENGE];
	int	namecnt;
	int	rv;
	int	verboten;
	long	funde=0;
	long	zeilecnt=1;

	while (1)
	{
		//Nächstes Wort abholen
		rv = hole_wort (textdaten, wort, NULL);
		//Wenn Textende erreichet Ende
		if (wort[0] == '\0') break;
		//Wenn Zeilenende
		if (wort[0] == '\n') zeilecnt++;
		//Wenn Wort
		else if (rv == TEXT_OK)
		{
			//Wenn Wort eines der verbotenen nicht weiter prüfen
			verboten=0;
			for (namecnt=0; namecnt < anz_namen; namecnt++)
			{
				if (namen_liste[namecnt].Ist_Minusname == OP_EIN)
				{
					if (strcmp (wort, namen_liste[namecnt].Name) == 0)
					{
						verboten = 1;
						break;
					}
				}
			}
			if (verboten == 1) continue;
			//Kein verbotenes Wort
			for (namecnt=0; namecnt < anz_namen; namecnt++)
			{
				//Name = verbotener Name, nicht prüfen
				if (namen_liste[namecnt].Ist_Minusname == OP_EIN) continue;
				//Prüfen ob phongleich
				if (pruefe_phongleich (namen_liste[namecnt], wort, optionen) == TEXT_OK)
				{
					//Farbig ausgeben / oder nicht
					funde++;
					if (optionen.n == OP_EIN)
					{
						if (optionen.f == OP_EIN) printf ("%ld %ld %s%s\033[m\n",
										zeilecnt, funde, namen_liste[namecnt].Farbstring, wort);
						else printf ("%ld %ld %s\n", zeilecnt, funde, wort);
					}
					else
					{
						if (optionen.f == OP_EIN) printf ("%s%s\033[m\n",
										namen_liste[namecnt].Farbstring, wort);
						else printf("%s\n", wort);
					}
					break;
				}
			}
		}
	}
	//Legende ausgeben
	zeige_legende (namen_liste, anz_namen, optionen, funde);

	if (funde != 0) return (TEXT_OK);
	else return (TEXT_UNGLEICH);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Ausgabe Option -c = keine Textausgabe, nur Zählen und Anzahl Funde ausgeben
//Übergabe:	Namenliste
//		Anzahl Namen in Liste
//		Optionen
//		Pointer auf Textdaten
//Rückgabe:	TEXT_OK wenn mind. ein Fund, wenn gar kein Fund TEXT_UNGLEICH
int ausgabe_c (NAME_FORM namen_liste[], int anz_namen, OPT_FORM optionen, char *textdaten)
{
	char	wort[MAX_WORT_LAENGE];
	int	namecnt;
	int	verboten;
	int	rv;
	long	funde=0;

	while (1)
	{
		//Nächstes Wort abholen
		rv = hole_wort (textdaten, wort, NULL);
		//Wenn Textende erreichet Ende
		if (wort[0] == '\0') break;
		//Wenn Wort
		else if (rv == TEXT_OK)
		{
			//Wenn Wort eines der verbotenen nicht weiter prüfen
			verboten=0;
			for (namecnt=0; namecnt < anz_namen; namecnt++)
			{
				if (namen_liste[namecnt].Ist_Minusname == OP_EIN)
				{
					if (strcmp (wort, namen_liste[namecnt].Name) == 0)
					{
						verboten = 1;
						break;
					}
				}
			}
			if (verboten == 1) continue;
			//Kein verbotenes Wort
			for (namecnt=0; namecnt < anz_namen; namecnt++)
			{
				//Name = verbotener Name, nicht prüfen
				if (namen_liste[namecnt].Ist_Minusname == OP_EIN) continue;
				//Prüfen ob phongleich
				if (pruefe_phongleich (namen_liste[namecnt], wort, optionen) == TEXT_OK)
				{
					//nur zählen
					funde++;
					break;
				}
			}
		}
	}
	//nur Anzahl ausgeben
	printf ("%ld\n", funde);
	if (funde != 0) return (TEXT_OK);
	else return (TEXT_UNGLEICH);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Ausgabe Option -x = Ausgegeben wird für jeden Fund die Startposition im String und die Stringlänge
//		      (beides in char "gemessen") aus den von stdin gelieferten Daten
//Übergabe:	Namenliste
//		Anzahl Namen in Liste
//		Optionen
//		Pointer auf Textdaten
//Rückgabe:	TEXT_OK wenn mind. ein Fund, wenn gar kein Fund TEXT_UNGLEICH
int ausgabe_x (NAME_FORM namen_liste[], int anz_namen, OPT_FORM optionen, char *textdaten)
{
	char	wort[MAX_WORT_LAENGE];
	int	namecnt;
	int	rv;
	int	verboten;
	int	laenge;
	long	funde=0;
	long	txtpos;

	while (1)
	{
		//Nächstes Wort abholen
		rv = hole_wort (textdaten, wort, &txtpos);
		//Wenn Textende erreichet Ende
		if (wort[0] == '\0') break;
		//Wenn Wort
		else if (rv == TEXT_OK)
		{
			//Wenn Wort eines der verbotenen nicht weiter prüfen
			verboten=0;
			for (namecnt=0; namecnt < anz_namen; namecnt++)
			{
				if (namen_liste[namecnt].Ist_Minusname == OP_EIN)
				{
					if (strcmp (wort, namen_liste[namecnt].Name) == 0)
					{
						verboten = 1;
						break;
					}
				}
			}
			if (verboten == 1) continue;
			//Kein verbotenes Wort
			for (namecnt=0; namecnt < anz_namen; namecnt++)
			{
				//Name = verbotener Name, nicht prüfen
				if (namen_liste[namecnt].Ist_Minusname == OP_EIN) continue;
				//Prüfen ob phongleich
				if (pruefe_phongleich (namen_liste[namecnt], wort, optionen) == TEXT_OK)
				{
					//nur zählen
					funde++;
					laenge = strlen(wort);
					printf ("%ld %d\n", txtpos - laenge, laenge);
					break;
				}
			}
		}
	}

	if (funde != 0) return (TEXT_OK);
	else return (TEXT_UNGLEICH);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Zeigt die Legende zur Suche an: Namenliste & Parameter aus Programmaufruf zur Suche sowie Fundanzahl
//Wird von ausgabe_a, ausgabe_z, ausgabe_w aufgerufen
void zeige_legende (NAME_FORM namen_liste[], int anz_namen, OPT_FORM optionen, long funde)
{
	int	cnt;
	int	nolist=0;

	if (optionen.f == OP_EIN) printf("\033[7m<<<<<< LEGENDE >>>>>>\033[m\n");
	else printf("<<<<<< LEGENDE >>>>>>\n");

	//Suchnamen
	printf("Suche: ");
	for (cnt = 0; cnt < anz_namen; cnt++)
	{
		if (namen_liste[cnt].Ist_Minusname == OP_AUS)
		{
			if (optionen.f == OP_EIN) printf("%s%s\033[m ", namen_liste[cnt].Farbstring, namen_liste[cnt].Name);
			else printf("%s ", namen_liste[cnt].Name);
		}
		else nolist=1;
	}

	//Verbotene Namen
	if (nolist == 1)
	{
		printf("\nnicht: ");
		for (cnt = 0; cnt < anz_namen; cnt++)
		{
			if (namen_liste[cnt].Ist_Minusname == OP_EIN) printf("%s ", namen_liste[cnt].Name);
		}
	}

	//Optionen
	if (optionen.k == OP_AUS && optionen.p == OP_AUS && optionen.s == OP_AUS && optionen.e == OP_AUS && optionen.l == 0)
	{
		printf ("\nSuche: Einfache Textsuche");
	}
	else if (optionen.k == OP_AUS && optionen.p == OP_AUS && optionen.s == OP_AUS && optionen.e == OP_AUS && optionen.l != 0)
	{
		printf ("\nTyp:   Textsuche mit Levenshtein-Distanz von nicht mehr als %d", optionen.l);
	}
	else
	{
		printf("\nTyp:   Phonetische Suche ");
		if (optionen.k == OP_EIN) printf ("(Kölner Phonetik) ");
		if (optionen.p == OP_EIN) printf ("(Phonem) ");
		if (optionen.s == OP_EIN) printf ("(Soundex) ");
		if (optionen.e == OP_EIN) printf ("(Extended Soundex) ");
		if (optionen.l != 0) printf ("\n       mit Levenshtein-Distanz von nicht mehr als %d im phonetischen Code", optionen.l);
	}

	//Funde
	printf("\nFunde: %ld\n", funde);
}





