#import "template.typ": *

#show: project.with(
  title: "IMS Project Report",
  authors: (
    "Tomáš Brablec (xbrabl04)",
    "Pavel Štarha (xstarh04)",
  ),
)

= Introduction
// Úvod musí vysvětlit, proč se celá práce dělá a proč má uživatel výsledků váš dokument číst (prosím, projekt sice děláte pro získání zápočtu v IMS, ale mohou existovat i jiné důvody). Případně, co se při čtení dozví.

This work attempts to create a simulation that is able to find the optimal policy for scheduling trains. Specifically, the goal is to find a policy for delaying trains to allow passengers to transfer from a delayed train to another. This is a problem without a simple solution since the optimal policy will depend on the specific conditions of the analyzed connections. The resulting simulation model can be configured for a range of situations.

== Authors
// Kdo se na práci podílel jako autor, odborný konzultant, dodavatel odborných faktů, význačné zdroje literatury/fakt, ... 

This work was created by the project authors, based on studying route plans in @route_plans_jmk and @route_plans_cd. Other sources include information about connecting train policies in @connecting_trains_cd and @connecting_trains_cd_2, and timetable creation policies from @timetable_creation_cd. Statistics of train delays are extracted from @delays_cd.

== Validity of the Model
// V jakém prostředí a za jakých podmínek probíhalo experimentální ověřování validity modelu – pokud čtenář/zadavatel vaší zprávy neuvěří ve validitu vašeho modelu, obvykle vaši práci odmítne už v tomto okamžiku. 

Model configurations used in simulations use real data gathered from sources described above. Additionally, validity of the model is checked firstly by simulating simple scenarios and checking that the results align with both our expectations and real-world data (see @experiments). Secondly, a real-world track is simulated (see @real_world_track) and checked that the results of the simulation align with data from @delays_cd.

= Problem Analysis <problem_analysis>
// Shrnutí všech podstatných faktů, které se týkají zkoumaného systému (co možná nejvěcnějším a technickým (ideálně formálně matematickým) přístupem, žádné literární příběhy). Podstatná fakta o systému musí být zdůvodněna a podepřena důvěryhodným zdrojem (vědecký článek, kniha, osobní měření a zjišťování). Pokud budete tvrdit, že ovce na pastvě sežere dvě kila trávy za den, musí existovat jiný (důvěryhodný) zdroj, který to potvrdí. Toto shrnutí určuje míru důvěryhodnosti vašeho modelu (nikdo nechce výsledky z nedůvěryhodného modelu). Pokud nebudou uvedeny zdroje faktů o vašem systému, hrozí ztráta bodů.

In our research we decided to focus on policies of waiting for delayed trains used by the Czech Railways. The current policy defines multiple tiers of trains @timetable_creation_cd, where only a train of lower tier will wait on a train of a higher tier, not the other way around. 

The current policy states that the train will wait only for such a time, so that it will leave the station with its own delay no greater than 5 minutes, as stated in @connecting_trains_cd_2. This means that a train which already accumulated a delay of 5 minutes will not wait for another train in any situation. Note that this is not equivalent to the policy of simply waiting the maximum of 5 minutes in each station, because this would allow for accumulation of delay greater than 5 minutes over multiple stations.

Our goal is to try out the behavior of similar policies on a simple model and to check whether we can find one that performs as well or better than the current one.

== Our Approach
// Popis použitých postupů pro vytvoření modelu a zdůvodnění, proč jsou pro zadaný problém vhodné. Zdůvodnění může být podpořeno ukázáním alternativního přístupu a srovnáním s tím vaším. Čtenář musí mít jistotu, že zvolené nástroje/postupy/technologie jsou ideální pro řešení zadaného problému (ovšem, "dělám to v Javě, protože momentálně Java frčí..." nemusí zadavatele studie uspokojit). 

Our conceptual model is represented using a Petri Net with some informal notation added on top. This is done to simplify the model when expressing more complicated relationships. Other models of railway systems, such as the one in @railway_petri_net, employ extensions of Petri Nets, such as their colored or fuzzy variants.

