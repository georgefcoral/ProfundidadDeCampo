function[Idx, TT] = simCamMov(Pts, K, R, T, incZ)
   figure(1);
   clf;

   zMin = min(Pts(:,3));
   zMax = max(Pts(:,3));
   %Esto que sigue es para calcular el area en que se va a dibujar.
   %No es realmente importante.
   xMin = min(Pts(:,1));
   xMax = max(Pts(:,1));
   yMin = min(Pts(:,2));
   yMax = max(Pts(:,2));
   Corners = [ xMin, yMin, zMin, 1;... 
      xMin, yMax, zMin, 1;...
      xMax, yMax, zMin, 1;...
      xMax, yMin, zMin, 1;...
      xMin, yMin, zMax, 1;...
      xMin, yMax, zMax, 1;...
      xMax, yMax, zMax, 1;...
      xMax, yMin, zMax, 1]';
   x = K * [R, T] * Corners;
   for j=1:length(Corners)
      x(:,j) /= x(3,j);
   end
   xM = max(x(1,:));
   xm = min(x(1,:));
   yM = max(x(2,:));
   ym = min(x(2,:));
   xRng = (xM-xm)/8;
   yRng = (yM-ym)/8;

   % Definimos la profundidad de campo.
   pCampo = (zMax-zMin)/25;
   
   % Distancia de avance de la cámara.
   zTravel=0.6;
   N = round(zTravel/incZ);

   % Definimos (de maner medio arbitraria pero bueno)
   % Que la al inicio del movimiento la camara enfoca bien
   % la region que empieza en la coordenada del objeto mas
   % cercana a la camara y termina pCampo despues.
   z=zMax;
   nP = length(Pts(:,3));
   Idx = zeros(N, nP);
   TT = zeros(N, 3);
   for i = 1:N
      idx=find(Pts(:,3)<z & Pts(:,3)>=(z-pCampo));
      Idx(i,idx) = 1;  
      X = [Pts(idx,:)';ones(1,length(idx))];
      x = K * [R, T] * X;
      TT(i, :) = T';
      [r,c] = size(x);
      for j=1:c
         x(:,j) /= x(3,j);
      end
      %plot(x(1,:), x(2,:), "*");
      %axis ([xm - xRng, xM + xRng, ym - yRng, yM + yRng], "square");
      %pause(0.01);
      z = z-incZ;
      T = T + [0,0,incZ]';
   end
end
