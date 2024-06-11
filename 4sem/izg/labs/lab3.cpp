/**
 * @file        student.cpp
 * @author      Ladislav Mosner, VUT FIT Brno, imosner@fit.vutbr.cz
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @author      Kamil Behun, VUT FIT Brno, ibehun@fit.vutbr.cz
 * @date        11.03.2018
 *
 * @brief       Deklarace funkci studentu. DOPLNUJI STUDENTI
 *
 */

#include "student.h"
#include "base.h"
#include "globals.h"
#include <algorithm>
#include <math.h>

/**
 * @brief Vraci barvu pixelu z pozice [x, y]
 * @param[in] x X souradnice pixelu
 * @param[in] y Y souradnice pixelu
 * @return Barva pixelu na pozici [x, y] ve formatu RGBA
 */
RGBA getPixel(int x, int y) {
    if (x >= width || y >= height || x < 0 || y < 0) {
        IZG_ERROR("Pristup do framebufferu mimo hranice okna\n");
    }
    return framebuffer[y * width + x];
}

/**
 * @brief Nastavi barvu pixelu na pozici [x, y]
 * @param[in] x X souradnice pixelu
 * @param[in] y Y souradnice pixelu
 * @param[in] color Barva pixelu ve formatu RGBA
 */
void putPixel(int x, int y, RGBA color) {
    if (x >= width || y >= height || x < 0 || y < 0) {
        IZG_ERROR("Pristup do framebufferu mimo hranice okna\n");
    }
    framebuffer[y * width + x] = color;
}

/**
 * @brief Vykresli usecku se souradnicemi [x1, y1] a [x2, y2]
 * @param[in] x1 X souradnice 1. bodu usecky
 * @param[in] y1 Y souradnice 1. bodu usecky
 * @param[in] x2 X souradnice 2. bodu usecky
 * @param[in] y2 Y souradnice 2. bodu usecky
 * @param[in] color Barva pixelu usecky ve formatu RGBA
 * @param[in] arrow Priznak pro vykresleni sipky (orientace hrany)
 */
void drawLine(int x1, int y1, int x2, int y2, RGBA color, bool arrow = false) {

    if (arrow) {
        // Sipka na konci hrany
        double vx1 = x2 - x1;
        double vy1 = y2 - y1;
        double length = sqrt(vx1 * vx1 + vy1 * vy1);
        double vx1N = vx1 / length;
        double vy1N = vy1 / length;
        double vx1NN = -vy1N;
        double vy1NN = vx1N;
        int w = 3;
        int h = 10;
        int xT = (int)(x2 + w * vx1NN - h * vx1N);
        int yT = (int)(y2 + w * vy1NN - h * vy1N);
        int xB = (int)(x2 - w * vx1NN - h * vx1N);
        int yB = (int)(y2 - w * vy1NN - h * vy1N);
        pinedaTriangle(Point(x2, y2), Point(xT, yT), Point(xB, yB), color,
                       color, false);
    }

    bool steep = abs(y2 - y1) > abs(x2 - x1);

    if (steep) {
        SWAP(x1, y1);
        SWAP(x2, y2);
    }

    if (x1 > x2) {
        SWAP(x1, x2);
        SWAP(y1, y2);
    }

    const int dx = x2 - x1, dy = abs(y2 - y1);
    const int P1 = 2 * dy, P2 = P1 - 2 * dx;
    int P = 2 * dy - dx;
    int y = y1;
    int ystep = 1;
    if (y1 > y2)
        ystep = -1;

    for (int x = x1; x <= x2; x++) {
        if (steep) {
            if (y >= 0 && y < width && x >= 0 && x < height) {
                putPixel(y, x, color);
            }
        } else {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                putPixel(x, y, color);
            }
        }

        if (P >= 0) {
            P += P2;
            y += ystep;
        } else {
            P += P1;
        }
    }
}

/**
 * @brief Vyplni a vykresli trojuhelnik
 * @param[in] v1 Prvni bod trojuhelniku
 * @param[in] v2 Druhy bod trojuhelniku
 * @param[in] v3 Treti bod trojuhelniku
 * @param[in] color1 Barva vyplne trojuhelniku
 * @param[in] color2 Barva hranice trojuhelniku
 * @param[in] arrow Priznak pro vykresleni sipky (orientace hrany)
 *
 * SPOLECNY UKOL. Doplnuji studenti se cvicicim.
 */

