/******************************************************************************
 * Laborator 04 Krivky - Zaklady pocitacove grafiky - IZG
 * ihulik@fit.vutbr.cz
 *
 *
 * Popis: Hlavicky funkci pro funkce studentu
 *
 * Opravy a modifikace:
 * - isvoboda@fit.vutbr.cz
 * - ireich@fit.vutbr.cz
 */

#include "student.h"
#include "globals.h"
#include "main.h"
#include "vector.h"

#include <math.h>
#include <time.h>

// Viz hlavicka vector.h
USE_VECTOR_OF(Point2d, point2d_vec)
#define point2d_vecGet(pVec, i) (*point2d_vecGetPtr((pVec), (i)))

/* Secte dva body Point2d a vysledek vrati v result (musi byt inicializovan a
 * alokovan)*/
void addPoint2d(const Point2d *a, const Point2d *b, Point2d *result) {
    IZG_ASSERT(result);
    result->x = a->x + b->x;
    result->y = a->y + b->y;
}

/* Vynasobi bod Point2d skalarni hodnotou typu double a vysledek vrati v result
 * (musi byt inicializovan a alokovan)*/
void mullPoint2d(double val, const Point2d *p, Point2d *result) {
    IZG_ASSERT(result);
    result->x = p->x * val;
    result->y = p->y * val;
}

/**
 * Inicializace ridicich bodu horni trajektorie ve vykreslovacim okne.
 * Pocatek souradnicove soustavy je v levem hornim rohu okna. Xova souradnice
 * roste smerem doprava, Y smerem dolu.
 * @param points vystup, kam se pridavaji ridici body
 * @param offset_x posun vsech ridicich bodu v horizontalnim smeru (aby se mohli
 * souradnice zadavat s pocatkem [0,0])
 * @param offset_y posun vsech ridicich bodu ve vertikalnim smeru
 */
void initControlPointsUp(S_Vector **points, int offset_x, int offset_y) {
    *points = vecCreateEmpty(sizeof(Point2d));
    Point2d p;
    p.x = 0;
    p.y = -1;
    point2d_vecPushBack(*points, p);

    p.x = 40;
    p.y = -250;
    point2d_vecPushBack(*points, p);
    p.x = 160;
    p.y = -250;
    point2d_vecPushBack(*points, p);

    p.x = 200;
    p.y = 0;
    point2d_vecPushBack(*points, p);

    p.x = 210;
    p.y = -180;
    point2d_vecPushBack(*points, p);
    p.x = 350;
    p.y = -180;
    point2d_vecPushBack(*points, p);

    p.x = 360;
    p.y = 0;
    point2d_vecPushBack(*points, p);

    p.x = 390;
    p.y = -120;
    point2d_vecPushBack(*points, p);
    p.x = 430;
    p.y = -120;
    point2d_vecPushBack(*points, p);

    p.x = 460;
    p.y = 0;
    point2d_vecPushBack(*points, p);

    p.x = 470;
    p.y = -70;
    point2d_vecPushBack(*points, p);
    p.x = 525;
    p.y = -70;
    point2d_vecPushBack(*points, p);

    p.x = 535;
    p.y = 0;
    point2d_vecPushBack(*points, p);

    p.x = 545;
    p.y = -40;
    point2d_vecPushBack(*points, p);
    p.x = 575;
    p.y = -40;
    point2d_vecPushBack(*points, p);

    p.x = 585;
    p.y = -1;
    point2d_vecPushBack(*points, p);

    Point2d offset = {static_cast<double>(offset_x),
                      static_cast<double>(offset_y), 1.0};
    for (int i = 0; i < (*points)->size; i++) {
        addPoint2d(point2d_vecGetPtr(*points, i), &offset,
                   point2d_vecGetPtr(*points, i));
    }
}

