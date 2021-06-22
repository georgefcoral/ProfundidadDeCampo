function [x, xp] = desplazamiento (X, K, T, dt) 
   [r,N] = size(X);
   if r ~= 4 && r~= 3
      display ('Error, X debe tener 3 o 4 renglones, pues representa coordenadas 3D cartesianas u homogeneas');
      return
   end
   if r == 3
      X = [X;ones(1,N)];
   else
      for i=1:N
         X(:,i) /= X(4,i);
      end
   end

   x = K * [eye(3),T] * X;

   for i=1:N
      x(:,i) /= x(3,i);
   end
   xp=zeros(size(x));
   xp(1,:)=x(1,:) + dt .* (x(1,:).*T(3)-T(1)) ./ X(3,:);
   xp(2,:)=x(2,:) + dt .* (x(2,:).*T(3)-T(2)) ./ X(3,:);
   xp(3,:)=ones(1,N);
end
