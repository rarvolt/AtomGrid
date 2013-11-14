#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>		// time()
#include <cstdlib>		// srand(); rand()
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>	// QueryPerformanceFrequency(); QueryPerformanceCounter()

typedef unsigned int uint;

long long milliseconds_now();
void clear_screen(void);
uint getElementOnCount(uint *grid, bool all = false);
std::string render_grid(uint *grid);

uint grid_width;
uint grid_height;
uint grid_size;
uint delay_ms;
uint draw_elements_count;

int main(int argc, char* argv[]) {
	using std::cout;
	using std::cin;
	using std::endl;
	using std::stringstream;
	using std::vector;

	std::srand(uint(time(NULL)));

	if (argc == 1) {
		cout << "Podaj szerokosc siatki: ";
		cin >> grid_width;

		cout << "Podaj wysokosc siatki: ";
		cin >> grid_height;
		cout << "Liczba elementow: " << grid_width * grid_height << endl << endl;

		cout << "Podaj w milisekundach odstep pomiedzy losowaniami: ";
		cin >> delay_ms;

		cout << "Podaj liczbe elementow modyfikowanych w kazdym losowaniu: ";
		cin >> draw_elements_count;
	}
	else if (argc == 5) {
		grid_width = atoi(argv[1]);
		grid_height = atoi(argv[2]);
		delay_ms = atoi(argv[3]);
		draw_elements_count = atoi(argv[4]);
	}
	else {
		cout << "Uzycie:" << endl;
		cout << argv[0] << " <wysokosc> <szerokosc> <czas> <ilosc_elementow_modyfikowanych>" << endl;
		return 0;
	}

	grid_size = grid_width * grid_height;

	// tworzenie siatki na podstawie podanych rozmiar�w
	uint *grid = new uint[grid_size];
	for (uint i = 0; i < grid_size; ++i)
		grid[i] = 0;

	// je�eli true program si� zako�czy
	bool stop = false;

	// true - w danym cyklu przeprowadzono losowanie
	bool drawn = false;

	// true - w danym cyklu zmodyfikowano wylosowane kom�rki
	bool changed = false;

	// licznik przeprowadzonych losowa�
	uint draws_counter = 0;

	// aktualnie wylosowany numer
	uint drawn_number;

	// tablica wylosowanych liczb
	vector<uint> numbers_drawn(draw_elements_count);

	while (!stop) {
		long long time_start = milliseconds_now();
		long long time_current = time_start;

		while ((time_current - time_start) < delay_ms) {
			if (!drawn) {
				drawn = true;
				++draws_counter;
				for (uint i = 0; i < draw_elements_count; ++i)
					numbers_drawn[i] = 0;
				uint last_valid_number_index = 0;
				for (uint i = 0; i < draw_elements_count; ++i) {
					drawn_number = std::rand() % (grid_size + 1);
					for (uint j = 0; j < draw_elements_count; ++j) {
						if (drawn_number == numbers_drawn[j] || drawn_number == 0) {
							i = last_valid_number_index + 1;
							break;
						}
					}
					numbers_drawn[i] = drawn_number;
					last_valid_number_index = i;
				}
			}

			if (!changed) {
				changed = true;
				for (uint i = 0; i < draw_elements_count; ++i) {
					if (!grid[numbers_drawn[i]])
						grid[numbers_drawn[i]] = 1;
				}
			}
			time_current = milliseconds_now();
		}
		if (getElementOnCount(grid) == grid_size)
			stop = true;
		else
			drawn = changed = false;

		clear_screen();
		cout << "Aby zatrzymac nacisnij Ctrl+C." << endl;

		cout << render_grid(grid);

		cout.precision(4);
		cout << "Wypelnienie: " << (float)getElementOnCount(grid, true) / (float)grid_size * 100.0
			<< '%' << endl;
		cout << "Liczba losowan: " << draws_counter << endl;
		cout << "Ostatnio wylosowane liczby:" << endl;
		for (uint i = 0; i < draw_elements_count; ++i)
			cout << numbers_drawn[i] << ", ";
		cout << endl;
		
		while (GetAsyncKeyState(VK_RETURN) & 0x8000);
	}

	cout << endl << endl;
	cout << "Symulacja zakonczona." << endl;
	system("pause");

	return 0;
}

long long milliseconds_now() {
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else {
		return GetTickCount();
	}
}

void clear_screen(void) {
	DWORD n;                         /* Number of characters written */
	DWORD size;                      /* number of visible characters */
	COORD coord = { 0 };               /* Top left screen position */
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	/* Get a handle to the console */
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(h, &csbi);

	/* Find the number of characters to overwrite */
	size = csbi.dwSize.X * csbi.dwSize.Y;

	/* Overwrite the screen buffer with whitespace */
	FillConsoleOutputCharacter(h, TEXT(' '), size, coord, &n);
	GetConsoleScreenBufferInfo(h, &csbi);
	FillConsoleOutputAttribute(h, csbi.wAttributes, size, coord, &n);

	/* Reset the cursor to the top left position */
	SetConsoleCursorPosition(h, coord);
}

uint getElementOnCount(uint *grid, bool all) {
	uint elementsOnCount = 0;
	if (all) {
		for (uint i = 0; i < grid_size; ++i)
			elementsOnCount += grid[i];
	}
	else {
		for (uint i = 0; i < grid_size; ++i)
			elementsOnCount += (grid[i] < 9) ? 0 : 1;
	}
	return elementsOnCount;
}

std::string render_grid(uint *grid) {

	std::stringstream buf;
	buf << " +-";
	for (uint i = 0; i < grid_width - 2; ++i)
		buf << "-";
	buf << "-+";
	buf << std::endl;

	for (uint i = 0; i < grid_height; ++i) {
		buf << " |";
		for (uint j = 0; j < grid_width; ++j)
			buf << (grid[i*grid_width + j] ? 'X' : ' ');
		buf << "|" << std::endl;
	}

	buf << " +-";
	for (uint i = 0; i < grid_width - 2; ++i)
		buf << "-";
	buf << "-+";
	buf << std::endl;

	return buf.str();
}