Our simulation model is implemented in C++ using the SIMLIB @simlib simulation library. The choice of this library is based on familiarity with its API (explained in @ims_slajdy) and its speed. Our simulation experiments often required millions of model time units to produce clean results, which would be hard to achieve in a language with more runtime overhead than C++.

== Origin of our Method
// Popis původu použitých metod/technologií (odkud byly získány (odkazy), zda-li jsou vytvořeny autorem) - převzaté části dokumentovat (specificky, pokud k nim přísluší nějaké autorské oprávnění/licence). Zdůvodnit potřebu vytvoření vlastních metod/nástrojů/algoritmů. Ve většině případů budete přebírat již navržené metody/algoritmy/definice/nástroje a je to pro školní projekt typické. Stejně tak je typické, že studenti chybně vymýšlí již hotové věci a dojdou k naprostému nesmyslu - je třeba toto nebezpečí eleminovat v tomto zdůvodnění.

Initially, we planned to create a full model of a railway system comparable to @railway_petri_net. However, the paper focuses on scheduling railway traffic in general, with respect to safety and liveness properties. This is not the focus of our work, therefore we decided to omit most of the details while still satisfying the basic rules of railway traffic outlined in @railway_petri_net. Mainly that each track section can be occupied by at most one train at a time. Exactly how this is satisfied in our model is described in @conceptual_model.

= Conceptual Model <conceptual_model>
// Konceptuální model je abstrakce reality a redukce reality na soubor relevantních faktů pro sestavení simulačního modelu. Předpokládáme, že model bude obsahovat fakta z "Rozboru tématu". Pokud jsou některá vyřazena nebo modifikována, je nuto to zde zdůvodnit (například: zkoumaný subjekt vykazuje v jednom procentu případů toto chování, ovšem pro potřeby modelu je to naprosto marginální a smíme to zanedbat, neboť ...). Pokud některé partie reality zanedbáváte nebo zjednodušujete, musí to být zdůvodněno a v ideálním případě musí být prokázáno, že to neovlivní validitu modelu. Cílem konceptuálního (abstraktního) modelu je formalizovat relevantní fakta o modelovaném systému a jejich vazby. Podle koncept. modelu by měl být každý schopen sestavit simulační model.

Our conceptual model describes a railway track with a series of stations. This track (from now referred to as the _main track_) is served by a single train (referred to as the _connecting train_). There are two types of stations. In the first type, only _direct passengers_ board the train. The connecting train rides from one end station to another and back in a loop.

Direct passenger comes to a station at the moment of scheduled arrival of the connecting train, boards the train, and then leaves the train at another station (their destination). Their travel time is counted from the scheduled (not actual) arrival of the connecting train to the moment of arriving into their destination.

The second type of station is a _transfer station_, where, in addition to direct passengers, _independent trains_ finish their journey. The movement of these trains themselves is not simulated since it does not affect the resulting data, the only information about independent trains is their delay at the moment of arrival to the transfer station. The scheduled (not actual) arrival of an independent train is synchronized to the arrival of the connecting train. Therefore, if the independent train arrives without a delay, all the passengers of the independent train will be able to transfer to the connecting train.

The independent train carries _transfer passengers_, which arrive to the transfer station in a (possibly delayed) independent train, then wait for the soonest connecting train heading in the direction of their destination station, board the train and continue the same way as direct passengers. Passengers arriving in the independent train to the transfer station and then not continuing in the connecting train are not simulated, since their total time of travel is not affected by the policy of waiting on delayed trains. 

The destinations of both direct and transfer passengers are generated randomly along the main track. The only rule that applies is that the destination station is different from the origin station (transfer station for transfer passengers).

As described in @problem_analysis, only the connecting train can wait for delayed independent train (hence the naming), not the other way around. Therefore, the delay of independent train can be generated independently of any other parameter based on a distribution extracted from real data.

