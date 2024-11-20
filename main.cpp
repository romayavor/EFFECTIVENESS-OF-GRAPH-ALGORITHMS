#include <iostream>
#include <queue>      // Do używania kolejki priorytetowej
#include <fstream>    // Do operacji na plikach
#include <climits>    // Dla INT_MAX (największa wartość inta)
#include <chrono>     // biblioteka do mierzenia czasu
#include <ctime>      // biblioteka do obsługi czasu

using namespace std;

// Struktura reprezentująca krawędź
struct Krawedz {
    int do_wierzcholka, waga;
};

// Lista sąsiedztwa
class GrafLista {
public:
    int V; // Liczba wierzchołków
    Krawedz** sasiedzi; // Tablica wskaźników na listy krawędzi
    int* rozmiarSasiedzi; // Tablica rozmiarów list sąsiedztwa

    // Konstruktor
    GrafLista(int V) {
        this->V = V; // Ustawiamy liczbę wierzchołków
        sasiedzi = new Krawedz*[V]; // Tworzymy tablicę wskaźników na listy krawędzi
        rozmiarSasiedzi = new int[V]; // Tworzymy tablicę rozmiarów list
        for (int i = 0; i < V; ++i) {
            sasiedzi[i] = nullptr; // Inicjalizujemy wskaźniki jako puste
            rozmiarSasiedzi[i] = 0; // Inicjalizujemy rozmiary jako zero
        }
    }

    // Destruktor
    ~GrafLista() {
        for (int i = 0; i < V; ++i) {
            delete[] sasiedzi[i]; // Usuwamy każdą listę krawędzi
        }
        delete[] sasiedzi; // Usuwamy tablicę wskaźników
        delete[] rozmiarSasiedzi; // Usuwamy tablicę rozmiarów
    }

    // Funkcja dodająca krawędź
    void dodajKrawedz(int od, int do_wierzcholka, int waga) {
        Krawedz* noweKrawedzie = new Krawedz[rozmiarSasiedzi[od] + 1]; // Tworzymy nową listę z jednym dodatkowym elementem
        for (int i = 0; i < rozmiarSasiedzi[od]; ++i) {
            noweKrawedzie[i] = sasiedzi[od][i]; // Kopiujemy istniejące krawędzie
        }
        noweKrawedzie[rozmiarSasiedzi[od]] = {do_wierzcholka, waga}; // Dodajemy nową krawędź
        delete[] sasiedzi[od]; // Usuwamy starą listę krawędzi
        sasiedzi[od] = noweKrawedzie; // Ustawiamy nową listę jako aktualną
        rozmiarSasiedzi[od]++; // Zwiększamy rozmiar listy
    }

    // Funkcja realizująca algorytm Dijkstry
    void dijkstra(int zrodlo, int* dystans) const;
};

// Realizacja algorytmu Dijkstry dla listy sąsiedztwa
void GrafLista::dijkstra(int zrodlo, int* dystans) const {
    bool* odwiedzone = new bool[V]; // Tablica odwiedzonych wierzchołków
    for (int i = 0; i < V; ++i) {
        dystans[i] = INT_MAX; // Inicjalizujemy odległości jako nieskończoność
        odwiedzone[i] = false; // Inicjalizujemy odwiedziny jako fałsz
    }
    dystans[zrodlo] = 0; // Odległość od źródła do siebie równa się zero
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq; // Kolejka priorytetowa
    pq.push({0, zrodlo}); // Dodajemy źródło do kolejki

    while (!pq.empty()) {
        int u = pq.top().second; // Wierzchołek z najmniejszą odległością
        pq.pop(); // Usuwamy go z kolejki
        if (odwiedzone[u]) continue; // Jeśli odwiedzony, pomijamy
        odwiedzone[u] = true; // Oznaczamy jako odwiedzony

        for (int i = 0; i < rozmiarSasiedzi[u]; ++i) {
            Krawedz krawedz = sasiedzi[u][i]; // Bierzemy każdą krawędź
            int v = krawedz.do_wierzcholka;
            int waga = krawedz.waga;

            if (dystans[u] + waga < dystans[v]) {
                dystans[v] = dystans[u] + waga; // Aktualizujemy odległość
                pq.push({dystans[v], v}); // Dodajemy wierzchołek do kolejki
            }
        }
    }
    delete[] odwiedzone; // Usuwamy tablicę odwiedzonych wierzchołków
}

// Macierz sąsiedztwa
class GrafMacierz {
public:
    int V; // Liczba wierzchołków
    int** sasiedzi; // Macierz sąsiedztwa

