#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <random>
#include <limits.h>

using namespace std;
using namespace std::chrono;

class GrafMacierz
{
    int V;
    vector<vector<int>> macierz;

public:
    GrafMacierz(int V) : V(V), macierz(V, vector<int>(V, 0)) {}

    void dodajKrawedz(int u, int v, int w)
    {
        macierz[u][v] = w;
        macierz[v][u] = w;
    }

    vector<vector<int>> pobierzMacierz() const
    {
        return macierz;
    }

    int pobierzV() const
    {
        return V;
    }

    void wczytajZPliku(const string &nazwaPliku)
    {
        ifstream plik(nazwaPliku);
        if (!plik)
        {
            cerr << "Nie można otworzyć pliku " << nazwaPliku << endl;
            exit(1);
        }
        int E;
        plik >> V >> E;
        macierz = vector<vector<int>>(V, vector<int>(V, 0));
        for (int i = 0; i < E; i++)
        {
            int u, v, w;
            plik >> u >> v >> w;
            dodajKrawedz(u, v, w);
        }
        plik.close();
    }

    void wyswietl() const
    {
        for (const auto &wiersz : macierz)
        {
            for (const auto &wartosc : wiersz)
            {
                cout << wartosc << " ";
            }
            cout << endl;
        }
    }
};

class GrafLista
{
    int V;
    vector<vector<pair<int, int>>> listaSasiedztwa;

public:
    GrafLista(int V) : V(V), listaSasiedztwa(V) {}

    void dodajKrawedz(int u, int v, int w)
    {
        listaSasiedztwa[u].push_back({v, w});
        listaSasiedztwa[v].push_back({u, w});
    }

    vector<vector<pair<int, int>>> pobierzListeSasiedztwa() const
    {
        return listaSasiedztwa;
    }

    int pobierzV() const
    {
        return V;
    }

    void wczytajZPliku(const string &nazwaPliku)
    {
        ifstream plik(nazwaPliku);
        if (!plik)
        {
            cerr << "Nie można otworzyć pliku " << nazwaPliku << endl;
            exit(1);
        }
        int E;
        plik >> V >> E;
        listaSasiedztwa = vector<vector<pair<int, int>>>(V);
        for (int i = 0; i < E; i++)
        {
            int u, v, w;
            plik >> u >> v >> w;
            dodajKrawedz(u, v, w);
        }
        plik.close();
    }

    void wyswietl() const
    {
        for (int i = 0; i < V; i++)
        {
            cout << i << ": ";
            for (const auto &p : listaSasiedztwa[i])
            {
                cout << "(" << p.first << ", " << p.second << ") ";
            }
            cout << endl;
        }
    }
};

void generujLosowyGraf(int V, double gestosc, GrafMacierz &gm, GrafLista &gl)
{
    gm = GrafMacierz(V);
    gl = GrafLista(V);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 1000);
    int krawedzie = gestosc / 100.0 * V * (V - 1) / 2;
    while (krawedzie > 0)
    {
        int u = rand() % V;
        int v = rand() % V;
        if (u != v && gm.pobierzMacierz()[u][v] == 0)
        {
            int w = dist(gen);
            gm.dodajKrawedz(u, v, w);
            gl.dodajKrawedz(u, v, w);
            krawedzie--;
        }
    }
}

vector<vector<int>> krawedzieDoMacierzy(int V, const vector<pair<int, int>> &krawedzie, const vector<vector<int>> &macierz)
{
    vector<vector<int>> macierzMST(V, vector<int>(V, 0));
    for (const auto &krawedz : krawedzie)
    {
        int u = krawedz.first;
        int v = krawedz.second;
        macierzMST[u][v] = macierz[u][v];
        macierzMST[v][u] = macierz[u][v];
    }
    return macierzMST;
}

void wyswietlMacierz(const vector<vector<int>> &macierz)
{
    for (const auto &wiersz : macierz)
    {
        for (const auto &wartosc : wiersz)
        {
            cout << wartosc << " ";
        }
        cout << endl;
    }
}

