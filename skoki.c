#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "drzewo.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 720
#define M_PI 3.1415
#define skalaSkoczni 1000.0
#define skalaGory 3000.0
#define wydluzenieGory 1000

Node* root = NULL; // korzen drzewa

/* Struktura Wektor 2D */
typedef struct
{
    float x;
    float y;
} VECTOR2;

/* Dodaje do siebie inny wektor */
void VECTOR2_Addself(VECTOR2* self, VECTOR2 Vector2);

/* Zwraca sume 2 wektorow */
VECTOR2 VECTOR2_GetSum(VECTOR2 first, VECTOR2 second);

VECTOR2 VECTOR2_SrodekOdcinka(VECTOR2 first, VECTOR2 second);

VECTOR2 VECTOR2_Roznica(VECTOR2 first, VECTOR2 second);

/* Wypisuje wartosci wektora */
void VECTOR2_Print(VECTOR2* self);

/* odleglosc miedzy dwoma punktami */
float VECTOR2_Odleglosc(VECTOR2 Vec1, VECTOR2 Vec2);

/* kat miedzy osia x  a prosta ktora przechodzi przez dwa punkty */
float VECTOR2_ATAN2(VECTOR2 Vec1, VECTOR2 Vec2);


/* Struktura Skoczka Zawsze przed renderowaniem mapy najpierw obliczane jest resultGravity biorac pod uwage wiatr i pochylenie skoczka, pozniej predkoscSkoczka po uwzglednieniu przyspieszenia*/
typedef struct
{
    float defaultGravity; //= 10; // tego nie zmieniac
    float resultGravity; //= 10; // grwaitacja traktowana jako przyspieszenie czyli np jezeli jest 10 to velocity.y skoczka bedzie o 10 mniejsze w ciagu sekundy
    VECTOR2 velocity; // predkosc podczas lotu
    VECTOR2 acceleration; // przyspieszenie podczas lotu
    VECTOR2 position; // pozycja podczas lotu
    VECTOR2 srodekNart; // pozycja srodka nart
    float rideAcceleration; // przyspieszenie podczas jazdy
    float rideVelocity; // predkosc podczas jazdy
    int aproximationPoints; // = 1000;
    int aproximationPointsGora;
    int currentPoint; //=0; // punkt na skoczni w ktorym znajduje sie srodek nart skoczka
    int isRiding; // = 1; // prawda/falsz czy skoczek jedzie po skoczni  //to może bool dać?
    int jumped;
    int zjezdzaZGory;
    int currentPointGory;
    int wynik;
    float radianAngle; // = 0; // kat tekstury skoczka w radianach czyli taki jaki mozna podac bezposrednio do funkcji ktora rysuje teksture
    float dlugoscNart; // = 2; //??????????????????????????????????????????? to trzeba zmienic!!!!!!!!!!      dlugosc nart skoczka w pikselach
    float resztaDrogi; // reszta drogi ktora nie zostala przypisana w poprzedniej petli

    float wysokosc;
    float szerokosc;

    char* name; // imie
}SKOCZEK;

void SKOCZEK_SetPosition(SKOCZEK* self, VECTOR2 position);

void SKOCZEK_DopasujDoSkoczni(SKOCZEK* self, VECTOR2 tablicaPunktow[]);

void SKOCZEK_PrzesunO(SKOCZEK* self, float s, VECTOR2 tablicaPunktow[]);

void SKOCZEK_SymulujJazde(SKOCZEK* self, float deltaTime, VECTOR2 tablicaPunktow[]);

float FUNCTION(float x);

VECTOR2* FUNCTION_GetPoints(int x, float dziedzina);



/*  -------------------------------- * WEKTORY * ---------------------------------- */
void VECTOR2_Addself(VECTOR2* self, VECTOR2 Vector2)
{
    self->x += Vector2.x;
    self->y += Vector2.y;
}

VECTOR2 VECTOR2_GetSum(VECTOR2 first, VECTOR2 second)
{
    VECTOR2 newVector;
    newVector.x = first.x + second.x;
    newVector.y = first.y + second.y;
    return newVector;
}

VECTOR2 VECTOR2_SrodekOdcinka(VECTOR2 first, VECTOR2 second)
{
    VECTOR2 srodek;
    srodek.x = (first.x + second.x) / 2;
    srodek.y = (first.y + second.y) / 2;
    return srodek;
}

VECTOR2 VECTOR2_Roznica(VECTOR2 first, VECTOR2 second)
{
    VECTOR2 roznica;
    roznica.x = first.x - second.x;
    roznica.y = first.y - second.y;
    return roznica;
}

void VECTOR2_Print(VECTOR2* self)
{
    printf("x: %f\ny: %f\n", self->x, self->y);
}

float VECTOR2_Odleglosc(VECTOR2 Vec1, VECTOR2 Vec2)
{
    return sqrt(pow(Vec1.x - Vec2.x, 2.0) + pow(Vec1.y - Vec2.y, 2.0));
}