void initControlPointsDown(S_Vector **points, int offset_x, int offset_y) {
    /* == TODO ==
     * Uloha c.2
     * Nasledujuci volanni funkce initControlPointsUp(.) smazte a nahradte
     * vlastnim kodem, ktery inicializuje ridici body tak, aby byla trajektorie
     * spojita (C1). Muzete zkopirovat kod funkce initControlPointsUp(.) a
     * upravit primo souradnice bodu v kodu. POCITEJTE S -Y SOURADNICEMI
     * RIDICICH BODU A MENTE JEJICH X SLOŽKU!
     */
    // initControlPointsUp(points, offset_x, offset_y);

    *points = vecCreateEmpty(sizeof(Point2d));
    Point2d p;
    p.x = 0;
    p.y = -1;
    point2d_vecPushBack(*points, p);

    p.x = 40;
    p.y = -250;
    point2d_vecPushBack(*points, p);
    p.x = 160;
    p.y = -250;
    point2d_vecPushBack(*points, p);

    p.x = 200;
    p.y = 0;
    point2d_vecPushBack(*points, p);

    p.x = 200 + (200 - 160) * (double)180 / 250; //
    p.y = 180;
    point2d_vecPushBack(*points, p);
    p.x = 360 - (390 - 360) * (double)180 / 120; //
    p.y = 180;
    point2d_vecPushBack(*points, p);

    p.x = 360;
    p.y = 0;
    point2d_vecPushBack(*points, p);

    p.x = 390;
    p.y = -120;
    point2d_vecPushBack(*points, p);
    p.x = 430;
    p.y = -120;
    point2d_vecPushBack(*points, p);

    p.x = 460;
    p.y = 0;
    point2d_vecPushBack(*points, p);

    p.x = 460 + (460 - 430) * (double)70 / 120; //
    p.y = 70;
    point2d_vecPushBack(*points, p);
    p.x = 535 - (545 - 535) * (double)70 / 40; //
    p.y = 70;
    point2d_vecPushBack(*points, p);

    p.x = 535;
    p.y = 0;
    point2d_vecPushBack(*points, p);

    p.x = 545;
    p.y = -40;
    point2d_vecPushBack(*points, p);
    p.x = 575;
    p.y = -40;
    point2d_vecPushBack(*points, p);

    p.x = 585;
    p.y = -1;
    point2d_vecPushBack(*points, p);

    Point2d offset = {static_cast<double>(offset_x),
                      static_cast<double>(offset_y), 1.0};
    for (int i = 0; i < (*points)->size; i++) {
        addPoint2d(point2d_vecGetPtr(*points, i), &offset,
                   point2d_vecGetPtr(*points, i));
    }
}

/**
 * Implementace vypoctu Bezierove kubiky.
 * @param P1,P2,P3,P4 ridici body kubiky
 * @param resolution pocet bodu na krivke, ktere chceme vypocitat
 * @param trajectory_points vystupni vektor bodu kubiky (nemazat, jen pridavat
 * body)
 */
void bezierCubic(const Point2d *P1, const Point2d *P2, const Point2d *P3,
                 const Point2d *P4, const int resolution,
                 S_Vector *trajectory_points) {

    /* == TODO ==
     * Soucast Ulohy c.1:
     * Sem pridejte kod vypoctu Bezierove kubiky. Body krivky pridavejte do
     * trajectory_points.
     */
    for (int i = 0; i < resolution; i++) {
        double t = (double)i / (resolution - 1);
        Point2d point{0, 0, 0};
        Point2d tmp;

        tmp = *P1;
        mullPoint2d((1 - t) * (1 - t) * (1 - t), &tmp, &tmp);
        addPoint2d(&tmp, &point, &point);

        tmp = *P2;
        mullPoint2d(3 * t * (1 - t) * (1 - t), &tmp, &tmp);
        addPoint2d(&tmp, &point, &point);

        tmp = *P3;
        mullPoint2d(3 * t * t * (1 - t), &tmp, &tmp);
        addPoint2d(&tmp, &point, &point);

        tmp = *P4;
        mullPoint2d(t * t * t, &tmp, &tmp);
        addPoint2d(&tmp, &point, &point);

        vecPushBack(trajectory_points, &point);
    }
}

/*
 * Implementace vypoctu trajektorie, ktera se sklada z Bezierovych kubik.
 * @param resolution pocet bodu krivky, ktere mame urcit
 * @param control_points ridici body krivky
 * @param trajectory_points vystupni body zakrivene trajektorie
 */
void bezierCubicsTrajectory(int resolution, const S_Vector *control_points,
                            S_Vector *trajectory_points) {
    // Toto musi byt na zacatku funkce, nemazat.
    point2d_vecClean(trajectory_points);

    /* == TODO ==
     * !!!!!!!!!!!!!!!!!!!!!!
     * Uloha c.1
     * Ziskejte postupne 4 ridici body a pro kazdou ctverici vypocitejte body
     * Bezierovy kubiky.
     */

    for (int i = 0; i < 5; i++) {
        Point2d *P1 = (Point2d *)vecGetPtr(control_points, 3 * i + 0);
        Point2d *P2 = (Point2d *)vecGetPtr(control_points, 3 * i + 1);
        Point2d *P3 = (Point2d *)vecGetPtr(control_points, 3 * i + 2);
        Point2d *P4 = (Point2d *)vecGetPtr(control_points, 3 * i + 3);

        bezierCubic(P1, P2, P3, P4, resolution, trajectory_points);
    }
}