void primMSTMacierz(const GrafMacierz &graf, bool symulacja = false)
{
    auto start = high_resolution_clock::now();

    int V = graf.pobierzV();
    vector<vector<int>> macierz = graf.pobierzMacierz();
    vector<int> klucz(V, INT_MAX);
    vector<int> rodzic(V, -1);
    vector<bool> wMST(V, false);
    vector<pair<int, int>> krawedzieMST;
    int wagaMST = 0;

    klucz[0] = 0;

    for (int licznik = 0; licznik < V - 1; licznik++)
    {
        int min = INT_MAX, u;
        for (int v = 0; v < V; v++)
            if (!wMST[v] && klucz[v] < min)
                min = klucz[v], u = v;

        wMST[u] = true;
        for (int v = 0; v < V; v++)
            if (macierz[u][v] && !wMST[v] && macierz[u][v] < klucz[v])
                rodzic[v] = u, klucz[v] = macierz[u][v];
    }

    for (int i = 1; i < V; i++)
    {
        krawedzieMST.push_back({rodzic[i], i});
        wagaMST += macierz[rodzic[i]][i];
    }
    cout << "Całkowita waga MST: " << wagaMST << "\n";

    auto stop = high_resolution_clock::now();
    auto czas = duration_cast<milliseconds>(stop - start);
    cout << "Czas wykonania MST Prima (Macierz): " << czas.count() << " milisekund\n";

    if (!symulacja)
    {
        vector<vector<int>> macierzMST = krawedzieDoMacierzy(V, krawedzieMST, macierz);
        cout << "Macierz MST:\n";
        wyswietlMacierz(macierzMST);
    }
}

void primMSTLista(const GrafLista &graf, bool symulacja = false)
{
    auto start = high_resolution_clock::now();

    int V = graf.pobierzV();
    vector<vector<pair<int, int>>> listaSasiedztwa = graf.pobierzListeSasiedztwa();
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    vector<int> klucz(V, INT_MAX);
    vector<int> rodzic(V, -1);
    vector<bool> wMST(V, false);
    vector<pair<int, int>> krawedzieMST;
    int wagaMST = 0;

    int src = 0;
    pq.push({0, src});
    klucz[src] = 0;

    while (!pq.empty())
    {
        int u = pq.top().second;
        pq.pop();
        wMST[u] = true;

        for (auto x : listaSasiedztwa[u])
        {
            int v = x.first;
            int waga = x.second;
            if (!wMST[v] && klucz[v] > waga)
            {
                klucz[v] = waga;
                pq.push({klucz[v], v});
                rodzic[v] = u;
            }
        }
    }

    for (int i = 1; i < V; ++i)
    {
        krawedzieMST.push_back({rodzic[i], i});
        for (auto x : listaSasiedztwa[rodzic[i]])
        {
            if (x.first == i)
            {
                wagaMST += x.second;
                break;
            }
        }
    }
    cout << "Całkowita waga MST: " << wagaMST << "\n";

    auto stop = high_resolution_clock::now();
    auto czas = duration_cast<milliseconds>(stop - start);
    cout << "Czas wykonania MST Prima (Lista): " << czas.count() << " milisekund\n\n";

    if (!symulacja)
    {
        cout << "Lista sąsiedztwa MST:\n";
        for (int i = 1; i < V; ++i)
        {
            cout << rodzic[i] << " - " << i << " \n";
        }
    }
}

struct Krawedz
{
    int u, v, waga;
    bool operator<(const Krawedz &k) const
    {
        return waga < k.waga;
    }
};

class DSU
{
    vector<int> rodzic, ranga;

public:
    DSU(int n)
    {
        rodzic.resize(n);
        ranga.resize(n);
        for (int i = 0; i < n; i++)
        {
            rodzic[i] = i;
            ranga[i] = 0;
        }
    }

    int znajdz(int u)
    {
        if (u != rodzic[u])
            rodzic[u] = znajdz(rodzic[u]);
        return rodzic[u];
    }

    void polacz(int u, int v)
    {
        u = znajdz(u);
        v = znajdz(v);
        if (u != v)
        {
            if (ranga[u] < ranga[v])
            {
                rodzic[u] = v;
            }
            else if (ranga[u] > ranga[v])
            {
                rodzic[v] = u;
            }
            else
            {
                rodzic[v] = u;
                ranga[u]++;
            }
        }
    }
};

