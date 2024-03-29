
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

umbral = 1000000

%Normalizamos para obtener el vector de traslación.
Tv = pFW / norm(pFW)

l = length(M);
Zs = zeros(l,3);

iK = inv(K);
for i = 1 : l%Aquí tenemos el # del objeto que contiene su trayectoria.
    M{i}(1,4) = 0;
    s=size(M{i});
    for j = 2 : s(1)  %Aquí recorremos la trayectoria por cada objeto.
          Tx = Tv(1)*shift*M{i}(j,3)/1000
          Ty = Tv(2)*shift*M{i}(j,3)/1000
          Tz = Tv(3)*shift*M{i}(j,3)/1000
          X_ = [M{i}(j-1,1), M{i}(j-1,2),1]%Aquí la j-esima coordenada del i-esimo objeto anterior.
          X = [M{i}(j,1), M{i}(j,2),1]%Aquí la j-esima coordenada del i-esimo objeto.
          Xw_ = iK * transpose(X_)
          Xw  = iK * transpose(X)
          if(Xw(2) - Xw_(2) != 0)
            M{i}(j,4) = (-Ty + Xw(2)*Tz)/Xw(2) - Xw_(2) ;
          else
            M{i}(j,4) = 0;
          endif
    endfor
endfor



[nM,cr,tray] = filterMeasurements(M,umbral);

%plotSegments(nM,pF,2)













