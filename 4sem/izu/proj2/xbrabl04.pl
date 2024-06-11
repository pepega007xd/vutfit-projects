% Zadani c. 7:
% Napiste program resici ukol dany predikatem u7(LIN,VOUT), kde LIN je vstupni 
% ciselny seznam s nejmene jednim prvkem a VOUT je vystupni promenna, ve ktere  
% se vraci hodnota rozdilu poctu kladnych a zapornych cisel v seznamu LIN (nula  
% neni ani kladne, ani zaporne cislo). 

% Testovaci predikaty:                                   	% VOUT 
u7_1:-u7([5,27.5,-1.4,0,16,-4], VOUT),write(VOUT).              % 1
u7_2:-u7([0],VOUT),write(VOUT).                                 % 0
u7_3:-u7([-1,-34,98.5,-2.78,-4],VOUT),write(VOUT).             	% -3
u7_r:- write('Zadej LIN: '),read(LIN),u7(LIN,VOUT),write(VOUT).

% Reseni:
u7([], VOUT) :- VOUT is 0.

u7([HEAD | TAIL], VOUT) :- u7(TAIL, TAIL_VOUT),
    clamp(HEAD, CLAMPED_HEAD), 
    VOUT is TAIL_VOUT + CLAMPED_HEAD.

clamp(X, OUT) :- X > 0, OUT is 1.
clamp(X, OUT) :- X == 0, OUT is 0.
clamp(X, OUT) :- X < 0, OUT is -1.