In our model, a track section mentioned in @railway_petri_net is simply the section of a track between two neighboring stations. Therefore, the safety property is satisfied automatically by simulating only one train on the main track, and ensuring that the delay of the independent trains does not exceed its period of repetition. That would mean that one train caught up with another on a single track.

== Model Description
// Způsob vyjádření konceptuálního modelu musí být zdůvodněn (na obrázku xxx je uvedeno schéma systému, v rovnicích xx-yy jsou popsány vazby mezi ..., způsob synchronizace procesů je na obrázku xxx s Petriho sítí).

#let t_next = $T_"next"$
#let t_mul = $T_"mul"$

@track_schema displays a schematic diagram of an example configuration of our model. Number and properties of stations and independent trains is fully configurable in our simulation model, so that we can simulate different situations using a single model. The parameter $P$ at each station describes the number of direct passengers departing from the station each time the connecting train is scheduled to arrive. #t_next is the time of ride into the next station in minutes. Note that #t_next of the last station is zero, since there is no next station.

Independent trains are marked with a dot, their parameter $P$ determines the number of transfer passengers travelling in the train. Parameter $D$ represents the delay of the train and #t_mul represents the number of connection train periods it takes for this train to arrive. The _connection train period_ $T_c$ is time it takes the connection train to do one full cycle over the track, from the starting station to the opposite station and back. The value $#t_mul = 4$ means that the independent train will arrive every 6 hours (connection train period is 90 minutes in @track_schema).

@petri_full shows a Petri Net model of a single transfer station with an independent train. Notice that the _maximum wait time_ $T_w$ is reduced by the current _connecting train delay_ $T_d$. This means that if the current delay is equal to the wait time, the connecting train will not wait for any other train. This models the property described in @problem_analysis.

@petri_half is a Petri Net model of a simple station (without an independent train). It shows that the train will not spend any time except the _boarding time_ in the station since there is no other train to wait for.

== Model Diagrams
// Formy konceptuálního modelu (důraz je kladen na srozumitelnost sdělení). Podle potřeby uveďte konkrétní relevantní:
// - obrázek/náčrt/schéma/mapa (možno čitelně rukou) 
// - ...

#figure(
  image("model_diagram.png"),
  caption: [Schematic model of an example track with four stations]
) <track_schema>

#figure(
  image("petri_full.png"),
  caption: [Petri Net model of a transfer station with an independent train and direct passengers]
) <petri_full>

#figure(
  image("petri_half.png"),
  caption: [Petri net model of a simple station with direct passengers]
) <petri_half>

= Simulation Model
// Tato kapitola má různou důležitost pro různé typy zadání. U implementačních témat lze tady očekávat jádro dokumentace. Zde můžete využít zajímavého prvku ve vašem simulačním modelu a tady ho "prodat".

The simulation model is implemented generically with respect to the number and properties of stations. The model time (variable `Time` in SIMLIB) is counted in minutes.

=== Station

 Each station is represented by the following structure (`station.h`):

```cpp
struct Station {
  Facility is_arriving;
  Queue waiting_passengers_left;
  Queue waiting_passengers_right;
  
  bool has_independent_train;
  
  IntGenerator transfer_passengers;
  IntGenerator direct_passengers;
  
  int repeat_period_multiplier; // T_mul
  int time_to_next_station;
  DoubleGenerator get_delay;
}
```

The types `IntGenerator` and `DoubleGenerator` represent functions that return `int` and `double` respectively, and they are used to represent distributions, such as `Uniform` with given parameters, or custom distribution.

Facility `is_arriving` is seized by the process `IndependentTrain` (`independent_train.h`) at the moment of scheduled arrival to the transfer station and is released at the moment of actual (delayed) arrival. The facility is checked by the connecting train at the moment of arrival into a station, and if it is seized, the train begins the wait.

The queues `waiting_passengers_left` and `waiting_passengers_right` are used by both types of passengers to wait till the arrival of the connecting train. The direction is determined automatically according to the destination station (passengers will not board the connecting train and ride it to the end station and back just to get to a station one place over).

The parameter `has_independent_train` determines the type of station. For code simplicity, both types of stations are represented using the same structure.