void kruskalMSTMacierz(const GrafMacierz &graf, bool symulacja = false)
{
    auto start = high_resolution_clock::now();

    int V = graf.pobierzV();
    vector<vector<int>> macierz = graf.pobierzMacierz();
    vector<Krawedz> krawedzie;
    vector<pair<int, int>> krawedzieMST;
    int wagaMST = 0;

    for (int u = 0; u < V; u++)
    {
        for (int v = u + 1; v < V; v++)
        {
            if (macierz[u][v])
            {
                krawedzie.push_back({u, v, macierz[u][v]});
            }
        }
    }

    sort(krawedzie.begin(), krawedzie.end());

    DSU dsu(V);
    for (const auto &krawedz : krawedzie)
    {
        int u = krawedz.u;
        int v = krawedz.v;
        int waga = krawedz.waga;

        if (dsu.znajdz(u) != dsu.znajdz(v))
        {
            krawedzieMST.push_back({u, v});
            wagaMST += waga;
            dsu.polacz(u, v);
        }
    }
    cout << "Całkowita waga MST: " << wagaMST << "\n";

    auto stop = high_resolution_clock::now();
    auto czas = duration_cast<milliseconds>(stop - start);
    cout << "Czas wykonania MST Kruskala (Macierz): " << czas.count() << " milisekund\n";

    if (!symulacja)
    {
        vector<vector<int>> macierzMST = krawedzieDoMacierzy(V, krawedzieMST, macierz);
        cout << "Macierz MST:\n";
        wyswietlMacierz(macierzMST);
    }
}

void kruskalMSTLista(const GrafLista &graf, bool symulacja = false)
{
    auto start = high_resolution_clock::now();

    int V = graf.pobierzV();
    vector<vector<pair<int, int>>> listaSasiedztwa = graf.pobierzListeSasiedztwa();
    vector<Krawedz> krawedzie;
    vector<pair<int, int>> krawedzieMST;
    int wagaMST = 0;

    for (int u = 0; u < V; u++)
    {
        for (auto &x : listaSasiedztwa[u])
        {
            int v = x.first;
            int waga = x.second;
            if (u < v)
            {
                krawedzie.push_back({u, v, waga});
            }
        }
    }

    sort(krawedzie.begin(), krawedzie.end());

    DSU dsu(V);
    for (const auto &krawedz : krawedzie)
    {
        int u = krawedz.u;
        int v = krawedz.v;
        int waga = krawedz.waga;

        if (dsu.znajdz(u) != dsu.znajdz(v))
        {
            krawedzieMST.push_back({u, v});
            wagaMST += waga;
            dsu.polacz(u, v);
        }
    }
    cout << "Całkowita waga MST: " << wagaMST << "\n";

    auto stop = high_resolution_clock::now();
    auto czas = duration_cast<milliseconds>(stop - start);
    cout << "Czas wykonania MST Kruskala (Lista): " << czas.count() << " milisekund\n\n";

    if (!symulacja)
    {
        cout << "Lista sąsiedztwa MST:\n";
        for (const auto &k : krawedzieMST)
        {
            cout << k.first << " - " << k.second << "\n";
        }
    }
}

void dijkstraMacierz(const GrafMacierz &graf, int zrodlo, bool symulacja = false)
{
    auto start = high_resolution_clock::now();

    int V = graf.pobierzV();
    vector<vector<int>> macierz = graf.pobierzMacierz();
    vector<int> odleglosci(V, INT_MAX);
    vector<bool> odwiedzone(V, false);
    odleglosci[zrodlo] = 0;

    for (int i = 0; i < V - 1; i++)
    {
        int min = INT_MAX, u;
        for (int v = 0; v < V; v++)
        {
            if (!odwiedzone[v] && odleglosci[v] <= min)
            {
                min = odleglosci[v];
                u = v;
            }
        }

        odwiedzone[u] = true;

        for (int v = 0; v < V; v++)
        {
            if (!odwiedzone[v] && macierz[u][v] && odleglosci[u] != INT_MAX && odleglosci[u] + macierz[u][v] < odleglosci[v])
            {
                odleglosci[v] = odleglosci[u] + macierz[u][v];
            }
        }
    }

    if (!symulacja)
    {
        cout << "Odległości od źródła " << zrodlo << ":\n";
    }
    for (int i = 0; i < V; i++)
    {
        if (!symulacja)
        {
            cout << "Do wierzchołka " << i << ": " << odleglosci[i] << "\n";
        }
    }

    auto stop = high_resolution_clock::now();
    auto czas = duration_cast<milliseconds>(stop - start);
    cout << "Czas wykonania algorytmu Dijkstry (Macierz): " << czas.count() << " milisekund\n";
}

