clf
%Script de mediciones en riel con respecto a una cámara que se desplaza cada cierta cantidad de mm.

littleMesh = 20;   % # de dientes
bigMesh = 36;      % # de dientes

proportion = 5/9;  % # por cada vuelta de littleMesh se obtiene 5/9 del grande.

xEach10Rev = .005; % Relacion por cada 10 revoluciones existe .5 mm
                   % de desplazamiento en la riel.

oneRev = 0.053343; % mm de desplazamiento en la riel.
step = 0.000148056*1.8 % (1.8 * oneRev)/360

stepping = 100; 


shift = stepping * step;% Desplazamiento de la cámara a través de la riel.

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
N0 = cleanUp(M, 1, 0.2,pF/pF(3));
display(sprintf("Hay %f objetos", length(N0)));
%plotResults(N0)
figure(8)
for idx=1:length(M)
  plot(M{idx}(:,1),M{idx}(:,2),'.b')
endfor
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
   R{i}=zeros(r,7);
end

dTx = Tv(1) * shift / 1000;
dTy = Tv(2) * shift / 1000;
dTz = Tv(3) * shift / 1000;%Incremento de Desplazamiento.

for i = 1 : l%Aquí tenemos el # del objeto que contiene su trayectoria.
    N0{i}(1,4) = 0;
    
    s=size(N0{i});
    for j = 2 : s(1)  %Aquí recorremos la trayectoria por cada objeto.



          Tx = dTx * N0{i}(j,3);
          Ty = dTy * N0{i}(j,3);
          Tz = dTz * N0{i}(j,3);%Desplazamiento absoluto con respecto al primer cuadro.


          X_ = [N0{i}(j-1,11), N0{i}(j-1,12),1];%Aquí la j-esima coordenada del i-esimo objeto anterior.
          X = [N0{i}(j,11), N0{i}(j,12),1];%Aquí la j-esima coordenada del i-esimo objeto.
        

        % Xw_ = Xw_ / Xw_(3);
        % Xw  = Xw  / Xw (3);
        if(X(2) - X_(2) != 0 & X(1) - X_(1) != 0)

            deltaX = X(1) - X_(1);%u
            deltaY = X(2) - X_(2);%v

            R{i}(j,1) = ((dTz)*(X(1) - pFPix(1))) / deltaX;  %Z;
            R{i}(j,2) = ((dTz)*(X(2) - pFPix(2))) / deltaY;  %Z;
            R{i}(j,3) = N0{i}(j,  7); 
            R{i}(j,4) = N0{i}(j, 10); 
            R{i}(j,5) =  R{i}(j,  1) + Tz;% Z component

            Xw  = iK * transpose(X);
            Xw = Xw / Xw(3);
            R{i}(j,6) = Xw(1) * R{i}(j, 5) + Tx;%Esta es X
            R{i}(j,7) = Xw(2) * R{i}(j, 5) + Ty;%Esta es Y
            %R{i}(j,8) = Xw(1) * R{i}(j, 1) + Tx;%Esta es X
            %R{i}(j,9) = Xw(2) * R{i}(j, 1) + Ty;%Esta es Y

          else
            
            display("Division por cero detectada.")
            N0{i}(j,4) = 0;
            N0{i}(j,7) = Tz;
          end

    endfor
endfor

%Plotting objects on 3D coordinates:

figure(4);

hold on
s1=length(R);
for i = 1:s1% de objeto
  if(length(R{i})!=0)


    % Z = R{i}(2:end,5);
    % X = R{i}(2:end,6);
    % Y = R{i}(2:end,7);


    Z = median(R{i}(2:end,5));
    X = median(R{i}(2:end,6));
    Y = median(R{i}(2:end,7));
   %Xt = median(R{i}(2:end,8));
   %Yt = median(R{i}(2:end,9));
    plot3(X,Y,Z,'.k');%, Xt, Yt, Z, '.r');
  endif
endfor

xlabel('X');
ylabel('Y');
zlabel('Z');

% figure(3);
% clf
% hold on
% for i = 1:l% de objeto
%   [s1,s2]=size(R{i});
%   Z = zeros(s1,1);
%   for j = 2:s1%indice por cada objeto
%     XObj = R{i}(j,6); 
%     YObj = R{i}(j,7); 
%     ZObj = R{i}(j,5); 
%     Z(j-1) = ZObj;
%     %display ([mean(XObj), mean(YObj), mean(ZObj)])
%     %plot3(XObj, YObj, ZObj,'r*')
%   endfor
%   plot(i,median(Z),'b*')
%   pause()
% endfor