Functions `transfer_passengers` and `direct_passengers` return the number of transfer and direct passengers, respectively. `get_delay` returns the delay of the independent train when called.

=== Independent Train

The process `IndependentTrain` is implemented in `independent_train.cpp` and behaves in the following way. When first activated, it activates all its passengers so that they start counting their travel time. Then the train waits for the duration of its delay and activates its passengers again. This represents the arrival into the transfer station.

=== Connecting Train

The connecting train process (`connecting_train.cpp`) runs in a loop -- a single instance is spawned at the start of the simulation and runs until the end. It cycles across the station list from start to end and back, in each station it first activates all leaving passengers, which informs them that they arrived to their destination, then boards all passengers waiting in the stations (from the queue corresponding to the current direction of travel). Finally, it checks whether there is an independent train arriving with delay, and waits for it according to the rules specified in @conceptual_model. Then it continues to the next station. 

When the train travels between stations with a delay, it can shorten the delay by the time it would normally spend waiting in the station. It is a common practice for a train to stop in larger stations for more time than is needed for the passengers to board the train. This time can then be used to shorten the delay. This time is constant for all stations (constant `MAX_BOARDING_TIME` in `params.h`). This constant can be derived from real data as the average of boarding times over all stations (for example, for the train Os 4758 it is approximately 1.2 minutes per station, see @route_plans_jmk).

=== Passengers

There is a single `Passenger` process for both direct and transfer passengers, which performs only three simple actions. When first activated, it logs the time of activation, then passivates itself. This represents waiting for arrival into their transfer station. Direct passengers are reactivated immediately. Then the process enqueues itself into the appropriate direction queue in the transfer station and waits to be dequeued by the connecting train. After being dequeued, it passivates itself again and waits for the activation by the `ConnectingTrain` process at the moment of arrival into its destination.

=== Parameters

Parameters of the simulation are stored in the file `params.h`. However, due to code structure, the table `stations` which contains the configuration of each station is located in `station.cpp`.

== Correspondence to the Conceptual Model
// Minimálně je nutno ukázat mapování abstraktního (koncept.) modelu do simulačního (resp. simulátoru). Např. které třídy odpovídají kterým procesům/veličinám a podobně. 

There is a direct correspondence between the processes `IndependentTrain` and `ConnectingTrain` in the simulation model and in the conceptual model. The processes of the simulation model behave as it is described in @conceptual_model. The parameters in `params.h` also directly correspond to parameters described in @conceptual_model.

= Simulation Experiments

== Our Approach to Experiments
// Naznačit postup experimentování – jakým způsobem hodláte prostřednictvím experimentů dojít ke svému cíli (v některých situacích je přípustné "to zkoušet tak dlouho až to vyjde", ale i ty musí mít nějaký organizovaný postup). 

The purpose of the experiments is to evaluate which maximum wait time ($T_w$) is optimal for most passengers. To evaluate this, we run simulations with different values of $T_w$ and collect travel times (time from first activation of a passenger to the reaching of final destination). This is done using the `Histogram` class in SIMLIB (`travel_time` in `utils.cpp`), which gives us the average value of the travel times. The lower this value is, the less time passengers spend travelling, therefore we look for the lowest possible average travel time.

// Experimentování probíhalo, že jsme našli konkrétní trať jednoho vlaku. Ze stránky Idos a https://kam.mff.cuni.cz/ byla vytažena data ukazující jeho zpoždění v závislosti na zpoždění přípojných vlaků, dále doba jízdy do následujících stanic, dle velikosti stanice se odhadl i počet cestujících. Odsimulováno bylo několik jak reálných, tak smyšlených tras s parametry, kde i laik dokáže určit ideální dobu čekání (zpoždění) vlaku. 

The common approach to the experiments was that we chose a track of a single train, extracted information about the route (travel time between stations, average delays) from @route_plans_cd and @delays_cd, information about independent trains and their delays, and approximated the number of passengers based on the size of the stations. Then we fed this data to the model and evaluated different values of maximum wait time, and plotted the results. In addition to real tracks, we also experimented with fictional tracks with hand-picked parameters, on which we checked that the model behaves as we would expect in certain situations.