void dijkstraLista(const GrafLista &graf, int zrodlo, bool symulacja = false)
{
    auto start = high_resolution_clock::now();

    int V = graf.pobierzV();
    vector<vector<pair<int, int>>> listaSasiedztwa = graf.pobierzListeSasiedztwa();
    vector<int> odleglosci(V, INT_MAX);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    odleglosci[zrodlo] = 0;
    pq.push({0, zrodlo});

    while (!pq.empty())
    {
        int u = pq.top().second;
        pq.pop();

        for (auto x : listaSasiedztwa[u])
        {
            int v = x.first;
            int waga = x.second;
            if (odleglosci[u] + waga < odleglosci[v])
            {
                odleglosci[v] = odleglosci[u] + waga;
                pq.push({odleglosci[v], v});
            }
        }
    }

    if (!symulacja)
    {
        cout << "Odległości od źródła " << zrodlo << ":\n";
    }
    for (int i = 0; i < V; i++)
    {
        if (!symulacja)
        {
            cout << "Do wierzchołka " << i << ": " << odleglosci[i] << "\n";
        }
    }

    auto stop = high_resolution_clock::now();
    auto czas = duration_cast<milliseconds>(stop - start);
    cout << "Czas wykonania algorytmu Dijkstry (Lista): " << czas.count() << " milisekund\n\n";
}

void BellmanFordMacierz(const GrafMacierz &graf, int zrodlo, bool symulacja = false)
{
    auto start = high_resolution_clock::now();

    int V = graf.pobierzV();
    vector<vector<int>> macierz = graf.pobierzMacierz();
    vector<int> odleglosci(V, INT_MAX);
    odleglosci[zrodlo] = 0;

    for (int i = 1; i <= V - 1; i++)
    {
        for (int u = 0; u < V; u++)
        {
            for (int v = 0; v < V; v++)
            {
                if (macierz[u][v] && odleglosci[u] != INT_MAX && odleglosci[u] + macierz[u][v] < odleglosci[v])
                {
                    odleglosci[v] = odleglosci[u] + macierz[u][v];
                }
            }
        }
    }

    for (int u = 0; u < V; u++)
    {
        for (int v = 0; v < V; v++)
        {
            if (macierz[u][v] && odleglosci[u] != INT_MAX && odleglosci[u] + macierz[u][v] < odleglosci[v])
            {
                cout << "Graf zawiera cykl o ujemnej wadze!\n";
                return;
            }
        }
    }

    if (!symulacja)
    {
        cout << "Odległości od źródła " << zrodlo << ":\n";
    }
    for (int i = 0; i < V; i++)
    {
        if (!symulacja)
        {
            cout << "Do wierzchołka " << i << ": " << odleglosci[i] << "\n";
        }
    }

    auto stop = high_resolution_clock::now();
    auto czas = duration_cast<milliseconds>(stop - start);
    cout << "Czas wykonania algorytmu Bellmana-Forda (Macierz): " << czas.count() << " milisekund\n";
}

void BellmanFordLista(const GrafLista &graf, int zrodlo, bool symulacja = false)
{
    auto start = high_resolution_clock::now();

    int V = graf.pobierzV();
    vector<vector<pair<int, int>>> listaSasiedztwa = graf.pobierzListeSasiedztwa();
    vector<int> odleglosci(V, INT_MAX);
    odleglosci[zrodlo] = 0;

    for (int i = 1; i <= V - 1; i++)
    {
        for (int u = 0; u < V; u++)
        {
            for (auto x : listaSasiedztwa[u])
            {
                int v = x.first;
                int waga = x.second;
                if (odleglosci[u] != INT_MAX && odleglosci[u] + waga < odleglosci[v])
                {
                    odleglosci[v] = odleglosci[u] + waga;
                }
            }
        }
    }

    for (int u = 0; u < V; u++)
    {
        for (auto x : listaSasiedztwa[u])
        {
            int v = x.first;
            int waga = x.second;
            if (odleglosci[u] != INT_MAX && odleglosci[u] + waga < odleglosci[v])
            {
                cout << "Graf zawiera cykl o ujemnej wadze!\n";
                return;
            }
        }
    }

    if (!symulacja)
    {
        cout << "Odległości od źródła " << zrodlo << ":\n";
    }
    for (int i = 0; i < V; i++)
    {
        if (!symulacja)
        {
            cout << "Do wierzchołka " << i << ": " << odleglosci[i] << "\n";
        }
    }

    auto stop = high_resolution_clock::now();
    auto czas = duration_cast<milliseconds>(stop - start);
    cout << "Czas wykonania algorytmu Bellmana-Forda (Lista): " << czas.count() << " milisekund\n\n";
}