float VECTOR2_DlugoscVectora(VECTOR2 vec)
{
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

VECTOR2 VECTOR2_Unit(VECTOR2 vec)
{
    VECTOR2 Unit;
    float dlugosc = VECTOR2_DlugoscVectora(vec);
    Unit.x = vec.x / dlugosc;
    Unit.y = vec.y / dlugosc;
}

float VECTOR2_ATAN2Strzalka(VECTOR2 Vec1, VECTOR2 Vec2)
{
    float dY = fabs(Vec2.y - Vec1.y);
    float dX = fabs(Vec2.x - Vec1.x);
    
    float wynik=atan2f(dY, dX);

    if (Vec2.x > Vec1.x && Vec2.y < Vec1.y)
    {
        wynik = 2 * M_PI - wynik;
    }
    else if (Vec2.x < Vec1.x && Vec2.y > Vec1.y)
    {
        wynik = M_PI - wynik;
    }
    else if (Vec2.x<Vec1.x && Vec2.y<Vec1.y)
    {
        wynik = M_PI + wynik;
    }

    return wynik;
}

float VECTOR2_ATAN2(VECTOR2 Vec1, VECTOR2 Vec2)
{
    float dY = fabs(Vec2.y - Vec1.y);
    float dX = fabs(Vec2.x - Vec1.x);

    return atan2f(dY, dX);
}

/*  -------------------------------- * SKOCZEK * ---------------------------------- */
void SKOCZEK_SetPosition(SKOCZEK* self, VECTOR2 position)
{
    self->position.x = position.x;
    self->position.y = position.y;
    //puts("ustawia pozycje");
}

void narysujLinie(VECTOR2 pkt1, VECTOR2 pkt2)
{
    al_draw_line(pkt1.x, 720 - pkt1.y, pkt2.x, 720 - pkt2.y, al_map_rgb(0, 0, 0), 3);
}

void narysujPunkt(VECTOR2 punkt, int k1, int k2, int k3)
{
    al_draw_filled_circle(punkt.x, punkt.y, 5, al_map_rgb(k1, k2, k3));
}

void SKOCZEK_Start(SKOCZEK* skoczek)
{
    skoczek->isRiding = 1;
    skoczek->currentPoint = 10;
    skoczek->defaultGravity = 1;

    skoczek->resztaDrogi = 0;
    skoczek->rideVelocity = 5;
    skoczek->radianAngle = 0;
    skoczek->aproximationPoints = 1000;
    skoczek->resztaDrogi = 0.0;
    skoczek->acceleration.x = 0;
    skoczek->acceleration.y = 0;
    skoczek->wysokosc = 125;
    skoczek->szerokosc = 175;
    skoczek->dlugoscNart = skoczek->szerokosc;
    skoczek->jumped = 0;
    skoczek->zjezdzaZGory = 0;
    skoczek->currentPointGory = 0;
    skoczek->aproximationPointsGora = 1000+wydluzenieGory/2;
    skoczek->wynik = 0;
}

void SKOCZEK_Skocz(SKOCZEK* skoczek, float force)
{
    if (skoczek->jumped == 0)
    {
        skoczek->velocity.y = force;
        skoczek->velocity.x = skoczek->rideVelocity;
        skoczek->jumped = 1;
        skoczek->isRiding = 0;
        skoczek->zjezdzaZGory = 0;
        skoczek->rideVelocity = 0;

        puts("skoczyl");
    }
}

void SKOCZEK_DopasujDoSkoczni(SKOCZEK* self, VECTOR2 tablicaPunktow[])
{
    // idzie w lewo i szuka punktu w ktory jest conajmniej tak odlegly od srodka nart co polowa dlugosci tych nart
    if (self->currentPoint == 0)
    {
        puts("za blisko lewej sciany - nie moze dopasowac nart do skoczni bo wejda w sciane");
        return;
    }
    if (self->currentPoint == self->aproximationPoints)
    {
        puts("skoczek wyjezdza ze skoczni - przechodzi na symulacje lotu");
        self->isRiding = 0;
        SKOCZEK_Skocz(self, 1);
        return;
    }

    int szukanyPointLewy = self->currentPoint;
    int szukanyPointPrawy = self->currentPoint;
    while (szukanyPointLewy > 0 && VECTOR2_Odleglosc(tablicaPunktow[self->currentPoint], tablicaPunktow[szukanyPointLewy]) < self->dlugoscNart / 2)
    {
        szukanyPointLewy -= 1;
    }
    while (szukanyPointPrawy < self->aproximationPoints && VECTOR2_Odleglosc(tablicaPunktow[self->currentPoint], tablicaPunktow[szukanyPointPrawy]) < self->dlugoscNart / 2)
    {
        szukanyPointPrawy += 1;
    }
    // ustawia srodek nart skoczka
    VECTOR2 srodekNart;
    if (szukanyPointPrawy >= self->aproximationPoints)
    {
        // przedluza prawy punkt aby nie zwalnial

        //tworzy punkt przedluzenia


        VECTOR2 przedluzonyPunkt;
        przedluzonyPunkt.x = self->position.x + self->dlugoscNart / 2;
        przedluzonyPunkt.y = tablicaPunktow[szukanyPointPrawy].y;

        //narysujPunkt(przedluzonyPunkt, 0, 0, 0);

        srodekNart = VECTOR2_SrodekOdcinka(tablicaPunktow[szukanyPointLewy], przedluzonyPunkt);
    }
    else
    {
        srodekNart = VECTOR2_SrodekOdcinka(tablicaPunktow[szukanyPointLewy], tablicaPunktow[szukanyPointPrawy]);
    }



    self->srodekNart = srodekNart;
    self->position = srodekNart;

    self->radianAngle = VECTOR2_ATAN2(tablicaPunktow[szukanyPointLewy], tablicaPunktow[szukanyPointPrawy]);
}

void SKOCZEK_DopasujDoSkoczniGora(SKOCZEK* self, VECTOR2 tablicaPunktow[])
{
    // idzie w lewo i szuka punktu w ktory jest conajmniej tak odlegly od srodka nart co polowa dlugosci tych nart

    if (self->currentPointGory >= self->aproximationPointsGora)
    {
        puts("skoczek zjechal");
        SKOCZEK_Start(self);
        return;
    }

    int szukanyPointLewy = self->currentPointGory;
    int szukanyPointPrawy = self->currentPointGory;
    while (szukanyPointLewy > 0 && VECTOR2_Odleglosc(tablicaPunktow[self->currentPointGory], tablicaPunktow[szukanyPointLewy]) < self->dlugoscNart / 2)
    {
        szukanyPointLewy -= 1;
    }
    while (szukanyPointPrawy < self->aproximationPoints && VECTOR2_Odleglosc(tablicaPunktow[self->currentPointGory], tablicaPunktow[szukanyPointPrawy]) < self->dlugoscNart / 2)
    {
        szukanyPointPrawy += 1;
    }
    // ustawia srodek nart skoczka
    VECTOR2 srodekNart = VECTOR2_SrodekOdcinka(tablicaPunktow[szukanyPointLewy], tablicaPunktow[szukanyPointPrawy]);

    self->srodekNart = srodekNart;
    self->position = srodekNart;

    self->radianAngle = VECTOR2_ATAN2(tablicaPunktow[szukanyPointLewy], tablicaPunktow[szukanyPointPrawy]);
}

void wypiszNaEkran(int point, ALLEGRO_FONT* font)
{

    

    char c1[] = {'W','y','n','i','k',' ',':',' '};
    char c[10];
    _gcvt(point, 4, c);

    sprintf(c, "%d", point);

    char cW[14];

    for (int i = 0;i < 8;i++)
    {
        cW[i] = c1[i];
    }
    
    cW[8] = c[0];
    cW[9] = c[1];
    cW[10] = c[2];
    cW[11] = c[3];

    al_draw_text(font, al_map_rgb(0, 0, 0), 500, 100, 0, cW);


}

void SKOCZEK_PrzesunOGora(SKOCZEK* skoczek, float s, VECTOR2 tablicaPunktow[])
{
    skoczek->resztaDrogi += s;

    float odleglosc = 0;
    if (skoczek->currentPointGory < skoczek->aproximationPointsGora)
    {
        odleglosc = VECTOR2_Odleglosc(tablicaPunktow[skoczek->currentPointGory], tablicaPunktow[skoczek->currentPointGory + 1]); // przesuniete o 1 w lewo bo indeks tablicy zaczyna sie od 0
    }
    else
    {
        puts("konczy zjazd i od nowa");
        SKOCZEK_Start(skoczek);
        return;
    }

    while (skoczek->currentPointGory < skoczek->aproximationPointsGora && odleglosc <= skoczek->resztaDrogi)
    {
        skoczek->resztaDrogi -= odleglosc;
        skoczek->currentPointGory += 1;
        if ((skoczek->currentPointGory >= skoczek->aproximationPointsGora - 1))
        {
            puts("konczy zjazd w while i od nowa");
            SKOCZEK_Start(skoczek);
            return;
        }
        odleglosc = VECTOR2_Odleglosc(tablicaPunktow[skoczek->currentPointGory], tablicaPunktow[skoczek->currentPointGory + 1]);
    }
    //SKOCZEK_SetPosition(skoczek, tablicaPunktow[skoczek->currentPointGory]);
    SKOCZEK_DopasujDoSkoczniGora(skoczek, tablicaPunktow);
}

void SKOCZEK_PrzesunO(SKOCZEK* skoczek, float s, VECTOR2 tablicaPunktow[])
{

    skoczek->resztaDrogi += s;

    float odleglosc = 0;
    if (skoczek->currentPoint < skoczek->aproximationPoints)
    {
        odleglosc = VECTOR2_Odleglosc(tablicaPunktow[skoczek->currentPoint], tablicaPunktow[skoczek->currentPoint + 1]); // przesuniete o 1 w lewo bo indeks tablicy zaczyna sie od 0
    }
    else
    {
        skoczek->isRiding = 0;
        skoczek->position = skoczek->srodekNart;
        SKOCZEK_Skocz(skoczek, 1);
        puts("konczy jazde w else");
        return;
    }

    while (skoczek->currentPoint < skoczek->aproximationPoints && odleglosc <= skoczek->resztaDrogi)
    {
        skoczek->resztaDrogi -= odleglosc;
        skoczek->currentPoint += 1;
        if (!(skoczek->currentPoint < skoczek->aproximationPoints))
        {
            skoczek->isRiding = 0;
            skoczek->position = skoczek->srodekNart;
            SKOCZEK_Skocz(skoczek, 1);
            puts("konczy jazde w while");
            return;
        }
        odleglosc = VECTOR2_Odleglosc(tablicaPunktow[skoczek->currentPoint], tablicaPunktow[skoczek->currentPoint + 1]);
    }
    //SKOCZEK_SetPosition(skoczek, tablicaPunktow[skoczek->currentPoint]);
    SKOCZEK_DopasujDoSkoczni(skoczek, tablicaPunktow);
}

void ZapisWynikow(SKOCZEK* skoczek) {
    insertNode(&root, skoczek->currentPointGory, skoczek->name);
    FILE* file; //okrema funkcja ktora bedzie zapisywac wyniki po koncu rundy
    if (fopen_s(&file, "WYNIKI.txt", "w") != 0) {
        printf("Nie udalo sie otworzyc pliku WYNIKI.txt");
        return;
    }
    writeToFile(root, file);
    fclose(file);
}

void SKOCZEK_znajdzCurrentPointSkoczni(SKOCZEK* skoczek, VECTOR2 tablicaPunktow[])
{
    skoczek->currentPoint = 0;
    int i = 1;
    while (skoczek->position.x > tablicaPunktow[i].x)
    {
        i += 1;
        skoczek->currentPoint = i;
    }
}

void SKOCZEK_ZnajdzCurrentPointGory(SKOCZEK* skoczek, VECTOR2 tablicaPunktow[])
{
    skoczek->currentPointGory = 0;
    int i = 1;
    while (skoczek->position.x > tablicaPunktow[i].x)
    {
        i += 1;
        skoczek->currentPointGory = i;
    }
    printf("\nCurrentPointGory: %d\n", skoczek->currentPointGory);
    ZapisWynikow(skoczek);
}

void SKOCZEK_SymulujJazde(SKOCZEK* skoczek, float deltaTime, VECTOR2 tablicaPunktow[])
{
    /* oblicza droge */
    float droga = skoczek->rideVelocity * deltaTime;

    /* zapisuje obecny punkt srodka nart skoczka zeby pozniej policzyc roznice wysokosci */
    int prevPointSave = skoczek->currentPoint;


    /* przesuwa skoczka o przebyta droge i dopasowuje go do skoczni */
    SKOCZEK_PrzesunO(skoczek, droga, tablicaPunktow);


    /* dodaje velocity ze zmiany wysokosci skoczka - energia potencjalna zmienia sie w energie kinetyczna */
    skoczek->rideVelocity += sqrt(skoczek->defaultGravity * (fabs(tablicaPunktow[prevPointSave].y - tablicaPunktow[skoczek->currentPoint].y)));
}

void SKOCZEK_SymulujLot(SKOCZEK* skoczek, float deltaTime,float katSkoczka)
{
    skoczek->position.x += skoczek->velocity.x * deltaTime;
    skoczek->position.y += skoczek->velocity.y * deltaTime;

    
    skoczek->acceleration.y -= katSkoczka*5;

    skoczek->velocity.x += skoczek->acceleration.x * deltaTime * 5; // tu bedzie uwzgledniana predkosc wiatru predkosc wiatru nie moze byc wieksza niz grawitacja
    skoczek->velocity.y += skoczek->acceleration.y * deltaTime * 5; // tu bedzie uwzgledniana predkosc wiatru

    skoczek->velocity.y -= skoczek->defaultGravity * deltaTime * 200;

    //printf("\nvelocity.y: %f", skoczek->velocity.y);
    //printf("\nvelocity.x: %f", skoczek->velocity.x);
}

void SKOCZEK_SymulujZjazd(SKOCZEK* skoczek, float deltaTime, VECTOR2 tablicaPunktow[])
{
    /* oblicza droge */
    float droga = skoczek->rideVelocity * deltaTime;

    /* zapisuje obecny punkt srodka nart skoczka zeby pozniej policzyc roznice wysokosci */
    int prevPointSave = skoczek->currentPointGory;


    /* przesuwa skoczka o przebyta droge i dopasowuje go do skoczni */
    SKOCZEK_PrzesunOGora(skoczek, droga, tablicaPunktow);


    /* dodaje velocity ze zmiany wysokosci skoczka - energia potencjalna zmienia sie w energie kinetyczna */
    skoczek->rideVelocity += sqrt(skoczek->defaultGravity * (fabs(tablicaPunktow[prevPointSave].y - tablicaPunktow[skoczek->currentPointGory].y)));

    //printf("\npredkoscSkoczka: %f", skoczek->rideVelocity);
}


/*  -------------------------------- * FUNCTION * ---------------------------------- */

//skala * 1.318116 - ekstremum dolne (x)

float FUNCTION(float x)
{
    return sin(x / skalaSkoczni + M_PI) * skalaSkoczni + skalaSkoczni + x / 4; // sin(x / skala + M_PI) * skala + skala + x / 4 (gdyby ktos szukal skali)
}

VECTOR2* FUNCTION_GetPoints(int x, float dziedzina)
{
    x += 1; // zeby mozna bylo napisac tablica[approximationPoints] i nie wyszlo poza zakres i nie trzeba odejmowac 1 za kazdym razem
    VECTOR2* tablica = (VECTOR2*)malloc(sizeof(VECTOR2) * x);
    for (int i = 0; i < x; i++)
    {
        tablica[i].x = dziedzina / x * i;
        tablica[i].y = FUNCTION(tablica[i].x);
    }
    return tablica;
}

float FUNKCJA_Gory(float x)
{
    //x = x - skalaSkoczni * 1.318116;
    //return -0.7 * (x - skalaSkoczni * 1.318116) + FUNCTION(skalaSkoczni * 1.318116);
    //return (-sqrtf(x / skalaGory) + x / skalaSkoczni / 2) * skalaSkoczni + FUNCTION(skalaSkoczni * 1.318116);
    return (-sin(x / skalaGory)) * skalaGory + skalaSkoczni;
}

VECTOR2* FUNKCJA_Gory_GetPoints(int x, float dziedzina)
{
    VECTOR2* tablica = (VECTOR2*)malloc(sizeof(VECTOR2) * (x+wydluzenieGory));
    float czastka = dziedzina /x;
    int i = 0;
    for (i; i < x; i++)
    {
        tablica[i].x = czastka * i;
        tablica[i].y = FUNKCJA_Gory(tablica[i].x);
    }

    float ekstremumDolne = tablica[i-1].y;

    for (i;i < wydluzenieGory+x;i++)
    {
        tablica[i].x = czastka * i;
        tablica[i].y =ekstremumDolne;
    }

    return tablica;
}

//funkcja, która tworzy i zwraca ciąg znaków reprezentujący imię gracza.
char* stworzyc_imie(ALLEGRO_EVENT_QUEUE* event_queue, ALLEGRO_FONT* font, bool *display_close) {
    // Tworzenie ciągu znaków do wprowadzania imienia
    char* temp = (char*)malloc(50 * sizeof(char));
    if (temp == NULL)
        return NULL;

    int pos = 0;
    char* name = "";
    // Flagi określające elementy, które mają być wyświetlane
    bool name_entered = false;
    bool name_entering = false;
    while (true) {

        // Wyświetlanie elementów w zależności od flag
        al_clear_to_color(al_map_rgb(255, 255, 255));

        al_draw_text(font, al_map_rgb(0, 0, 0), 520, 150, 0, "Wprowadz imie gracza");
        al_draw_rectangle(520, 200, 680, 250, al_map_rgb(0, 0, 0), 2);
        al_draw_text(font, al_map_rgb(0, 0, 0), 530, 210, 0, temp);

        al_draw_rectangle(550, 300, 650, 330, al_map_rgb(0, 0, 0), 2);
        al_draw_text(font, al_map_rgb(0, 0, 0), 580, 310, 0, "Zapisz");

        al_draw_text(font, al_map_rgb(0, 0, 0), 20, 15, 0, "Nowe imie:");

        if (name_entered && !name_entering)
            al_draw_text(font, al_map_rgb(0, 0, 0), 20, 35, 0, name);

        al_draw_rectangle(990, 630, 1135, 660, al_map_rgb(0, 0, 0), 2);
        al_draw_text(font, al_map_rgb(0, 0, 0), 1005, 640, 0, "Powrot do menu");

        al_flip_display();

        // Obsługa zdarzeń klawiatury i myszy
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);
        if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
            // Przetwarzanie wpisu imienia
            if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
                if (pos > 0) {
                    temp[pos - 1] = '\0';
                    pos--;
                }
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                name_entered = true;
                name_entering = false;
            }
            else if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
                if (event.keyboard.unichar >= 32 && event.keyboard.unichar <= 126 && pos < 49) {
                    name_entering = true;
                    temp[pos] = (char)event.keyboard.unichar;
                    pos++;
                    temp[pos] = '\0';
                }
            }
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            *display_close = true;
            break;
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            // Przetwarzanie przycisku "Zapisz"
            if (event.mouse.x >= 550 && event.mouse.x <= 650 && event.mouse.y >= 300 && event.mouse.y <= 330) {
                name_entered = true;
                name_entering = false;
                name = temp;
            }
            // Przetwarzanie przycisku "Powrót do menu"
            else if (event.mouse.x >= 990 && event.mouse.x <= 1135 && event.mouse.y >= 630 && event.mouse.y <= 660) {
                break;
            }
        }
    }
    return name;
}

