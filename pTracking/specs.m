
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
% La información consiste de una celda N0, que contiene las coordenadas de cada
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
%N1 = cleanUp(N0, 2);
%display(sprintf("Hay %f objetos", length(N1)));

% Elimina objetos repetidos
%N2 = cleanUp(N1, 3);
%display(sprintf("Hay %f objetos", length(N2)));

%Normalizamos para obtener el vector de traslación.
Tv = pFW / norm(pFW)

l = length(N0);
Zs = zeros(l,3);

iK = inv(K);

%%Creamos una celda para almacenar resultados.
R={};
for i = 1:l
   [r,c] = size(N0{i});
   R{i}=zeros(r,5);
end

for i = 1 : l%Aquí tenemos el # del objeto que contiene su trayectoria.
    N0{i}(1,4) = 0;
    
    s=size(N0{i});
    for j = 2 : s(1)  %Aquí recorremos la trayectoria por cada objeto.
          Tx = Tv(1)*shift*N0{i}(j,3)/1000;
          Ty = Tv(2)*shift*N0{i}(j,3)/1000;
          Tz = Tv(3)*shift*N0{i}(j,3)/1000;
          %N0{i}(1,7) = Tz;
          %N0{i}(1,5) = Tx;
          %N0{i}(1,6) = Ty;
          X_ = [N0{i}(j-1,1), N0{i}(j-1,2),1];%Aquí la j-esima coordenada del i-esimo objeto anterior.
          X = [N0{i}(j,1), N0{i}(j,2),1];%Aquí la j-esima coordenada del i-esimo objeto.
        

         % Xw_ = Xw_ / Xw_(3);
         % Xw = Xw / Xw(3);
         if(X(2) - X_(2) != 0 & X(1) - X_(1) != 0)

            deltaX = X(1) - X_(1);%u
            deltaY = X(2) - X_(2);%v

            R{i}(j,1) = ((Tz)*(X(1) - pFPix(1)))/ deltaX ;  %Z;
            R{i}(j,2) =  ((Tz)*(X(2) - pFPix(2)))/ deltaY;   %Z;
            R{i}(j,3) = N0{i}(j,7); 
            R{i}(j,4) = N0{i}(j,10); 
            %N0{i}(j,8) = N0{i}(j,5) + Tz;% Z component
            %N0{i}(j,6) = Xw(1)*N0{i}(j,8) + Tx;
            %N0{i}(j,7) = Xw(2)*N0{i}(j,8) + Ty;
          else
            display("Division por cero detectada.")
            N0{i}(j,4) = 0;
            N0{i}(j,7) = Tz;
          endif
    endfor
endfor

%Plotting objects on 3D coordinates:

%figure(3);
%indexObj = 400;
%for i = 1:l
%  XObj = N0{i}(:,5); 
%  YObj = N0{i}(:,6); 
%  ZObj = N0{i}(:,7); 
%  display ([mean(XObj), mean(YObj), mean(ZObj)])
%  plot3(XObj, YObj, ZObj,'r*')
%  %plot3(mean(XObj), mean(YObj), mean(ZObj),'ro')
%endfor