== Experiments <experiments>
// Dokumentace jednotlivých experimentů - souhrn vstupních podmínek a podmínek běhu simulace, komentovaný výpis výsledků, závěr experimentu a plán pro další experiment (např. v experimentu 341. jsem nastavil vstup x na hodnotu X, která je typická pro ... a vstup y na Y, protože chci zjistit chování systému v prostředi ... Po skončení běhu simulace byly získány tyto výsledky ..., kde je nejzajímavější hodnota sledovaných veličin a,b,c které se chovaly podle předpokladu a veličin d,e,f které ne. Lze z toho usoudit, že v modelu není správně implementováno chování v podmínkách ... a proto v následujících experimentech budu vycházet z modifikovaného modelu verze ... Nebo výsledky ukazují, že systém v těchto podmínkách vykazuje značnou citlivost na parametr x ... a proto bude dobré v dalších experimentech přesně prověřit chování systému na parametr x v intervalu hodnot ... až ...) 

=== No independent trains

// Experiment č.1 byl v situaci, kde jsme na trať umístili několik zastávek, aniž by se na kterékoliv z nich čekalo na přípojný vlak. Když jsme uměle zvýšili zpoždění vlaku, nárůst průměrné doby jízdy pro všechny cestující byl přímo úměrný době zpoždění. Konfigurace byla upravena do stavu, kde jsme vlaku uměle zvýšili zpoždění. Vytvořili jsme 3 stanice, kde v každé nastupovalo 25 lidí a doba jízdy mezi stanicemi byla 20 a 15 minut. 

In this experiment, we created a track with three stations and no independent trains. All passengers carried by the connecting train are therefore direct passengers. Then we modified the simulation model to give the connecting train a constant delay and disallowed it to reduce the delay. The expected result of this is that with increasing delay, the average travel time will increase in a linear way. The number of direct passengers was set to 25 in each station, and the travel times between stations were set to 20 and 15 minutes.

#figure(
  image("linear_delay.png"),
  caption: [Experiment result: linear growth of average travel time with growing constant delay]
)

=== Short track

// Experiment č.2 popisoval situaci, kdy vlak čekal na každé zastávce na přípojný vlak, který měl zpoždění v prvních 15 minutách s pravděpodobností 78 % a trasa byla nastavena na krátkou dobu jízdy. Nejlepší výsledek byl v případě, kdy vlak absolutně nečekal na žádný přípojný vlak z důvodu, že vlak se mnohdy stihl vrátit, a tudíž není zapotřebí čekat, když za chvíli jede zas.

// Druhých nejlepších časů dosáhlo, pokud vlak nabral zpoždění lehce nad 15 minut. To koresponduje i s daty získanými ze stránky Idos, tudíž dle výsledku lze experiment považovat za úspěšný. Konfigurace byla nastavena na 3 stanice, kde se na každé čekalo na přípojný vlak. Všechny vlaky vezly 50 lidí, na nástupišti nastoupilo 15 lidí, doba jízdy mezi stanicemi byla 20 a 15 minut, kde na zastávku přijížděl vlak s periodou 5, 4 a 3 doby okruhu vlaku dle zastávky a doba nástupu byla maximálně 3 minuty.  

This experiment simulates a scenario where a train waits at each stop for a connecting train, which arrives with a delay generated by the `get_delay` function based on real data from @delays_cd (the delay is less than 15 minutes with a probability of 78%), and the route was set for a short travel time. The best result was achieved when the train did not wait for any independent trains at all because the connecting train period was short (70 minutes), making it unnecessary to wait since the transfer passengers would not wait long for the connecting train to make another loop.

The configuration involved three stations where the train waited for an independent train at each one. All independent trains carried 50 passengers, with 15 direct passengers boarding at each station. The travel time between stations was 20 and 15 minutes respectively, and the independent trains arrived at the transfer stations with a periodicity of 5th, 4th, and 3rd connecting train period respectively. The boarding time was capped at a maximum of 3 minutes.

