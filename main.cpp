#include <iostream>
#include <fstream>
#include <cmath>
#include <stdlib.h>
using namespace std;
struct fileheader {
    unsigned short type;
    unsigned int size;
    unsigned short reserved1, reserved2;
    unsigned int offbits;
};
struct infoheader {
    unsigned int size;
    int width, height;
    unsigned short planes;
    unsigned short bitCount;
    unsigned int compression;
    unsigned int imageSize;
    int xresolution, yresolution;
    unsigned int uColours;
    unsigned int importantColours;
};
int odczytywaniefileheader(ifstream& ifs, fileheader& fileheader, int& size) {
    ifs.read(reinterpret_cast<char*>(&fileheader.type), 2);
    ifs.read(reinterpret_cast<char*>(&fileheader.size), 4);
    ifs.read(reinterpret_cast<char*>(&fileheader.reserved1), 2);
    ifs.read(reinterpret_cast<char*>(&fileheader.reserved2), 2);
    ifs.read(reinterpret_cast<char*>(&fileheader.offbits), 4);
    size = fileheader.size - fileheader.offbits;
    return ifs.tellg();
}
int odczytywanieinfoheader(ifstream& ifs, infoheader& infoheader, int& width, int& height, int& bitCount, int& padding) {
    ifs.read(reinterpret_cast<char*>(&infoheader.size), 4);
    ifs.read(reinterpret_cast<char*>(&infoheader.width), 4);
    ifs.read(reinterpret_cast<char*>(&infoheader.height), 4);
    ifs.read(reinterpret_cast<char*>(&infoheader.planes), 2);
    ifs.read(reinterpret_cast<char*>(&infoheader.bitCount), 2);
    ifs.read(reinterpret_cast<char*>(&infoheader.compression), 4);
    ifs.read(reinterpret_cast<char*>(&infoheader.imageSize), 4);
    ifs.read(reinterpret_cast<char*>(&infoheader.xresolution), 4);
    ifs.read(reinterpret_cast<char*>(&infoheader.yresolution), 4);
    ifs.read(reinterpret_cast<char*>(&infoheader.uColours), 4);
    ifs.read(reinterpret_cast<char*>(&infoheader.importantColours), 4);
    width = infoheader.width;
    height = infoheader.height;
    bitCount = infoheader.bitCount;
    padding = width % 4;
    return ifs.tellg();
}
void wyswietlanie(fileheader fileheader, infoheader infoheader) {
    if (fileheader.type != 0x4d42)
        cout << "\nZly format pliku.";   //sprawdzanie czy plik wejsciowy to bmp
    else {
        cout << "Fileheader:" << endl;
        cout << "Sygnatura pliku (typ): " << fileheader.type << endl;
        cout << "Rozmiar calego pliku (dlugosc w bajtach): " << fileheader.size << endl;
        cout << "Pole zarezerwowane (zwykle zero): " << fileheader.reserved1 << endl;
        cout << "Pole zarezerwowane (zwykle zero): " << fileheader.reserved2 << endl;
        cout << "Pozycja danych obrazowych w pliku: " << fileheader.offbits << endl << endl;
        cout << "Infoheader:" << endl;
        cout << "Rozmiar naglowka: " << infoheader.size << endl;
        cout << "Szerokosc obrazu: " << infoheader.width << endl;
        cout << "Wysokosc obrazu: " << infoheader.height << endl;
        cout << "Liczba platow: " << infoheader.planes << endl;
        cout << "Liczba bitow na piksel: " << infoheader.bitCount << endl;
        cout << "Algorytm kompresji: " << infoheader.compression << endl;
        cout << "Rozmiar obrazu: " << infoheader.imageSize << endl;
        cout << "Rozdzielczosc pozioma: " << infoheader.xresolution << endl;
        cout << "Rozdzielczosc pionowa: " << infoheader.yresolution << endl;
        cout << "Liczba kolorow w palecie: " << infoheader.uColours << endl;
        cout << "Liczba waznych kolorow w palecie: " << infoheader.importantColours << endl << endl;
    }
}
void kolory(ifstream& ifs, int width, int height, int bitCount, int size, int padding, char& p, unsigned char*& colourR, unsigned char*& colourG, unsigned char*& colourB, unsigned char*& color) {
    if (bitCount != 24) {
        cout << "This program works only for 24-bit bmp" << endl;  //sprawdzenie czy bmp jest 24-bit
        exit(0);
    }
    color = (unsigned char*)malloc(size);   //rezerwowanie pamieci
    ifs.read(reinterpret_cast<char*>(color), size); //wartosci pikseli calego pliku
    colourR = (unsigned char*)malloc(size);
    colourG = (unsigned char*)malloc(size);
    colourB = (unsigned char*)malloc(size);
    int licznik = 0;
    if (color && colourB && colourG && colourR) {
        for (int i = height - 1; i >= 0; i--) {
            for (int j = 0; j < width; j++) {
                int p = (i * width + j) * 3 + i * padding;
                if (licznik < size) {
                    colourB[licznik] = color[p];
                    colourG[licznik] = color[p + 1];     //rozdzielanie pikseli na wektory R,G,B
                    colourR[licznik] = color[p + 2];
                    licznik++;
                }
            }
        }
    }
    if (padding != 0) {
        unsigned char** paddingar = new unsigned char* [height];
        for (int i = 0; i < height; i++) {
            paddingar[i] = new unsigned char[width + padding];
        }
        int buf = 0;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < (width + padding); j++) {
                if (buf < size) {
                    paddingar[i][j] = color[buf];
                    buf++;
                }
            }
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width + padding; j++) { //padding
                    if (j >= width) {
                        p = paddingar[i][j];
                    }
                }
            }
        }
        delete[] paddingar;
    }
}
void greyscale(int size, unsigned char*& grey, unsigned char* colourR, unsigned char* colourG, unsigned char* colourB) {
    unsigned char R, G, B;
    grey = (unsigned char*)malloc(size);
    for (int i = 0; i < size; i++) {
        R = colourR[i];
        G = colourG[i];
        B = colourB[i];
        grey[i] = (R * 0.3) + (G * 0.59) + (B * 0.11);  //zmiana na greyscale i zapis w wektorze
    }
}
void zapisywanie(ofstream& ofs, fileheader fileheader, infoheader infoheader, char& p, int height, int width, int padding, unsigned char* grey, unsigned char** wyjscie) {
    ofs.write(reinterpret_cast<char*>(&fileheader.type), 2);
    ofs.write(reinterpret_cast<char*>(&fileheader.size), 4);
    ofs.write(reinterpret_cast<char*>(&fileheader.reserved1), 2);
    ofs.write(reinterpret_cast<char*>(&fileheader.reserved2), 2);
    ofs.write(reinterpret_cast<char*>(&fileheader.offbits), 4);
    ofs.write(reinterpret_cast<char*>(&infoheader.size), 4);
    ofs.write(reinterpret_cast<char*>(&infoheader.width), 4);
    ofs.write(reinterpret_cast<char*>(&infoheader.height), 4);
    ofs.write(reinterpret_cast<char*>(&infoheader.planes), 2);
    ofs.write(reinterpret_cast<char*>(&infoheader.bitCount), 2);
    ofs.write(reinterpret_cast<char*>(&infoheader.compression), 4);
    ofs.write(reinterpret_cast<char*>(&infoheader.imageSize), 4);
    ofs.write(reinterpret_cast<char*>(&infoheader.xresolution), 4);
    ofs.write(reinterpret_cast<char*>(&infoheader.yresolution), 4);
    ofs.write(reinterpret_cast<char*>(&infoheader.uColours), 4);
    ofs.write(reinterpret_cast<char*>(&infoheader.importantColours), 4);
    for (int i = height - 1; i >= 0; i--) {
        for (int j = 0; j < width + padding; j++) {
            if (j < width) {                                               //piksele
                ofs << wyjscie[i][j] << wyjscie[i][j] << wyjscie[i][j];
            }
            else if (j >= width) { //padding
                ofs << p;
            }
        }
    }
}
void sobel(int width, int height, int padding, int size, unsigned char*& grey, unsigned char**& wyjscie) {
    int s1[9] = { -1,0,1,-2,0,2,-1,0,1 };
    int s2[9] = { 0,1,2,-1,0,1,-2,-1,0 };
    int s3[9] = { 1,2,1,0,0,0,-1,-2,-1 };
    int s4[9] = { 2,1,0,1,0,-1,0,-1,-2 };
    int s5[9] = { 1,0,-1,2,0,-2,1,0,-1 }; //maski
    int s6[9] = { 0,-1,-2,1,0,-1,2,1,0 };
    int s7[9] = { -1,-2,-1,0,0,0,1,2,1 };
    int s8[9] = { -2,-1,0,-1,0,1,0,1,2 };
    int a, b, c, d, e, f, g, h;
    wyjscie = new unsigned char* [height];
    for (int i = 0; i < height; i++) {
        wyjscie[i] = new unsigned char[width];  //rezerwowanie pamieci dla dynamicznych tablic dwuwymiarowych
    }
    unsigned char** wejscie = new unsigned char* [height];
    for (int i = 0; i < height; i++) {
        wejscie[i] = new unsigned char[width];
    }
    int buf = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < (width); j++) {
            if (buf < size) {
                wejscie[i][j] = grey[buf];  //rozdzielanie wektora do tablicy dwuwymiarowej
                buf++;
            }
        }
    }
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            a = 0;
            b = 0;
            c = 0;
            d = 0;
            e = 0;
            f = 0;
            g = 0;
            h = 0;
            a = wejscie[i][j] * s1[4] + wejscie[i - 1][j] * s1[1] + wejscie[i][j - 1] * s1[3] + wejscie[i - 1][j - 1] * s1[0] + wejscie[i + 1][j] * s1[7] + wejscie[i][j + 1] * s1[5] + wejscie[i + 1][j + 1] * s1[8] + wejscie[i + 1][j - 1] * s1[6] + wejscie[i - 1][j + 1] * s1[2];
            b = wejscie[i][j] * s2[4] + wejscie[i - 1][j] * s2[1] + wejscie[i][j - 1] * s2[3] + wejscie[i - 1][j - 1] * s2[0] + wejscie[i + 1][j] * s2[7] + wejscie[i][j + 1] * s2[5] + wejscie[i + 1][j + 1] * s2[8] + wejscie[i + 1][j - 1] * s2[6] + wejscie[i - 1][j + 1] * s2[2];
            c = wejscie[i][j] * s3[4] + wejscie[i - 1][j] * s3[1] + wejscie[i][j - 1] * s3[3] + wejscie[i - 1][j - 1] * s3[0] + wejscie[i + 1][j] * s3[7] + wejscie[i][j + 1] * s3[5] + wejscie[i + 1][j + 1] * s3[8] + wejscie[i + 1][j - 1] * s3[6] + wejscie[i - 1][j + 1] * s3[2];
            d = wejscie[i][j] * s4[4] + wejscie[i - 1][j] * s4[1] + wejscie[i][j - 1] * s4[3] + wejscie[i - 1][j - 1] * s4[0] + wejscie[i + 1][j] * s4[7] + wejscie[i][j + 1] * s4[5] + wejscie[i + 1][j + 1] * s4[8] + wejscie[i + 1][j - 1] * s4[6] + wejscie[i - 1][j + 1] * s4[2];
            e = wejscie[i][j] * s5[4] + wejscie[i - 1][j] * s5[1] + wejscie[i][j - 1] * s5[3] + wejscie[i - 1][j - 1] * s5[0] + wejscie[i + 1][j] * s5[7] + wejscie[i][j + 1] * s5[5] + wejscie[i + 1][j + 1] * s5[8] + wejscie[i + 1][j - 1] * s5[6] + wejscie[i - 1][j + 1] * s5[2];
            f = wejscie[i][j] * s6[4] + wejscie[i - 1][j] * s6[1] + wejscie[i][j - 1] * s6[3] + wejscie[i - 1][j - 1] * s6[0] + wejscie[i + 1][j] * s6[7] + wejscie[i][j + 1] * s6[5] + wejscie[i + 1][j + 1] * s6[8] + wejscie[i + 1][j - 1] * s6[6] + wejscie[i - 1][j + 1] * s6[2];
            g = wejscie[i][j] * s7[4] + wejscie[i - 1][j] * s7[1] + wejscie[i][j - 1] * s7[3] + wejscie[i - 1][j - 1] * s7[0] + wejscie[i + 1][j] * s7[7] + wejscie[i][j + 1] * s7[5] + wejscie[i + 1][j + 1] * s7[8] + wejscie[i + 1][j - 1] * s7[6] + wejscie[i - 1][j + 1] * s7[2];
            h = wejscie[i][j] * s8[4] + wejscie[i - 1][j] * s8[1] + wejscie[i][j - 1] * s8[3] + wejscie[i - 1][j - 1] * s8[0] + wejscie[i + 1][j] * s8[7] + wejscie[i][j + 1] * s8[5] + wejscie[i + 1][j + 1] * s8[8] + wejscie[i + 1][j - 1] * s8[6] + wejscie[i - 1][j + 1] * s8[2];
            wyjscie[i][j] = (abs(a) + abs(b) + abs(c) + abs(d) + abs(e) + abs(f) + abs(g) + abs(h)) / 8; //operator sobela dla kazdego piksela
        }
    }
    for (int i = 0; i < width; i++) {
        wyjscie[0][i] = 0;
    }
    for (int i = 0; i < height; i++) {
        wyjscie[i][0] = 0;                             //krawedzie
    }
    for (int i = 0; i < width; i++) {
        wyjscie[height - 1][i] = 0;
    }
    for (int i = 0; i < height; i++) {
        wyjscie[i][width - 1] = 0;
    }
    delete[] wejscie;
}
int main() {
    int width, height, bitCount, size, padding;
    unsigned char* color = 0;
    char p;
    string nazwa1, nazwa2;
    cout << "Podaj nazwe pliku wejsciowego: " << endl;
    cin >> nazwa1;
    nazwa1.append(".bmp");
    cout << endl;
    fileheader fileheader;
    unsigned char* colourR = 0;
    unsigned char* colourG = 0;
    unsigned char* colourB = 0;
    unsigned char* grey = 0;
    unsigned char** wyjscie = 0;
    infoheader infoheader;
    ifstream ifs;
    ifs.open(nazwa1, ios::binary | ios::in);
    if (!ifs) {
        cout << "Nie udalo sie otworzyc pliku" << endl;
        exit(0);
    }
    odczytywaniefileheader(ifs, fileheader, size);
    odczytywanieinfoheader(ifs, infoheader, width, height, bitCount, padding);
    wyswietlanie(fileheader, infoheader);
    kolory(ifs, width, height, bitCount, size, padding, p, colourR, colourG, colourB, color);
    cout << "Podaj nazwe pliku wyjsciowego: " << endl;
    cin >> nazwa2;
    nazwa2.append(".bmp");
    ofstream ofs(nazwa2, ios::binary);
    greyscale(size, grey, colourR, colourG, colourB);
    sobel(width, height, padding, size, grey, wyjscie);
    zapisywanie(ofs, fileheader, infoheader, p, height, width, padding, grey, wyjscie);
    free(colourB);
    free(colourR);
    free(grey);
    free(colourG);
    delete[] wyjscie;
    ifs.close();
    ofs.close();
}
