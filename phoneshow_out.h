#ifndef PHONESHOW_OUT
#define PHONESHOW_OUT

// Bolean
typedef enum {true, false} bool;



// Farbstrings
#define COLORSTRING_SIZE 		16
#define NUMBER_OF_COLORS		28
static const char colors[NUMBER_OF_COLORS][COLORSTRING_SIZE] = {
	{"\033[1;37m\033[41m"}, {"\033[1;37m\033[42m"},	{"\033[1;37m\033[43m"}, {"\033[1;37m\033[44m"},
	{"\033[1;37m\033[45m"},	{"\033[1;37m\033[46m"}, {"\033[1;37m\033[47m"}, {"\033[1;33m\033[41m"},
	{"\033[1;33m\033[42m"}, {"\033[1;33m\033[43m"}, {"\033[1;33m\033[44m"}, {"\033[1;33m\033[45m"},
	{"\033[1;33m\033[46m"}, {"\033[1;33m\033[47m"},	{"\033[1;36m\033[41m"}, {"\033[1;36m\033[42m"},
	{"\033[1;36m\033[43m"}, {"\033[1;36m\033[44m"}, {"\033[1;36m\033[45m"}, {"\033[1;36m\033[46m"},
	{"\033[1;36m\033[47m"}, {"\033[1;32m\033[41m"}, {"\033[1;32m\033[42m"}, {"\033[1;32m\033[43m"},
	{"\033[1;32m\033[44m"}, {"\033[1;32m\033[45m"},	{"\033[1;32m\033[46m"}, {"\033[1;32m\033[47m"}
};

// Für je einen übergebenen Namen
typedef struct {
	char	Name[BUFFER_NAME_SIZE];
	bool	Is_Minusname;
	char	Code_k[BUFFER_CODE_SIZE];
	char	Code_p[BUFFER_CODE_SIZE];
	char	Code_s[BUFFER_CODE_SIZE];
	char	Code_e[BUFFER_CODE_SIZE];
	char	Colorstring[COLORSTRING_SIZE];
} nameinfo_t;

// Für Optionen
typedef struct {
	bool	k;
	bool	p;
	bool	s;
	bool	e;
	int		l;
	bool	n;
	bool	f;
} options_t;



// Ausgabefunktionen
// Rückgabe true wenn Fund, false wenn keiner
bool outmode_a (const nameinfo_t names_list[], const int number_of_names, const options_t options, const char *text_data);
bool outmode_z (const nameinfo_t names_list[], const int number_of_names, const options_t options, const char *text_data);
bool outmode_w (const nameinfo_t names_list[], const int number_of_names, const options_t options, const char *text_data);
bool outmode_c (const nameinfo_t names_list[], const int number_of_names, const options_t options, const char *text_data);
bool outmode_x (const nameinfo_t names_list[], const int number_of_names, const options_t options, const char *text_data);


#endif //PHONESHOW_OUT



