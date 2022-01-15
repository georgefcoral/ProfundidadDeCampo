
lin=0;
lisup=7;
m=1.5;
b=3;
PercErr = .25;

%Posicion absoluta en centimetros del riel.
ms1 = [15.42,15.346,14.184,13.630,13.094,12.022,11.948];


%Numero de revoluciones que dio el motor
rev1 = [70,60,50,40,30,20,10];

%Posicion absoluta en centimetros del riel.
ms2 = [11.890,12.240,12.880,13.428,13.956,14.524,15.150,15.624];

%Numero de revoluciones que dio el motor
rev2 = [10,20,30,40,50,60,70,80];


% Ajustamos los datos usando ransac.
% sol contiene los parámetros de la linea.
sol1=ransac1(rev1, ms1,0.5,0.25);
sol2=ransac1(rev2, ms2,0.5,0.25);


% Graficamos los datos y la linea que se ajusta a ellos.
ms1Est=polyval(sol1, rev1);
ms2Est=polyval(sol2, rev2);
plot(rev1,ms1, 'b*', rev1, ms1Est, 'b', rev2, ms2, 'r*', rev2, ms2Est, 'r');

