function [N] = cleanUp(M, fg, thr)
   % Cuenta cuantos objetos y cuantas coordenadas hay.
   nObjects = length(M);
   nCoors = 0;
   for i = 1:nObjects
      [r, c] = size(M{i});
      nCoors += r;
   end

   % Crea una matriz para almacenar todas las coordenadas de todos
   % los objetos.
   Objs=zeros(nCoors, c+4);

   % Vuelca las coordeandas en M en la matriz objs
   % La primera columna de Objs corresponde al indice del objeto.
   % Ajusta una linea recta a las coors asociadas a cada objeto
   % usando ransac para eliminar outliers. Almacena en el objeto
   % los parámetros de la linea, así como el error de cada punto
   % con respecto a la linea ajustada.
   row = 1;
   newM = {}
   acumIdx = 0;

   % Aqui se reubican los indices de cada rasgo seguido. Despues de este
   % ciclo for, cada elemento de la celda M queda como sigue:
   % campo 1: indice del rasgo
   % campo 2 y 3: Centro de Masa
   % campo 4: Indice de Frame
   % campo 5: confianza en el match
   % campo 6: índice del rasgo (feature)
   % campo 7: area del rasgo.
   % campo 8: Inlier (1), o Outlier(0):
   % campo 9 y 10: Parametrós de la linea que pasa por el rasgo.
   % campo 11: error.
   % campo 12 y 13: proyección del centro de masa en la linea a
   %                la que pertenece.
   %
   for i = 1:nObjects
      [r, c] = size(M{i});
      %Aplicamos Ransac a las coordenadas del objeto.
      [sol, idx, err] = ransac1(M{i}(:,1), M{i}(:,2), 0.5, .6);
      
      if(err <thr)
         M{i}(idx, 8) = 1; %Marcamos como 1 a los inliers
         newM{i} = M{i}(idx,:);
         acumIdx = acumIdx + length(idx);
         for j = 1:length(idx)
            Objs(row, 1) = i;
            Objs(row, 2:c+1) = newM{i}(j, :);
            Objs(row, 9:10) = sol;
            Objs(row,11) = err;

            %Encontrar parámetros de la recta perpendicular a l,
            m = sol(1);
            b = sol(2);

            mp = -1/m;
            bp = -mp*newM{i}(j,1)+newM{i}(j,2);

            %Proyectar la coordenada en la linea estimada (sol)
            xp = (bp - b)/(m - mp);
            yp = (m)*xp + b;
            %Almacenarla en las columnas 12 y 13
            Objs(row,12) = xp;
            Objs(row,13) = yp;

            row += 1;
         end
      endif
   end

%drop outliers allocation memory

%Busca Coordenadas repetidas
repeated = cell;
tmp = [];
currentObject = Objs(1, 1);
for i = 1:nCoors
   idO = Objs(i, 1);
   if idO ~= currentObject
      if length(tmp) > 0
         repeated{currentObject} = tmp;
         tmp = [];
      end
      currentObject = idO;
   end

   [x,y] = find (Objs(i+1:nCoors, 2) == Objs(i, 2) & Objs(i+1:nCoors, 3) == Objs(i, 3));
   if length(x) > 1
      for j = 1:length(x)
         id = Objs(x(j)+i, 1);
         if length(find (tmp == id)) == 0
            tmp(end+1) = id;
         end
      end
   end
end

% Eliminate repeated
idxM=[];
for i = 1:length(repeated)
   n = length(repeated{i});
   if n ~= 0
      [rmax, c] = size(M{i});
      bst = i;
      for j = 1:n
         k = repeated{i}(j);
         [r, c] = size(M{k});
         if r > rmax
            M{bst} = [];
            rmax = r;
            bst = k;
         else
            M{k} = [];
         end
      end
      idxM(end+1) = bst;
   else
      if length(M{i}) > 0
         idxM(end+1) = i;
      end
   end
end

if length(idxM) < 1
   N = M;
   return;
end

idxM=sort(idxM);
Q=[idxM(1)];
for i=2:length(idxM)
   if idxM(i-1) ~= idxM(i)
      Q(end+1) = idxM(i);
   end
end
idxM=Q;

   % Despues de este for se elimina el campo 1 de cada registro
   %  y queda entonces así:
   % campo 1 y 2: Centro de Masa
   % campo 3: Indice de Frame
   % campo 4: confianza en el match
   % campo 5: índice del rasgo (feature)
   % campo 6: area del rasgo.
   % campo 7: Inlier (1), o Outlier(0):
   % campo 8 y 9: Parametrós de la linea que pasa por el rasgo.
   % campo 10: error.
   % campo 11 y 12: proyección del centro de masa en la linea a
   %                la que pertenece.
   %

nObjects = length(idxM);
N={};
for i = 1:nObjects
   idxF = find (Objs(:,1) == idxM(i)) ;% Objs(:,8) == 1);
   N{i}=Objs(idxF,2:end);
end


figure(fg);
clf;
axis equal;
hold on;
for i = 1:nObjects
   idx = find (Objs(:,1) == idxM(i));
   idxF = find (Objs(:,1) == idxM(i) & Objs(:,8) == 1);
   %plot(Objs(idx,2), Objs(idx,3), Objs(idxF,2), Objs(idxF,3), 'r');
   plot(Objs(idxF,2), Objs(idxF,3), 'r');
end
hold off;



%Trabajos pendientes, realizar las capturar con valores de umbral diferentes... 0.1, 0.2, 0.3 y 0.5.

%