void rysujRekord(VECTOR2 pkt1,VECTOR2 pkt2,VECTOR2 camera)
{
    //al_draw_line(pkt1.x, 720 - pkt1.y, pkt2.x, 720 - pkt2.y, al_map_rgb(0, 0, 0), 3);

    // 720=displayHeight

    al_draw_line(pkt1.x - camera.x, 720 - pkt1.y - camera.y, pkt2.x - camera.x, 720 - pkt2.y - camera.y, al_map_rgb(255, 255, 0), 5);

}

void Wyniki(ALLEGRO_EVENT_QUEUE* event_queue, ALLEGRO_FONT* font, bool* display_close, SKOCZEK* skoczek) {
    FILE* file;
    if (fopen_s(&file, "WYNIKI.txt", "r") != 0) {
        printf("Nie udalo sie otworzyc pliku WYNIKI.txt");
        return;
    }
    char buffer[100];

    while (true) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);
        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            if (event.mouse.x >= 1000 && event.mouse.x <= 1130 && event.mouse.y >= 35 && event.mouse.y <= 65) {
                break; // powrót do menu
            }
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            *display_close = true;
            break;
        }
        al_clear_to_color(al_map_rgb(255, 255, 255));

        int positionY = 100; // pozycja początkowa dla Y

        rewind(file); // przeniesienie wskaźnika pliku na początek pliku

        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            // Usunięcie znaków nowej linii ('\n') z ciągu
            size_t length = strlen(buffer);
            if (length > 0 && buffer[length - 1] == '\n') 
                buffer[length - 1] = '\0'; // Ustawienie znaku null zamiast znaku nowej linii

            al_draw_text(font, al_map_rgb(0, 0, 0), 100, positionY, 0, buffer);
            positionY += 20; // zwiekszamy pozycje po Y
        }

        al_draw_rectangle(1000, 35, 1130, 65, al_map_rgb(0, 0, 0), 2);
        al_draw_text(font, al_map_rgb(0, 0, 0), 1010, 45, 0, "Powrot do menu");

        al_flip_display();
    }
    fclose(file);
}


