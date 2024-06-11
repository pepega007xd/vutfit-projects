Implementace ICP projektu - Pacman
- Autor: Tomáš Brablec
- Licence: GPL 3.0 (https://www.gnu.org/licenses/gpl-3.0.txt)

Ovládání:
- Pohyb Pacmana se ovládá klávesami W (nahoru), A (vlevo), S (dolů), D (vpravo).
- Hra se spustí mezerníkem, po skončení hry se dá restartovat klávesou R.
- Při přehrávání lze krokovat klávesou N (next), a krok zpět se udělá klávesou B (back).

Návrh aplikace:

Aplikace je rozdělena do dvou hlavních tříd a dlších pomocných struktur.

- třída MainWindow (src/main_window.cpp)
  Zajišťuje funkčnost samotného grafického okna, tlačítka v UI mají nastavené callbacky
  přímo na metody MainWindow. Třída je zodpovědná za vytvoření instance třídy Game, její
  obsluhu a odstranění. Konstruktor rovnou načte výchozí mapu.

- třída Game (src/game.cpp) 
  Reprezentuje jednu konkrétní hru, má přístup ke canvasu, do kterého renderuje obsah.
  Třída MainWindow obsahuje vždy jen jednu instanci třídy Game (singleton).

- třída GameMap (src/game_map.cpp) 
  Pomocná třída, obsahuje datovou strukturu herní mapy, a logiku pro načítání mapy
  z textové reprezenatace.

- sada pomocných funkcí (src/utils.cpp)
  Abych zabránil hromadění kódu v jedné třídě, vyčlenil jsem jednotlivé funkce a typy
  nevyžadující přístup k datům tříd do samostatného modulu, většinou jde o typy
  reprezentující nějakou pozici, stav, nebo směr, a funkce pro jejich úpravu
  (např. posun bodu pomocí směru).

Rozdíly implementace vůči zadání:

- V současné verzi bohužel není možné zvolit výchozí zobrazení "od konce" při přehrávání 
  záznamu, ani záznam plynule přehrávat pozpátku. Možné je pouze krokování zpět.