#figure(
  image("experiment_2.png"),
  caption: [Experiment result showing the average travel time on a short track] 
) 

=== Long track

// Experiment č. 3 vycházel z předchozího experimentu č. 2, kde jsme přidali další zastávku, aby se eliminoval případ, kdy se čekání vyplatí ze všeho nejvíc. Mezi druhou a třetí stanici se přidala další, kde přijížděl vlak s 50 cestujícími každé 4 periody doby jízdy vlaku, na nástupišti nastupovalo 15 lidí a doba do další stanice byla 15 minut. Tímto experimentem jsme prokázali, že když se zvýší počet zastávek a tím pádem i doba jízdy vlaku, výhodnější je počkat a vzít lidi z vlaku i za cenu, že vlak bude čekat. 

This experiment is based on the previous one, where we added an additional stop to make the main track longer to hopefully get different results from the previous experiment. Between the second and third stations, another stop is added, where an independent train carrying 50 passengers arrives every 4 connecting train periods. 15 direct passengers board the connecting train, and the travel time to the next station was 15 minutes. This experiment demonstrates that as the number of stops (and consequently the connecting train period) increases, it becomes more advantageous to wait for transfer passengers, even at the cost of delaying direct passengers.

#figure(
  image("experiment_3.png"),
  caption: [Experiment result showing the average travel time on a long track] 
)

=== Ratio of direct passengers to transfer passengers

This experiment evaluates the influence of how many direct passengers travel in the connecting train compared to transfer passengers. Intuitively, we would expect the simulation to return that it is more advantageous to wait on independent trains when the number of transfer passengers is greater. The track is the same as in the Short Track experiment. 

The number of transfer passengers in the first simulation (more transfer passengers) are `Uniform(80, 100)`, `Uniform(50, 80)` and `Uniform(25, 50)` in each station, and the number of direct passengers are 15, 10, and 5 respectively.

The number of transfer passengers in the second simulation (less transfer passengers) are 70, 40 and 25 in each station, and the number of direct passengers are `Uniform(80, 100`, `Uniform(50, 80)` and `Uniform(25, 50)` respectively.

#figure(
  image("more_pepole_in_train.png"),
  caption: [Experiment result with more transfer passengers] 
)
#figure(
  image("more_people_in_station.png"),
  caption: [Experiment result with less transfer passengers] 
)
    
=== Real-world example <real_world_track>

// Tento experiment popisuje skutečný vlak S31 (Os 6040) jedoucí z Nymburku do Mladé Boleslavi. Simuluje situaci, jak moc se vyplatí čekat na přípojný vlak S2 (Os 5822) jedoucí z Kolína do Nymburku. Tento osobní vlak nabývá minimálního zpoždění (27 z 32 do 5 minut a 5 z 32 do 10 minut) z naměřených dat databáze https://kam.mff.cuni.cz/. Experiment obsahuje všechny zastávky včetně odhadovaného počtu lidí, co nastupuje v dané stanici dle velikosti a frekventovanosti města. Z experimentu je jasně patrné, že vlaku se vyplatí čekat přibližně deset minut, což vychází i z databáze, která poskytuje data se zpožděným odjezdem maximálně do 15 minut.

This experiment describes the real train S31 (Os 6040) traveling from Nymburk to Mladá Boleslav. It simulates the scenario of how advantageous it is to wait for the independent train S2 (Os 5822) traveling from Kolín to Nymburk. This independent train usually arrives with no delay (27 out of 32 instances of delays are up to 5 minutes, and 5 out of 32 delays are between 5 and 10 minutes). This is extracted from @delays_cd. The experiment includes all stops @route_plans_cd, along with an estimated number of passengers boarding at each station based on the size of the station. The experiment shows that waiting for the independent train for approximately ten minutes is the most efficient policy overall, which aligns with the data from @delays_cd, indicating delayed departures of up to 15 minutes.

#figure(
  image("real_world.png"),
  caption: [Experiment result showing the average travel time on a real-world track] 
)