// zamienia stopnie w radinay
float radianyZeStopni(float stopnie)
{
    return stopnie * M_PI / 180.0;
}

void narysujSkocznie(VECTOR2 tablicaPunktow[], int points, float displayHeight, VECTOR2 camera)
{
    for (int i = 1; i < points; i++)
    {
        //al_flip_display();
        al_draw_line(tablicaPunktow[i - 1].x - camera.x, displayHeight - tablicaPunktow[i - 1].y - camera.y, tablicaPunktow[i].x - camera.x, displayHeight - tablicaPunktow[i].y - camera.y, al_map_rgb(85, 85, 85), 50);
    }
}

void narysujGore(VECTOR2 tablicaPunktow[], int points, float displayHeight, VECTOR2 camera)
{
    for (int i = 1; i < points+wydluzenieGory; i++)
    {
        //al_flip_display();
        float X = tablicaPunktow[i].x - camera.x;
        float Y = displayHeight - tablicaPunktow[i].y - camera.y;
        al_draw_line(tablicaPunktow[i - 1].x - camera.x, displayHeight - tablicaPunktow[i - 1].y - camera.y, X, Y, al_map_rgb(200, 200, 200), 40);
        //al_draw_line(X,Y,X,Y+720, al_map_rgb(200, 200, 200),5); // za malo fps
    }
}