void pinedaTriangle(const Point &v1, const Point &v2, const Point &v3,
                    const RGBA &color1, const RGBA &color2, bool arrow) {
    // Nalezeni obalky (minX, maxX), (minY, maxY) trojuhleniku.

    //////// DOPLNTE KOD /////////
    int minx = MIN(MIN(v1.x, v2.x), v3.x);
    int miny = MIN(MIN(v1.y, v2.y), v3.y);

    int maxx = MAX(MAX(v1.x, v2.x), v3.x);
    int maxy = MAX(MAX(v1.y, v2.y), v3.y);

    // Oriznuti obalky (minX, maxX, minY, maxY) trojuhleniku podle rozmeru okna.

    //////// DOPLNTE KOD /////////
    minx = MAX(minx, 0);
    minx = MIN(minx, width);

    maxx = MAX(maxx, 0);
    maxx = MIN(maxx, width);

    miny = MAX(miny, 0);
    miny = MIN(miny, height);

    maxy = MAX(maxy, 0);
    maxy = MIN(maxy, height);

    // Spocitani parametru hranove funkce (deltaX, deltaY) pro kazdou hranu.
    // Hodnoty deltaX, deltaY jsou souradnicemi vektoru, ktery ma pocatek
    // v prvnim vrcholu hrany, konec v druhem vrcholu.
    // Vypocet prvnotni hodnoty hranove funkce.

    //////// DOPLNTE KOD /////////
    auto ep = EdgeParams{3};
    ep[0] = (EdgeParam{v2.x - v1.x, v2.y - v1.y});
    ep[1] = (EdgeParam{v3.x - v2.x, v3.y - v2.y});
    ep[2] = (EdgeParam{v1.x - v3.x, v1.y - v3.y});

    EdgeFncValues efv(3);
    efv[0] = ((miny - v1.y) * ep[0].deltaX - (minx - v1.x) * ep[0].deltaY);
    efv[1] = ((miny - v2.y) * ep[1].deltaX - (minx - v2.x) * ep[1].deltaY);
    efv[2] = ((miny - v3.y) * ep[2].deltaX - (minx - v3.x) * ep[2].deltaY);

    // Vyplnovani: Cyklus pres vsechny body (x, y) v obdelniku (minX, minY),
    // (maxX, maxY). Pro aktualizaci hodnot hranove funkce v bode P (x +/- 1, y)
    // nebo P (x, y +/- 1) vyuzijte hodnoty hranove funkce E (x, y) z bodu P (x,
    // y).

    //////// DOPLNTE KOD /////////
    int lineLength = maxx - minx;

    for (int y = miny; y < maxy; y++) {
        for (int x = minx; x < maxx; x++) {
            if (efv[0] >= 0 && efv[1] >= 0 && efv[2] >= 0) {
                putPixel(x, y, color1);
            }

            efv[0] -= ep[0].deltaY;
            efv[1] -= ep[1].deltaY;
            efv[2] -= ep[2].deltaY;
        }

        efv[0] += ep[0].deltaX + ep[0].deltaY * lineLength;
        efv[1] += ep[1].deltaX + ep[1].deltaY * lineLength;
        efv[2] += ep[2].deltaX + ep[2].deltaY * lineLength;
    }

    // Prekresleni hranic trojuhelniku barvou color2.
    drawLine(v1.x, v1.y, v2.x, v2.y, color2, arrow);
    drawLine(v2.x, v2.y, v3.x, v3.y, color2, arrow);
    drawLine(v3.x, v3.y, v1.x, v1.y, color2, arrow);
}

/**
 * @brief Vyplni a vykresli polygon
 * @param[in] points Pole bodu polygonu
 * @param[in] size Pocet bodu polygonu (velikost pole "points")
 * @param[in] color1 Barva vyplne polygonu
 * @param[in] color2 Barva hranice polygonu
 *
 * SAMOSTATNY BODOVANY UKOL. Doplnuji pouze studenti.
 */
