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

%tracking;
% run("./Models/1/tracking.m") 
% run("./Models/2/tracking.m") 
run("./Models/3/tracking.m") 
%run("./Models/4/tracking.m") 
%run("./Models/5/tracking.m") 


%Matriz de calibración K 
K = [[1743,0,354];[0,1743,233];[0,0,1]]

% Lo multiplica por la inversa de la matriz de covarianza 
% Para manejarlo coordenadas canónicas.

pFW = K^-1 * pF

display(sprintf("Hay %f objetos", length(M)));

% Elimina objetos repetidos
N0 = cleanUp(M, 1, 0.3,pF/pF(3));

save("N0.mat","N0")
display(sprintf("Hay %f objetos", length(N0)));
%plotResults(N0)

% for idx=1:length(M)
%   plot(M{idx}(:,1),M{idx}(:,2),'.b')
% endfor
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


R = find3DCoords(N0,l,Tv,iK,shift,pFPix,stepping,step)

%Plotting objects on 3D coordinates:


plotting3D(R,3,pFW)


% %open file identifier
% fid=fopen('model.xyz','w');
% fod = fopen('modelDetalled.xyz','w');

% hold on
% axis equal
% s1=length(R);
% for i = 1:s1% de objeto
%   if(length(R{i})!=0)

%     % Z = R{i}(2:end,5);
%     % X = R{i}(2:end,6);
%     % Y = R{i}(2:end,7);

%     Z = median(R{i}(2:end,5));
%     X = median(R{i}(2:end,6));
%     Y = median(R{i}(2:end,7));
%     ZE = R{i}(2:end,5);
%     XE = R{i}(2:end,6);
%     YE = R{i}(2:end,7);
%     %['33.126', '-73', '6.339']
%     ptStr = cstrcat(num2str(X),' ',num2str (Y),' ',num2str (Z),'\n')
%     for j = 1:length(XE)
%       ptStr2 = cstrcat(num2str(XE(j)),' ',num2str (YE(j)),' ',num2str (ZE(j)),'\n')
%       fprintf(fod,ptStr2);
%     endfor   
%     fprintf(fid,ptStr);
%    %Xt = median(R{i}(2:end,8));
%    %Yt = median(R{i}(2:end,9));
%     plot3(XE,YE,ZE,'.k');%, Xt, Yt, Z, '.r');
%   endif
% endfor

% xlabel('X');
% ylabel('Y');
% zlabel('Z');

% fclose(fod)
% fclose(fid)