=== Determining the optimal offset withing connecting train period

When experimenting with a modified version of our model, we created a simulation which had slightly different properties from our other simulations. Firstly, there were no direct passengers boarding in any station. Secondly, the maximum wait time was set to zero, therefore the connecting train did not wait for any other delayed trains. Thirdly, the connecting train was delayed by a fixed initial amount of time at the start of simulation. When we calculate the average travel time with different initial offsets, we are basically calculating the point within the interval of the connecting train period, at which it is best to schedule the connecting train for a given set of stations and independent trains. For example, given the connecting train period $T_c = 80"min"$, we are calculating, at which point in this 80 minute interval should the train be located at the initial (leftmost) station. This could be used to create new train schedules based on approximate numbers of transfer passengers and independent train delays.

The following result is from an example three-station track with an independent train carrying 50 passengers at each station. Note that the first average travel time (at offset of 0 minutes) is approximately the same as the last average travel time (at offset of 80 minutes). This is to be expected because the offset of 80 minutes is a multiple of the independent train period (80 minutes) and therefore the result should be the same as with no offset at all.

#figure(
  image("offset_catch.png"),
  caption: [Experiment result of finding the optimal offset] 
)

== Conclusion of Experiments

// Závěry experimentů – bylo provedeno N experimentů v těchto situacích ... V průběhu experimentování byla odstraněna ... chyba v modelu. Z experimentů lze odvodit chování systémů s dostatečnou věrohodností a experimentální prověřování těchto ... situací již napřinese další výsledky, neboť ... 

// Byly provedeny experimenty v situacích, kdy vlak čekal jen na jeden vlak nebo bral lidi ze zastávky. V těchto experimentech jsme odstranili nedostatky s dobou čekání vlaku a nabíráním pasažérů. Jakmile experimenty podle pomocných výpisů prokázaly, že vlak nabral cestující, čekal na jiný vlak případně stihl odjet ve správný čas, experimentování pokračovalo dál. Z dat jsme dokázali vypozorovat, že pokud návazný vlak nedokáže dohnat zpoždění, jsme schopni odsimulovat ideální offset mezi vlaky, aby doba jízdy byla snížena na minimum. 

We ran experiments in which we spawned either just direct or just transfer passengers. In these experiments, we solved issues related to calculating train waiting times and passenger boarding. From the other experiments, we observed a few general facts about the system. 

Firstly, there is a relationship between the ratio of transfer passengers and direct passengers on the optimal maximum wait time. The more transfer passengers there are compared to direct passengers, the more advantageous it is to allow for greater maximum wait time. At some point, there is no reason to wait for delayed trains at all since any created delays will impact the direct passengers. This threshold can be found using our simulation model simply by iterating through different values for the numbers of passengers and calculating the average travel time.

Secondly, the length of the main track impacts the optimal wait time. For short tracks where the connecting train returns to the same station after a relatively short time (in other words, the connecting train period is short), the optimal value is zero. This is because the transfer passengers do not have to wait long for the train to make a full cycle. Above a certain track length, it is advantageous to allow for some maximum wait time. Exactly how much can be calculated using our model.

= Conclusion
// Závěrem dokumentace se rozumí zhodnocení simulační studie a zhodnocení experimentů (např. Z výsledků experimentů vyplývá, že ... při předpokladu, že ... Validita modelu byla ověřena ... V rámci projektu vznikl nástroj ..., který vychází z ... a byl implementován v ...).

In this work we created a model of a single railway track with a connecting train and an arbitrarily configurable number of stations and independent trains, for which the connecting train waits according to the policy used by the Czech Railways. We implemented the simulation model using SIMLIB and validated it against data collected on real-world railways. The model is implemented generically, and is therefore able to simulate a wide variety of specific real-world tracks simply by changing the track configuration and adjusting the stations' parameters. When simulating fictional tracks, the model produces results which satisfy basic reasoning. When the model's parameters are set to real-world data, the simulation results are aligned with data from the real track.

#bibliography("bibliography.bib")