void wyswietlMenu()
{
    cout << "Menu:\n";
    cout << "1. Wczytaj dane z pliku\n";
    cout << "2. Wygeneruj graf losowo\n";
    cout << "3. Wyświetl graf listowo i macierzowo\n";
    cout << "4. Algorytm Prima (macierzowo i listowo) z wyświetleniem wyników\n";
    cout << "5. Algorytm Kruskala (macierzowo i listowo) z wyświetleniem wyników\n";
    cout << "6. Algorytm Dijkstry (macierzowo i listowo) z wyświetleniem wyników\n";
    cout << "7. Algorytm Bellmana-Forda (macierzowo i listowo) z wyświetleniem wyników\n";
    cout << "8. Tryb symulacji\n";
    cout << "9. Wyjście\n";
}

int main()
{
    GrafMacierz gm(0);
    GrafLista gl(0);
    int wybor;
    string nazwaPliku;
    int V = 0, zrodlo = 0, liczbaSymulacji = 1;
    double gestosc = 0.0;
    bool symulacja = false;
    while (true)
    {
        wyswietlMenu();
        cin >> wybor;
        switch (wybor)
        {
        case 1:
            cout << "Podaj nazwę pliku: ";
            cin >> nazwaPliku;
            gm.wczytajZPliku(nazwaPliku);
            gl.wczytajZPliku(nazwaPliku);
            break;
        case 2:
            cout << "Podaj liczbę wierzchołków: ";
            cin >> V;
            cout << "Podaj gęstość grafu (w procentach): ";
            cin >> gestosc;
            generujLosowyGraf(V, gestosc, gm, gl);
            break;
        case 3:
            cout << "\nGraf w formie macierzowej:\n";
            gm.wyswietl();
            cout << "\nGraf w formie listowej:\n";
            gl.wyswietl();
            break;
        case 4:
            if (!symulacja)
            {
                primMSTMacierz(gm);
                cout << "\n";
                primMSTLista(gl);
            }
            else
            {
                for (int i = 0; i < liczbaSymulacji; i++)
                {
                    generujLosowyGraf(V, gestosc, gm, gl);
                    primMSTMacierz(gm, symulacja);
                    cout << "\n";
                    primMSTLista(gl, symulacja);
                }
            }
            break;
        case 5:
            if (!symulacja)
            {
                kruskalMSTMacierz(gm);
                cout << "\n";
                kruskalMSTLista(gl);
            }
            else
            {
                for (int i = 0; i < liczbaSymulacji; i++)
                {
                    generujLosowyGraf(V, gestosc, gm, gl);
                    kruskalMSTMacierz(gm, symulacja);
                    cout << "\n";
                    kruskalMSTLista(gl, symulacja);
                }
            }
            break;
        case 6:
            cout << "Podaj wierzchołek źródłowy: ";
            cin >> zrodlo;
            if (!symulacja)
            {
                dijkstraMacierz(gm, zrodlo);
                cout << "\n";
                dijkstraLista(gl, zrodlo);
            }
            else
            {
                for (int i = 0; i < liczbaSymulacji; i++)
                {
                    generujLosowyGraf(V, gestosc, gm, gl);
                    dijkstraMacierz(gm, zrodlo, symulacja);
                    cout << "\n";
                    dijkstraLista(gl, zrodlo, symulacja);
                }
            }
            break;
        case 7:
            cout << "Podaj wierzchołek źródłowy: ";
            cin >> zrodlo;
            if (!symulacja)
            {
                BellmanFordMacierz(gm, zrodlo);
                cout << "\n";
                BellmanFordLista(gl, zrodlo);
            }
            else
            {
                for (int i = 0; i < liczbaSymulacji; i++)
                {
                    generujLosowyGraf(V, gestosc, gm, gl);
                    BellmanFordMacierz(gm, zrodlo, symulacja);
                    cout << "\n";
                    BellmanFordLista(gl, zrodlo, symulacja);
                }
            }
            break;
        case 8:
            cout << "Podaj liczbę symulacji: ";
            cin >> liczbaSymulacji;
            symulacja = true;
            break;
        case 9:
            return 0;
        default:
            cout << "Nieprawidłowa opcja! Spróbuj ponownie.\n";
        }
    }
}