void pinedaPolygon(const Point *points, const int size, const RGBA &color1,
                   const RGBA &color2) {
    // Pri praci muzete vyuzit pro vas predpripravene datove typy z base.h.,
    // napriklad:
    //
    //      Pro ukladani parametru hranovych funkci muzete vyuzit prichystany
    //      vektor parametru hranovych funkci "EdgeParams":
    //
    //          EdgeParams edgeParams(size)                         // Vytvorite
    //          vektor (pole) "edgeParams" parametru hranovych funkci o
    //          velikosti "size". edgeParams[i].deltaX, edgeParams[i].deltaY //
    //          Pristup k parametrum (deltaX, deltaY) hranove funkce v poli
    //          "edgeParams" na indexu "i".
    //
    //      Pro ukladani hodnot hranovych funkci muzete vyuzit prichystany
    //      vektor hodnot hranovych funkci "EdgeFncValues":
    //
    //          EdgeFncValues edgeFncValues(size)                   // Vytvorite
    //          vektor (pole) "edgeFncValues" hodnot hranovych funkci o
    //          velikosti "size". edgeFncValues[i] // Pristup k hodnote hranove
    //          funkce v poli "edgeFncValues" na indexu "i".
    //

    // Nalezeni obalky (minX, maxX), (minY, maxY) polygonu.

    //////// DOPLNTE KOD /////////
    if (size <= 1) {
        return;
    }
    int minx = 0;
    int miny = 0;
    int maxx = width;
    int maxy = height;
    for (int i = 0; i < size; i++) {
        minx = MIN(minx, points[i].x);
        miny = MIN(miny, points[i].y);
        maxx = MAX(maxx, points[i].x);
        maxy = MAX(maxy, points[i].y);
    }

    // Oriznuti obalky (minX, maxX), (minY, maxY) polygonu podle rozmeru okna

    //////// DOPLNTE KOD /////////
    minx = MAX(minx, 0);
    minx = MIN(minx, width);

    maxx = MAX(maxx, 0);
    maxx = MIN(maxx, width);

    miny = MAX(miny, 0);
    miny = MIN(miny, height);

    maxy = MAX(maxy, 0);
    maxy = MIN(maxy, height);

    // Spocitani parametru (deltaX, deltaY) hranove funkce pro kazdou hranu.
    // Hodnoty deltaX, deltaY jsou souradnicemi vektoru, ktery ma pocatek
    // v prvnim vrcholu hrany, konec v druhem vrcholu.
    // Vypocet prvnotnich hodnot hranovych funkci pro jednotlive hrany.

    //////// DOPLNTE KOD /////////
    EdgeParams ep(size);
    for (int i = 0; i < size; i++) {
        auto curr = points[i];
        auto next = points[(i + 1) % size];
        // ep[0] = (EdgeParam{v2.x - v1.x, v2.y - v1.y});
        ep[i] = EdgeParam(next.x - curr.x, next.y - curr.y);
    }

    EdgeFncValues efv(size);
    for (int i = 0; i < size; i++) {
        auto curr = points[i];
        // efv[0] = ((miny - v1.y) * ep[0].deltaX  - (minx - v1.x) *
        // ep[0].deltaY);
        efv[i] =
            (miny - curr.y) * ep[i].deltaX - (minx - curr.x) * ep[i].deltaY;
    }

    // Test konvexnosti polygonu

    //////// DOPLNTE KOD /////////

    // Vyplnovani: Cyklus pres vsechny body (x, y) v obdelniku (minX, minY),
    // (maxX, maxY). Pro aktualizaci hodnot hranove funkce v bode P (x +/- 1, y)
    // nebo P (x, y +/- 1) vyuzijte hodnoty hranove funkce E (x, y) z bodu P (x,
    // y) */

    //////// DOPLNTE KOD /////////
    int lineLength = maxx - minx;

    for (int y = miny; y < maxy; y++) {
        for (int x = minx; x < maxx; x++) {

            // check if all values of EF are positive
            bool all_positive = true;
            for (int i = 0; i < size; i++) {
                all_positive = all_positive && efv[i] >= 0;
            }
            if (all_positive && x >= 0 && y >= 0 && x < width && y < height) {
                putPixel(x, y, color1);
            }

            // go to next point horizontally
            for (int i = 0; i < size; i++) {
                // efv[0] -= ep[0].deltaY;
                efv[i] -= ep[i].deltaY;
            }
        }

        // go to next line vertically, and the whole line back horizontally
        for (int i = 0; i < size; i++) {
            // efv[0] += ep[0].deltaX + ep[0].deltaY * lineLength;
            efv[i] += ep[i].deltaX + ep[i].deltaY * lineLength;
        }
    }

    // Prekresleni hranic polygonu barvou color2.
    for (int i = 0; i < size; i++) {
        drawLine(points[i].x, points[i].y, points[(i + 1) % size].x,
                 points[(i + 1) % size].y, color2 /*, true*/);
    }
}
