Laptimer
========

Řada sportovních aktivit, jako například automobilové nebo motocyklové
závody, se odehrává na nějakém okruhu. V cíli jde samozřejmě o to, kdo
dojede první, ale v průběhu závodu je důležité sledovat časy na
jednotlivá kola. Cílem úlohy je tedy naprogramovat jednoduché stopky pro
okruhové aktivity.

Okruhové stopky měří a ukazují čas v rámci jednoho kola, nikoliv celkový
čas jako běžné stopky. Stisk mezičasového tlačítka indikuje dokončení
jednoho kola, čímž dojde k jeho zaznamenání a vynulování aktuálního času
na kolo (bez přerušení měření času). Zaznamenaný čas zůstane po nějakou
dobu (např. 1 sekundu) na displeji (aby bylo možné ho v klidu odečíst) a
po uplynutí této doby začne displej opět ukazovat aktuální měřený čas.

Stopky budou mít 4 paměťové sloty na časy, jejichž obsazenost bude
indikována pomocí 4 LED. Při větším počtu zaznamenaných časů bude vždy
nejstarší čas zahozen. Spolu s časem závěrečného kola si tedy stopky
budou pamatovat posledních 5 časů na kolo.

Čas na kolo je zobrazován na 7-segmentovém displeji ve formátu `SSS.D`,
kde `SSS` představuje celé sekundy a `D` desetiny sekund.

## Základní funkce a ovládání

-   Tlačíko B1 spustí/zastaví měření času.
-   Pokud stopky běží:
    -   Stisk B2 zaznamená čas na kolo.
    -   Stisk B3 vyresetuje aktuální čas na kolo, ale zachová
        zaznamenané časy.
-   Po zastavení stopky zobrazují aktuální čas na kolo a LED diody
    indikují využité paměťové sloty.
-   Pokud stopky stojí:
    -   Stisk B2 přepíná mezi zaznamenanými časy na kolo a aktuálním
        (zastaveným časem). Prázdné sloty se nezobrazují.
    -   Pokud je zobrazován čas z paměťového slotu, příslušná LED svítí,
        pokud je zobrazován aktuální čas, LED indikují obsazenost slotů.
    -   Stisk B3 vyresetuje aktuální čas a všechny zaznamenané časy.

## Bonusové funkce

-   Pokud stopky stojí, při prohlížení paměťových slotů (pomocí B2)
    bliká LED odpovídající právě zobrazovanému slotu zatímco ostatní LED
    indikují obsazenost slotu. Např. pokud byly zaznamenány 3 časy, tak
    při zobrazení času ze slotu 2 budou LED1 a LED3 svítit, LED2 bude
    blikat, a LED4 bude zhasnutá.

-   Pokud stopky stojí, dlouhý stisk B2 zobrazí průměr všech časů na
    kolo (včetně těch, které už si stopky nepamatují) a všechny LED
    budou blikat.

-   U tlačítka B3 se bude rozlišovat krátký a dlouhý stisk.

    -   Krátký stisk vyresetuje aktuální čas, ale zachová zaznamenané
        časy (bez ohledu na to, zda stopky stojí nebo běží).
    -   Dlouhý stisk vyresetuje aktuální čas a všechny zaznamenané časy
        (bez ohledu na to, zda stopky stojí nebo běží).
