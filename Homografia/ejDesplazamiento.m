% Vector de Translación
T = [0.05,-0.04, 0.06]';
%T = [1, 1, 0.1]';

% Datos 3D
figure (1);
N=100;
X=[5*(rand(1,N)-0.5);3.3*(rand(1,N)-0.5);1 + 20 * rand(1,N); ones(1,N)];
plot3(X(1,:),X(2,:),X(3,:),'*');
K=[1000,0,400;0,1000,300;0,0,1];

%proyeccion y movimiento a lo largo del eje T
%Puntos a diferentes profundidades.
dt = 4;
[x,xp] = desplazamientoIdeal(X, T, dt);
figure (2);
plot(x(1,:),x(2,:),'r*',xp(1,:),xp(2,:),'b*',T(1)/T(3), T(2)/T(3), '*k')
axis equal;
hold on;
for i=1:N
   plot( [x(1,i),xp(1,i)], [x(2,i),xp(2,i)], 'm');
end
hold off;

%proyeccion y movimiento a lo largo del eje T
%Puntos a la misma profundidad.
X(3,:)=2 * ones(1,N);
[x,xp] = desplazamientoIdeal(X, T, 4);
figure (3);
plot(x(1,:),x(2,:),'r*',xp(1,:),xp(2,:),'b*',T(1)/T(3), T(2)/T(3), '*k')
axis equal;
hold on;
for i=1:N
   plot( [x(1,i),xp(1,i)], [x(2,i),xp(2,i)], 'm');
end
hold off;

%Calculamos la homografia entre dos frames cuando la camara se desplaza
%con una velocidad translacional definida por T en intervalo dt de tiempo.
xpn=zeros(size(xp));
for i=1:N
   q=(X(3,i)+dt*T(3))/X(3,i);
   H=[q, 0, -T(1)*dt/X(3,i); 0, q, -T(2)*dt/X(3,i);0,0,1];
   xpn(:,i) = H * x(:,i);
end
