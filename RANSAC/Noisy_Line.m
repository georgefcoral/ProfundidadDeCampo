function[X,Y]=Noisy_Line(n, limi, lims, sigma2, poutl, m, b)

if poutl > 1 | poutl < 0
	display 'poutli debe ser un valor en el intervalo [0,1]';
	return
end

if limi >= lims
	display 'intervalo erroneo'
	return
end

mint=(limi+lims)/2;
tint=lims-limi;

ninliers=round(n*(1-poutl));
noutliers=n-ninliers;
X=tint*rand(1,ninliers)-(0.5*tint+mint)*ones(1,ninliers);
Y=[m,b]*[X;ones(1,ninliers)];

X=X+mgd(ninliers,1,0,sigma2)';
Y=Y+mgd(ninliers,1,0,sigma2)';

X=[X,(max(X)-min(X))*rand(1,noutliers)+min(X)];
Y=[Y,(max(Y)-min(Y))*rand(1,noutliers)+min(Y)];

[S,I]=sort(rand(n,1));
X=X(I);
Y=Y(I);