    // Konstruktor
    GrafMacierz(int V) {
        this->V = V; // Ustawiamy liczbę wierzchołków
        sasiedzi = new int*[V]; // Tworzymy macierz
        for (int i = 0; i < V; ++i) {
            sasiedzi[i] = new int[V]; // Tworzymy wiersz w macierzy
            for (int j = 0; j < V; ++j) {
                sasiedzi[i][j] = (i == j) ? 0 : INT_MAX; // Inicjalizujemy macierz
            }
        }
    }

    // Destruktor
    ~GrafMacierz() {
        for (int i = 0; i < V; ++i) {
            delete[] sasiedzi[i]; // Usuwamy każdy wiersz
        }
        delete[] sasiedzi; // Usuwamy macierz
    }

    // Funkcja dodająca krawędź
    void dodajKrawedz(int od, int do_wierzcholka, int waga) {
        sasiedzi[od][do_wierzcholka] = waga; // Ustawiamy wagę krawędzi
    }

    // Funkcja realizująca algorytm Dijkstry
    void dijkstra(int zrodlo, int* dystans) const;
};

// Realizacja algorytmu Dijkstry dla macierzy sąsiedztwa
void GrafMacierz::dijkstra(int zrodlo, int* dystans) const {
    bool* odwiedzone = new bool[V]; // Tablica odwiedzonych wierzchołków
    for (int i = 0; i < V; ++i) {
        dystans[i] = INT_MAX; // Inicjalizujemy odległości jako nieskończoność
        odwiedzone[i] = false; // Inicjalizujemy odwiedziny jako fałsz
    }
    dystans[zrodlo] = 0; // Odległość od źródła do siebie równa się zero

    for (int i = 0; i < V; ++i) {
        int u = -1;

        for (int j = 0; j < V; ++j) {
            if (!odwiedzone[j] && (u == -1 || dystans[j] < dystans[u])) {
                u = j; // Wybieramy wierzchołek z najmniejszą odległością
            }
        }

        if (dystans[u] == INT_MAX) {
            break; // Jeśli najmniejsza odległość równa się nieskończoności, kończymy
        }

        odwiedzone[u] = true; // Oznaczamy jako odwiedzony

        for (int v = 0; v < V; ++v) {
            if (!odwiedzone[v] && sasiedzi[u][v] != INT_MAX && dystans[u] + sasiedzi[u][v] < dystans[v]) {
                dystans[v] = dystans[u] + sasiedzi[u][v]; // Aktualizujemy odległość
            }
        }
    }
    delete[] odwiedzone; // Usuwamy tablicę odwiedzonych wierzchołków
}

// Funkcja do odczytu grafu z pliku
void wczytajGrafZPliku(const char* nazwaPliku, GrafLista*& grafLista, GrafMacierz*& grafMacierz) {
    ifstream plik(nazwaPliku);
    if (plik.is_open()) {
        int E, V;
        plik >> V >> E; // Odczytujemy liczbę wierzchołków i krawędzi

        // Usuwamy stare grafy, jeśli istnieją
        if (grafLista != nullptr) {
            delete grafLista;
        }
        if (grafMacierz != nullptr) {
            delete grafMacierz;
        }

        grafLista = new GrafLista(V); // Tworzymy nowe grafy
        grafMacierz = new GrafMacierz(V);

        for (int i = 0; i < E; ++i) {
            int od, do_wierzcholka, waga;
            plik >> od >> do_wierzcholka >> waga; // Odczytujemy krawędź
            grafLista->dodajKrawedz(od, do_wierzcholka, waga); // Dodajemy krawędź do grafów
            grafMacierz->dodajKrawedz(od, do_wierzcholka, waga);
        }
        plik.close();
    } else {
        cout << "Nie mozna otworzyc pliku." << endl;
    }
}

// Funkcja do generowania losowego grafu
void generujLosowyGraf(int V, int gestosc, GrafLista*& grafLista, GrafMacierz*& grafMacierz) {
    // Usuwamy stare grafy, jeśli istnieją
    if (grafLista != nullptr) {
        delete grafLista;
    }
    if (grafMacierz != nullptr) {
        delete grafMacierz;
    }

    grafLista = new GrafLista(V); // Tworzymy nowe grafy
    grafMacierz = new GrafMacierz(V);

    int E = gestosc * V * (V - 1) / 200; // Liczymy liczbę krawędzi
    srand(time(nullptr)); // Inicjalizujemy generator liczb losowych

    for (int i = 0; i < E; ++i) {
        int od = rand() % V; // Losowy wierzchołek
        int do_wierzcholka = rand() % V; // Losowy wierzchołek
        int waga = rand() % 100 + 1; // Losowa waga krawędzi
        grafLista->dodajKrawedz(od, do_wierzcholka, waga); // Dodajemy krawędź do grafów
        grafMacierz->dodajKrawedz(od, do_wierzcholka, waga);
    }
}

