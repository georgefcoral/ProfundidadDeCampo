%Definimos una punto aleatorio.
x = round(1500*rand()-750);
y = round(1500*rand()-750);

X = [x, y, 1]';

%Generamos 100 lineas que concurren en el punto definido, y les añadimos ruido.


n=20;
L=zeros(3,n);
XP=zeros(3,n);
for i=1:n
   %Generamos un punto Xp aleatorio.
   theta=2*pi*(rand(1)-0.5);
   radio=50;
   Xp =[X(1:2)+radio*[cos(theta),sin(theta)]';1];
   %Encontramos la linea que los une.
   l=cross(X, Xp);
   %La normalizamos
   l=l/norm(l);
   %Le añadimos ruido.
   l=l+0.00001*(rand(3,1)-0.5);
   L(:,i)=l;
   XP(:,i)=Xp;
end

figure(1);
clf;
hold on;
for i=1:n
   if L(2,i) ~= 0
      m=-L(1,i)/L(2,i);
      b=-L(3,i)/L(2,i);
      dx=XP(1,i)-X(1);
      xa=X(1)-dx;
      xb=X(1)+dx;
      ya = m * xa + b;
      yb = m * xb + b;
      plot([xa,xb],[ya,yb],XP(1,i)/XP(3,i), XP(2,i)/XP(3,i),'r*');
   end
end  
plot(X(1),X(2),'k*');
axis equal;
hold off;

%
%%Implementamos el algoritmo de la página (298pdf, 297 libro).
M = zeros(3);
N = M;

epsilon = 0.1; %Variance of the \delta x
Vo=diag([epsilon.^2,epsilon.^2,0]);
W = 1;
c = 0;
%
figure(1);
hold on;
cont = 0;
while 1
   for i=1:n
      l   = [L(1,i), L(2,i), L(3,i)]';
      M  += W * l * l'; 
      N  += W * Vo; 
   end
   M  /= n;
   N  /= n;

   [en, el] = eig(M - c * N);
   en = en(:,1);% Eigenvector asociado al eigenvalor menor
   el = el(1,1);% Eigenvalor menor

   display (sprintf('W = %f', W));
   display (sprintf('c = %f', c));
   display (sprintf('el = %f', el));

   cont += 1;
   if abs(el) < 0.0000003
      break;
   end

   W = 1 / dot(en, Vo * en);
   c = c + el / dot(en, N * en);

   pause(1)
end
Xc = en/en(3);
plot(Xc(1), Xc(2), '*b');
hold off;
display(sprintf('cont = %d', cont));
display(sprintf('epsiolon^2 = %f', c/(1-(2/n))));
hold off;
