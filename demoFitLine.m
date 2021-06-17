%Definimos una linea aleatoria.
A = round(1500*rand()-750);
C = round(1500*rand()-750);
while (1)
   B = round(1500*rand()-750);
   if B ~= 0
      break
   end
end

m = -A / B;
b = -C / B;

%Generamos 100 puntos que yacen en la linea definida, y le añadimos ruido.
n=100;
x=sort(1200*rand(1,n)-600);
y = m * x + b;
xr=x;
yr=yr=y+1*(rand(1,n)-0.5);


%Implementamos el algoritmo de la página (298pdf, 297 libro).
M = zeros(3);
N = M;
Mr = N;
Nr = Mr;

epsilon = 0.1; %Variance of the \delta x
Vo=diag([epsilon.^2,epsilon.^2,0]);
W = 1;
Wr = 1;
c = 0;
cr = 0;

figure(1);
clf;
figure(2);
clf;
plot(x,y, 'b');
hold on;
cont = 0;
while 1
   for i=1:n
      p   = [x(i), y(i), 1]';
      M  += W * p * p'; 
      pr  = [xr(i), yr(i), 1]';
      Mr += Wr * pr * pr';
      N  += W * Vo; 
      Nr += Wr * Vo; 
   end
   M  /= n;
   Mr /= n;
   N  /= n;
   Nr /= n;

   [en, el] = eig(M - c * N);
   en = en(:,1);
   el = el(1,1);

   [enr, elr] = eig(Mr - cr * Nr);
   enr = enr(:,1);
   elr = elr(1,1);
   display (sprintf('Wr = %f', Wr));
   display (sprintf('cr = %f', cr));
   display (sprintf('elr = %f', elr));

   cont += 1;
   if abs(elr) < 0.0000003
      break;
   end
   
   Af = enr(1);
   Bf = enr(2);
   Cf = enr(3);
   mf = -Af / Bf;
   bf = -Cf / Bf;

   xf = x;
   yf = mf * xf + bf;
   plot(xf, yf, 'r', xr, yr, 'g*');


   W = 1 / dot(en, Vo * en);
   Wr = 1 / dot(enr, Vo * enr);
   c = c + el / dot(en, N * en);
   cr = cr + elr / dot(enr, Nr * enr);

   Error = sum([y-yf].^2)/n;
   display (sprintf('Error = %f\n', Error));
   pause(1)
end

figure(1);
Af = enr(1);
Bf = enr(2);
Cf = enr(3);
mf = -Af / Bf;
bf = -Cf / Bf;

xf = x;
yf = mf * xf + bf;

plot(x,y, 'b', xf, yf, 'r', xr, yr, 'g*');
figure(2)
plot(xf, yf, 'm' );
hold off

Error = sum([y-yf].^2)/n;
display (sprintf('Error = %f\n', Error));
display(sprintf('cont = %d', cont));
display(sprintf('epsiolon^2 = %f', cr/(1-(2/n))));
