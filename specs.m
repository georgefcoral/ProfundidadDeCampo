
%Script de mediciones en riel con respecto a una cámara que se desplaza cada cierta cantidad de mm.



littleMesh = 20 % # de dientes
bigMesh = 36 % # de dientes

proportion = 5/9 % # por cada vuelta de littleMesh se obtiene 5/9 del grande.

xEach10Rev = .005 %Relacion por cada 10 revoluciones existe .5 mm de desplazamiento en la riel.

oneRev = 0.053343 %mm de desplazamiento en la riel.


stepping = 100;


shift = stepping * oneRev;% Desplazamiento de la cámara a través de la riel.

%Seguimiento de los objetos en el plano de imagen

tracking



puntoDeFugaH

%Matriz de calibración K 

K = [[1743,0,354];[0,1743,233];[0,0,1]]

pFW = K^-1 * pF










