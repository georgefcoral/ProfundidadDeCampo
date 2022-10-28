function [N] = cleanUp(M, fg)
   % Cuenta cuantos objetos y cuantas coordenadas hay.
   nObjects = length(M);
   nCoors = 0;
   for i = 1:nObjects
      [r, c] = size(M{i});
      nCoors += r;
   end

   % Crea una matriz para almacenar todas las coordenadas de todos
   % los objetos.
   Objs=zeros(nCoors, c+3);

   % Vuelca las coordeandas en M en la matriz objs
   % La primera columna de Objs corresponde al indice del objeto.
   % Ajusta una linea recta a las coors asociadas a cada objeto
   % usando ransac para eliminar outliers. Almacena en el objeto
   % los parámetros de la linea, así como el error de cada punto
   % con respecto a la linea ajustada.
   row = 1;
   for i = 1:nObjects
      [r, c] = size(M{i});
      %Aplicamos Ransac a las coordenadas del objeto.
      [sol, idx, err] = ransac1(M{i}(:,1), M{i}(:,2), 0.5, .6);
      M{i}(idx, 7) = 1; %Marcamos como 1 a los inliers
      for j = 1:r
         Objs(row, 1) = i;
         Objs(row, 2:c+1) = M{i}(j, :);
         Objs(row, 9:10) = sol;
         Objs(row,11) = err;
         %Proyectar la coordenada en la linea estimada (sol)
         %Almacenarla en las columnas 12 y 13
         row += 1;
      end
   end

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

nObjects = length(idxM);
N={};
for i = 1:nObjects
   idxF = find (Objs(:,1) == idxM(i));% & Objs(:,8) == 1);
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


