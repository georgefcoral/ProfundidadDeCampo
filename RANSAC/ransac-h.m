function [inl, outl, X]=ransac(X, Y, pri)
% Esta funcion encuentra el polinomio de la forma ay+bx^3+cx^2+dx+e=0.

Nsamples=ceil(log (.99)/log(1-(1-pri).^4));

tsamples=length(X);

Nsamples=200;
sol=zeros(4,Nsamples);
for i=1:Nsamples
   R=rand(tsamples,1);
   [S,I]=sort(R);
   A=ones(4,4);
   b=ones(4,1);
   for j=1:5
      x=X(I(j));
      A(j,:)=[x.^3, x.^2, x, 1];
      b(j,1)=Y(I(j));
   end
   sol(:,i)=A\b;
 % [S,V]=eig(A*A');
 % /sol(:,i)=S(:,5);
   
   ninl=0;
   noutl=0;
   a=sol(1);
   b=sol(2);
   c=sol(3);
   d=sol(4);
   for j=1:tsamples
      x=X(j);
      Jacobiano=9*a.^2*x.^4+12*a*b*x.^3+(6*a*c+4*b.^2)*x.^2+4*b*c*x+c.^2;
      epsilon=(Y(j)-(a.*x.^2+b.*x.^2+c.*x+d)).^2;
      d=epsilon/Jacobiano;
      if (abs(d)<=2.4474)
         ninl=ninl+1;
      else
         noutl = noutl+1;
      end
   end
   [ninl,noutl]
   inl(i)=ninl;
   outl(i)=noutl;
end

