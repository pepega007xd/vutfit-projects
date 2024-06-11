## 16. Banka
Navrhněte modul jednoduchého informačního systému banky, který bude umožňovat správu bankovních účtů. V systému jsou evidovány základní údaje o zaměstnancích banky i jejich klientech, kteří mají minimálně jeden účet u banky nebo jsou disponenty s nějakým účtem. Systém uchovává informace o různých operacích nad účty. Předpokládejte, že každý účet má pouze jednoho vlastníka, ale může mít více disponentů, kteří mají omezená práva u daného účtu. K účtu mohou být klientům nebo disponentům vystaveny platební karty a zřízeno internetové bankovnictví. V rámci jednoho internetového bankovnictví klient spravuje všechny své účty a všechny účty, ke kterým má dispoziční právo.
Banka nabízí více typů účtu, přičemž každý typ účtu má dostupné různé služby, které zákazník může nebo nemusí mít aktivovány. Klienti bance zadávají požadavky prostřednictvím zaměstnanců na pobočce, kartou v platebních terminálech, nebo přes internetové bankovnictví. Základní operace nad účty jsou vklad, výběr z účtu, převod mezi účty, nebo platba kartou. Pokud se jedná o převod v rámci dané banky, transakce je dohledatelná z obou účtů. U každé transakce je možné zpětně dohledat, který klient o operaci zažádal, kdy byla provedena a, v případě zadání operace na pobočce, kterým zaměstnancem byla operace provedena. Banka může mít více poboček, u zaměstnanců je možné zjistit na které pobočce pracují a jaké účty jím byly na dané pobočce zřízeny.



### ER:
* ~~V systému jsou evidovány základní údaje o zaměstnancích banky i jejich klientech, kteří mají minimálně jeden účet u banky nebo jsou disponenty s nějakým účtem.~~
* ~~Předpokládejte, že každý účet má pouze jednoho vlastníka, ale může mít více disponentů, kteří mají omezená práva u daného účtu.~~
* ~~K účtu mohou být klientům nebo disponentům vystaveny platební karty a zřízeno internetové bankovnictví.~~
* ~~Banka nabízí více typů účtu, přičemž každý typ účtu má dostupné různé služby, které zákazník může nebo nemusí mít aktivovány.~~
* ~~Klienti bance zadávají požadavky prostřednictvím zaměstnanců na pobočce, kartou v platebních terminálech, nebo přes internetové bankovnictví.~~
* ~~Systém uchovává informace o různých operacích nad účty.~~
* ~~Základní operace nad účty jsou vklad, výběr z účtu, převod mezi účty, nebo platba kartou.~~
* ~~Pokud se jedná o převod v rámci dané banky, transakce je dohledatelná z obou účtů.~~
* ~~U každé transakce je možné zpětně dohledat, který klient o operaci zažádal, kdy byla provedena a, v případě zadání operace na pobočce, kterým zaměstnancem byla operace provedena.~~
* ~~Banka může mít více poboček, u zaměstnanců je možné zjistit na které pobočce pracují a jaké účty jím byly na dané pobočce zřízeny.~~
* TODO: Add cardinalities to relations