// Funkcja do wyświetlania grafu
void wyswietlGraf(const GrafLista& grafLista, const GrafMacierz& grafMacierz) {
    cout << "Graf w formie listy sasiedztwa:" << endl;
    for (int i = 0; i < grafLista.V; ++i) {
        cout << i << ":";
        for (int j = 0; j < grafLista.rozmiarSasiedzi[i]; ++j) {
            cout << " -> (" << grafLista.sasiedzi[i][j].do_wierzcholka << ", " << grafLista.sasiedzi[i][j].waga << ")";
        }
        cout << endl;
    }

    cout << "Graf w formie macierzy sasiedztwa:" << endl;
    for (int i = 0; i < grafMacierz.V; ++i) {
        for (int j = 0; j < grafMacierz.V; ++j) {
            if (grafMacierz.sasiedzi[i][j] == INT_MAX) {
                cout << "INF ";
            } else {
                cout << grafMacierz.sasiedzi[i][j] << " ";
            }
        }
        cout << endl;
    }
}

// Funkcja do uruchamiania algorytmu Dijkstry i mierzenia czasu
void uruchomDijkstraIMierzCzas(const GrafLista& grafLista, const GrafMacierz& grafMacierz, int zrodlo, int cel) {
    int* dystansLista = new int[grafLista.V];
    int* dystansMacierz = new int[grafMacierz.V];

    auto start_lista = chrono::high_resolution_clock::now();
    grafLista.dijkstra(zrodlo, dystansLista);
    auto end_lista = chrono::high_resolution_clock::now();

    auto start_macierz = chrono::high_resolution_clock::now();
    grafMacierz.dijkstra(zrodlo, dystansMacierz);
    auto end_macierz = chrono::high_resolution_clock::now();

    cout << "Czas wykonania (lista sasiedztwa): " << chrono::duration_cast<chrono::milliseconds>(end_lista - start_lista).count() << " milisekund" << endl;
    cout << "Czas wykonania (macierz sasiedztwa): " << chrono::duration_cast<chrono::milliseconds>(end_macierz - start_macierz).count() << " milisekund" << endl;

    cout << "Najkrotsza droga od " << zrodlo << " do " << cel << " (lista sasiedztwa): " << dystansLista[cel] << endl;
    cout << "Najkrotsza droga od " << zrodlo << " do " << cel << " (macierz sasiedztwa): " << dystansMacierz[cel] << endl;

    delete[] dystansLista;
    delete[] dystansMacierz;
}

// Funkcja główna
int main() {
    GrafLista* grafLista = nullptr; // Wskaźniki na grafy
    GrafMacierz* grafMacierz = nullptr;
    int opcja, V, gestosc, zrodlo, cel;
    char nazwaPliku[100];

    while (true) {
        cout << "Menu:\n";
        cout << "1. Wczytaj z pliku\n";
        cout << "2. Wygeneruj losowo\n";
        cout << "3. Wyswietl graf\n";
        cout << "4. Uruchom Dijkstre\n";
        cout << "5. Wyjscie\n";
        cout << "Wybierz opcje: ";
        cin >> opcja;

        switch (opcja) {
        case 1:
            cout << "Podaj nazwe pliku: ";
            cin >> nazwaPliku;
            wczytajGrafZPliku(nazwaPliku, grafLista, grafMacierz);
            break;
        case 2:
            cout << "Podaj liczbe wierzcholkow: ";
            cin >> V;
            cout << "Podaj gestosc grafu (w %): ";
            cin >> gestosc;
            generujLosowyGraf(V, gestosc, grafLista, grafMacierz);
            break;
        case 3:
            if (grafLista && grafMacierz) {
                wyswietlGraf(*grafLista, *grafMacierz);
            } else {
                cout << "Graf nie zostal jeszcze zainicjalizowany." << endl;
            }
            break;
        case 4:
            if (grafLista && grafMacierz) {
                cout << "Podaj wierzcholek poczatkowy: ";
                cin >> zrodlo;
                cout << "Podaj wierzcholek koncowy: ";
                cin >> cel;
                uruchomDijkstraIMierzCzas(*grafLista, *grafMacierz, zrodlo, cel);
            } else {
                cout << "Graf nie zostal jeszcze zainicjalizowany." << endl;
            }
            break;
        case 5:
            delete grafLista; // Oczyszczamy graf przed wyjściem
            delete grafMacierz; // Oczyszczamy graf przed wyjściem
            return 0;
        default:
            cout << "Nieprawidlowa opcja. Sprobuj ponownie.\n";
        }
    }
}