void SetCamera(VECTOR2* camera, SKOCZEK* skoczek) {
    camera->x = skoczek->position.x - WINDOW_WIDTH / 4; // przesuwa kamerę o ćwierć ekranu w lewo od pozycji narciarza
    camera->y = WINDOW_HEIGHT - skoczek->position.y - 2 * skoczek->wysokosc;
}

bool startGame(ALLEGRO_EVENT_QUEUE* event_queue, ALLEGRO_FONT* font, SKOCZEK* skoczek, bool *display_close) {

    ALLEGRO_BITMAP* playerT = al_load_bitmap("img/Player.png");
    ALLEGRO_BITMAP* strzalka = al_load_bitmap("img/strzalka.png");
    ALLEGRO_BITMAP* playerFly = al_load_bitmap("img/PlayerLeci.png");
    ALLEGRO_SAMPLE* jump_sound = al_load_sample("dzw/jump.wav");
    ALLEGRO_SAMPLE* skocznia_sound = al_load_sample("dzw/skocznia.wav");
    ALLEGRO_SAMPLE* gora_sound = al_load_sample("dzw/gora.mp3");


    ALLEGRO_MOUSE_STATE state;
    float mx, my;

    al_get_mouse_state(&state);
    

    if (!playerT) {
        printf("Nie udalo sie zalodowac image");
        return false;
    }
    else if (!jump_sound && !skocznia_sound) {
        printf("Nie udalo sie zalodowac dzwiekow");
        return false;
    }
    // mnozniki skali zeby mozna bylo ustawic szerokosc i wysokosc w pixelach
    float mnoznikX = skoczek->szerokosc / al_get_bitmap_width(playerT);
    float mnoznikY = skoczek->wysokosc / al_get_bitmap_height(playerT);

    float mnoznikXFly = skoczek->szerokosc / al_get_bitmap_width(playerFly);
    float mnoznikYFly = skoczek->wysokosc / al_get_bitmap_height(playerFly);

    float katStrzalki = 0;

    // zmienne do fizyki
    float dT = 0.016666;

    float szerokoscSkoczni = 10.0;

    float czasZmianyWiatru = 5;// sekund

    int maksymalnaPredkoscWiatru = 5;
    int minimalnaPredkoscWiatru = 1;

    float miernikCzasu = czasZmianyWiatru;

    srand(time(0));

    VECTOR2 Wiatr;
    Wiatr.x = 0;
    Wiatr.y = 0;

    VECTOR2 pozycjaStrzalki;
    pozycjaStrzalki.x = 900;
    pozycjaStrzalki.y = 100;

    float szerokoscGory = skalaGory*1.5707;
    float dziedzinaFunkcji = skalaSkoczni * 1.318116; //skala * 1.318116 - ekstremum dolne (x) 
    int iloscPunktowGory = skoczek->aproximationPointsGora-wydluzenieGory/2; // odejmuje to co dodaje na starcie bo pozniej w funkcji getPoints dodaje cale wydluzenie

    // tworzenie tablicy punktow funkcji skoczni             
    VECTOR2* tablicaPunktow = FUNCTION_GetPoints(skoczek->aproximationPoints, dziedzinaFunkcji);

    VECTOR2* tabelaPunktowGory = FUNKCJA_Gory_GetPoints(iloscPunktowGory,szerokoscGory);

    // tworzenie kamery
    VECTOR2 camera;
    SetCamera(&camera,skoczek);

    VECTOR2 pozycjaSkoczka;
    pozycjaSkoczka.x = skoczek->position.x - camera.x;
    pozycjaSkoczka.y = WINDOW_HEIGHT - skoczek->position.y - camera.y;

    int rekord = drzewo_max(root);

    while (true) {

        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            al_clear_to_color(al_map_rgb(255, 255, 255)); // lepiej zostawic zawsze na poczatku

            miernikCzasu += dT;

            int rekord = drzewo_max(root);

            if (miernikCzasu >= czasZmianyWiatru)
            {
                miernikCzasu = 0;

                float los = rand() % (maksymalnaPredkoscWiatru / 5 * 2 * 10);
                Wiatr.x += (los - maksymalnaPredkoscWiatru / 5 * 10) / 10;

                float losY = rand() % (maksymalnaPredkoscWiatru / 5 * 2 * 10);
                Wiatr.y += (losY - maksymalnaPredkoscWiatru / 5 * 10) / 10;

                if (Wiatr.y < -maksymalnaPredkoscWiatru)
                {
                    Wiatr.y = -maksymalnaPredkoscWiatru;
                }
                else if (Wiatr.y > maksymalnaPredkoscWiatru)
                {
                    Wiatr.y = maksymalnaPredkoscWiatru;
                }

                if (Wiatr.x < -maksymalnaPredkoscWiatru)
                {
                    Wiatr.x = -maksymalnaPredkoscWiatru;
                }
                else if (Wiatr.x > maksymalnaPredkoscWiatru)
                {
                    Wiatr.x = maksymalnaPredkoscWiatru;
                }

                skoczek->acceleration = Wiatr;

                //VECTOR2_Print(&Wiatr);

            }

            // symulacja fizyki co 1/60 sekundy
            if (skoczek->isRiding)
            {
                SKOCZEK_SymulujJazde(skoczek, dT, tablicaPunktow);
                al_play_sample(skocznia_sound, 1.5f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
                SetCamera(&camera, skoczek); // aktualizacja pozycji kamery

            }
            else if (skoczek->zjezdzaZGory)
            {
                //symuluje zjazd z gory
                SKOCZEK_SymulujZjazd(skoczek, dT, tabelaPunktowGory);
                SetCamera(&camera, skoczek); // aktualizacja pozycji kamery
            }
            else // jezeli nie zjezdza ze skoczni ani z gory to znaczy ze leci
            {
                // symuluje lot i sprawdza czy nie spadl na skocznie jezeli tak to ustawia isRiding na lub zjezdzaZGory na 1 i kontynuuje zjazd ze skoczni lub gory
                al_get_mouse_state(&state);
                mx = (float)state.x;
                my = (float)state.y;
                VECTOR2 pozycjaMyszy;
                pozycjaMyszy.x = mx;
                pozycjaMyszy.y = my;

                float katSkoczka = VECTOR2_ATAN2(pozycjaSkoczka, pozycjaMyszy);

                SKOCZEK_SymulujLot(skoczek, dT,katSkoczka);
                SetCamera(&camera, skoczek); // aktualizacja pozycji kamery

                if (skoczek->position.y <= FUNCTION(skoczek->position.x) && skoczek->position.x < dziedzinaFunkcji) // laduje na skoczni
                {
                    skoczek->rideVelocity = skoczek->velocity.x;
                    skoczek->velocity.y = 0;
                    skoczek->velocity.x = 0;
                    skoczek->isRiding = 1;
                    skoczek->zjezdzaZGory = 0;
                    skoczek->jumped = 0;
                    SKOCZEK_znajdzCurrentPointSkoczni(skoczek, tablicaPunktow);
                    puts("wyladowal na skoczni");
                }
                else if (skoczek->position.y <= FUNKCJA_Gory(skoczek->position.x) && skoczek->position.x >= dziedzinaFunkcji) // laduje na gorze
                {
                    skoczek->rideVelocity = skoczek->velocity.x;
                    skoczek->velocity.y = 0;
                    skoczek->velocity.x = 0;
                    skoczek->isRiding = 0;
                    skoczek->zjezdzaZGory = 1;
                    skoczek->jumped = 0;
                    skoczek->resztaDrogi = 0;
                    SKOCZEK_ZnajdzCurrentPointGory(skoczek, tabelaPunktowGory);
                    al_stop_samples();
                    al_play_sample(gora_sound, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
                    puts("wyladowal na gorze");
                    skoczek->wynik = skoczek->currentPointGory;
                    //wypiszNaEkran(skoczek->currentPointGory,font);
                }
            }
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            // przetwarzanie klawiatury dla pierwszego gracza
            if (event.keyboard.keycode == ALLEGRO_KEY_SPACE && !skoczek->zjezdzaZGory)
            {
                SKOCZEK_Skocz(skoczek, 100.0);
                al_stop_samples();
                al_play_sample(jump_sound, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL); //poprawic
            }
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            *display_close = true;
            break;
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            if (event.mouse.x >= 1000 && event.mouse.x <= 1130 && event.mouse.y >= 35 && event.mouse.y <= 65) {
                break; // powrot do menu
            }
        }

        //rysuje skoczka i obraca go i pozycjonuje w wys-position.y pozycji bo y jest odwrocone...
        narysujSkocznie(tablicaPunktow, skoczek->aproximationPoints, WINDOW_HEIGHT, camera);
        narysujGore(tabelaPunktowGory, iloscPunktowGory, WINDOW_HEIGHT, camera);

        if (skoczek->isRiding || skoczek->zjezdzaZGory)
        {
            al_draw_scaled_rotated_bitmap(playerT, skoczek->szerokosc, skoczek->wysokosc, skoczek->position.x - camera.x, WINDOW_HEIGHT - skoczek->position.y - camera.y, mnoznikX, mnoznikY, skoczek->radianAngle, 0);
        }
        else
        {
            mx = (float)state.x;
            my = (float)state.y;

            VECTOR2 pozycjaMyszy;
            pozycjaMyszy.x = mx;
            pozycjaMyszy.y = my;

            float katSkoczka = VECTOR2_ATAN2Strzalka(pozycjaSkoczka, pozycjaMyszy);

            al_draw_scaled_rotated_bitmap(playerFly, 175/2, 52, skoczek->position.x - camera.x, WINDOW_HEIGHT - skoczek->position.y - camera.y, 1, 1, katSkoczka, 0);
        }

        
        
        // rysuje strzalke wiatru
        VECTOR2 przedluzonyWiatr;
        przedluzonyWiatr.x = Wiatr.x * 20;
        przedluzonyWiatr.y = Wiatr.y * 20;
        VECTOR2 koniecStrzalki;
        koniecStrzalki.x = pozycjaStrzalki.x + przedluzonyWiatr.x*2;
        koniecStrzalki.y = pozycjaStrzalki.y - przedluzonyWiatr.y*2;

        katStrzalki = VECTOR2_ATAN2Strzalka(pozycjaStrzalki, koniecStrzalki);
        
        //narysujPunkt(koniecStrzalki,0,0,0);

        al_draw_scaled_rotated_bitmap(strzalka, 25, 25, pozycjaStrzalki.x, pozycjaStrzalki.y, 1, 1, katStrzalki, 0);

        
        // zamienia wiatr float na string
        float wiatrMag = VECTOR2_DlugoscVectora(Wiatr);
        char c[10];
        _gcvt(wiatrMag, 4, c);
        c[4] = 'm';
        c[5]='/';
        c[6] = 's';
        c[7] = '\0';
        al_draw_text(font, al_map_rgb(0, 0, 0), pozycjaStrzalki.x-12.5, pozycjaStrzalki.y+30, 0, c);

        // rysuje wynik

        /*VECTOR2 pktNad;
        pktNad.x = rekord;
        pktNad.y = tabelaPunktowGory[rekord].y;
        VECTOR2 pktPod;
        pktPod.x = rekord;
        pktPod.y = tabelaPunktowGory[rekord].y + 10;*/


        VECTOR2 pktNad;
        pktNad.x = tabelaPunktowGory[rekord].x;
        pktNad.y = tabelaPunktowGory[rekord].y + 25;

        VECTOR2 pktPod;
        pktPod.x = tabelaPunktowGory[rekord].x;
        pktPod.y = tabelaPunktowGory[rekord].y;

        rysujRekord(pktPod, pktNad, camera);

        //printf("%d\n", skoczek->currentPointGory);
        //printf("%d\n", rekord);

        if (skoczek->zjezdzaZGory)
        {
            wypiszNaEkran(skoczek->wynik,font);
            //printf("%d", rekord);
        }

        al_draw_rectangle(1000, 35, 1130, 65, al_map_rgb(0, 0, 0), 2);
        al_draw_text(font, al_map_rgb(0, 0, 0), 1010, 45, 0, "Powrot do menu");

        al_flip_display();
    }
    al_destroy_bitmap(playerT);
    al_destroy_sample(jump_sound);
    al_destroy_sample(skocznia_sound);
    al_destroy_sample(gora_sound);
    return true;
}

void menu(ALLEGRO_EVENT_QUEUE* event_queue, ALLEGRO_FONT* font, SKOCZEK* skoczek) {
    //FLAGI:
    bool name_entered = false;
    bool gameOver = false;
    bool display_close = false;

    //Pętla menu
    while (!gameOver) {

        al_clear_to_color(al_map_rgb(255, 255, 255));
        //Name
        al_draw_text(font, al_map_rgb(0, 0, 0), 20, 18, 0, "Your name: ");
        al_draw_text(font, al_map_rgb(0, 0, 0), 120, 18, 0, skoczek->name);
        // Przycisk "Ustawic imie"
        al_draw_rectangle(20, 45, 135, 75, al_map_rgb(0, 0, 0), 2);
        al_draw_text(font, al_map_rgb(0, 0, 0), 30, 55, 0, "Ustawic imie");
        // Przycisk "Start"
        al_draw_rectangle(550, 300, 650, 330, al_map_rgb(0, 0, 0), 2);
        al_draw_text(font, al_map_rgb(0, 0, 0), 570, 310, 0, "Start");
        // Przycisk "Wyniki"
        al_draw_rectangle(550, 340, 650, 370, al_map_rgb(0, 0, 0), 2);
        al_draw_text(font, al_map_rgb(0, 0, 0), 570, 350, 0, "Wyniki");
        // Przycisk "Wyjście"
        al_draw_rectangle(550, 380, 650, 410, al_map_rgb(0, 0, 0), 2);
        al_draw_text(font, al_map_rgb(0, 0, 0), 570, 390, 0, "Wyjscie");
        al_flip_display();

        // Obsługa zdarzeń myszy
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);
        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            //Przycisk "Ustawic imie"
            if (event.mouse.x >= 20 && event.mouse.x <= 135 && event.mouse.y >= 45 && event.mouse.y <= 75) {
                char* temp = skoczek->name;
                skoczek->name = stworzyc_imie(event_queue, font, &display_close);
                if (strcmp(skoczek->name, "") == 0) {
                    skoczek->name = temp;
                    name_entered = false;
                }
                else
                    name_entered = true;
                if (display_close)
                    gameOver = true;
            }
            // Przycisk "Start"
            else if (event.mouse.x >= 550 && event.mouse.x <= 650 && event.mouse.y >= 300 && event.mouse.y <= 330) {
                printf("\nreszta drogi: %f\n", skoczek->resztaDrogi);
                if (!startGame(event_queue, font, skoczek, &display_close))
                    gameOver = true;
                if (display_close)
                    gameOver = true;
            }
            // Przycisk "Wyniki"
            else if (event.mouse.x >= 550 && event.mouse.x <= 650 && event.mouse.y >= 340 && event.mouse.y <= 370) {
                Wyniki(event_queue, font, &display_close, skoczek);
                if (display_close)
                    gameOver = true;
            }
            // Przycisk "Wyjście"
            else if (event.mouse.x >= 550 && event.mouse.x <= 650 && event.mouse.y >= 380 && event.mouse.y <= 410) {
                gameOver = true;
            }
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            gameOver = true;
        }

    }
    if (name_entered)
        free(skoczek->name);
}

