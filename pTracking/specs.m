
%Script de mediciones en riel con respecto a una cámara que se desplaza cada cierta cantidad de mm.



littleMesh = 20 % # de dientes
bigMesh = 36 % # de dientes

proportion = 5/9 % # por cada vuelta de littleMesh se obtiene 5/9 del grande.

xEach10Rev = .005 %Relacion por cada 10 revoluciones existe .5 mm de desplazamiento en la riel.

oneRev = 0.053343 %mm de desplazamiento en la riel.


stepping = 100;


shift = stepping * oneRev;% Desplazamiento de la cámara a través de la riel.

% Seguimiento de los objetos en el plano de imagen
% Carga la información de seguimiento de objetos generada por PuntoDeFuga2
% La información consiste de una celda M, que contiene las coordenadas de cada
% uno de los objetos encontrados.
tracking;


% Carga el punto de fuga.
puntoDeFugaH

%Matriz de calibración K 
K = [[1743,0,354];[0,1743,233];[0,0,1]]

% Lo multiplica por la inversa de la matriz de covarianza 
% Para manejarlo coordenadas canónicas.
pFW = K^-1 * pF

display(sprintf("Hay %f objetos", length(M)));

% Elimina objetos repetidos
N0 = cleanUp(M, 1);
display(sprintf("Hay %f objetos", length(N0)));

% Elimina objetos repetidos
N1 = cleanUp(N0, 2);
display(sprintf("Hay %f objetos", length(N1)));

% Elimina objetos repetidos
N2 = cleanUp(N1, 3);
display(sprintf("Hay %f objetos", length(N2)));