int main(void)
{
    al_init();
    al_init_primitives_addon();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(1);

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    al_start_timer(timer);

    ALLEGRO_DISPLAY* display = al_create_display(WINDOW_WIDTH, WINDOW_HEIGHT);
    al_set_window_title(display, "Skoki");

    // Tworzenie czcionki do wyświetlania tekstu
    ALLEGRO_FONT* font = al_create_builtin_font();

    if (!display)
        return -1;

    ALLEGRO_BITMAP* cursorBitmap = al_load_bitmap("kursor/kursor.png");
    if (!cursorBitmap) {
        puts("Nie udalo sie zaladowac kursor!");
        return -1;
    }
    ALLEGRO_MOUSE_CURSOR* cursor = al_create_mouse_cursor(cursorBitmap, 0, 0);
    al_set_mouse_cursor(display, cursor);

    // Tworzenie kolejki zdarzeń
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    readFromFile(&root, "WYNIKI.txt"); //wczytujemy wyniki do drzewa

    SKOCZEK skoczek;
    skoczek.isRiding = 1;
    skoczek.currentPoint = 10;
    skoczek.defaultGravity = 1;

    skoczek.resztaDrogi = 0;
    skoczek.rideVelocity = 5;
    skoczek.radianAngle = 0;
    skoczek.aproximationPoints = 1000;
    skoczek.resztaDrogi = 0.0;
    skoczek.acceleration.x = 0;
    skoczek.acceleration.y = 0;
    skoczek.wysokosc = 125;
    skoczek.szerokosc = 175;
    skoczek.name = "Player";
    skoczek.dlugoscNart = skoczek.szerokosc; // wczesniej bylo 2
    skoczek.jumped = 0;
    skoczek.zjezdzaZGory = 0;
    skoczek.currentPointGory = 0;
    skoczek.aproximationPointsGora = 1000+wydluzenieGory/2;

    menu(event_queue, font, &skoczek);

    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_bitmap(cursorBitmap);
    al_destroy_mouse_cursor(cursor);
    removeNode(root); //dealokujemy drzewo
    return 0;
}
