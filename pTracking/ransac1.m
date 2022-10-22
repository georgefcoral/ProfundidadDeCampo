function [sol, Idx, err]=ransac1(X, Y, sigma2, pri)


% Here we found the number of samples needed in order to ensure that the
% probability p, that one random sample is free from outilers is 0.99.
Nsamples=ceil(log (1-.99)/log(1-(1-pri).^2));

tsamples=length(X);

% sol is an array were the solution will be stored. 
sol=zeros(2,1);

% The Threshold we use in order to classify outliers from inliers.
Threshold=3.84*sigma2;


%Generate a random index, used to draw samples of pair of points randomly.
R=rand(tsamples,1);
[S,Indice]=sort(R);
k=0;


for i=1:Nsamples   
	A=ones(2,2);
   B=ones(2,1);

	% Select a pair of points randomly and set up the matrices needed to solve the problem. 
   idx1 = mod(round(1000*rand(1)),tsamples-1)+1;
   idx2 = idx1;
   while (idx1 == idx2)
      idx2 = mod(round(1000*rand(1)),tsamples-1)+1;
   end 
   A(1,:)=[X(idx1), 1];
   A(2,:)=[X(idx2), 1];
   B(1,1)=Y(idx1);
   B(2,1)=Y(idx2);
   
	%Find line that fit the sample points selected earlier. 
   sol=A\B;
   
   
	%Estimate the minimum distance from each point in the set to the solution found before.
	m=sol(1);
   b=sol(2);
	Dists=zeros(tsamples,1);
   for j=1:tsamples
      x0=X(j);
		y0=Y(j);
		
		% The minimum distance found by solving for x in the derivative of the error 
		% function(x0-x).^2+(y0-(mx+b)).
		x1=(x0-m*(b-y0))/(1+m.^2);
		y1=[x1, 1]*sol;
		Dists(j)=(x1-x0).^2+(y1-y0).^2;
   end
	
	% Find those points in the set that are within the threshold.
	idx=find(Dists<=Threshold);
	% Count the number of inliers.
	ninl=length(idx);
	
	% If the number of inliers is the largest so far, store the index of inliers
	% and the solution.
	if (i>1)
		if ninl>Ninl
			Sol=sol;
			Ninl=ninl;
			Idx=idx;
		end
	else
		Sol=sol;
		Ninl=ninl;
		Idx=idx;
	end	
end

% Variable Sol stores the best solution found.
m=Sol;
b=Sol;

%Estimate the best solution from a overdetermined sistem of equations using the inliers.
A=zeros(Ninl,2);
B=zeros(Ninl,1);
for j=1:length(Idx)
   A(j,:)=[X(Idx(j)),1];
	B(j,1)=Y(Idx(j));
end;
sol=A\B;

%Compute error
m=sol(1);
b=sol(2);
Dists=zeros(length(Idx),1);
for j=1:length(Idx)
   x0=X(Idx(j));
	y0=Y(Idx(j));
		
	% The minimum distance found by solving for x in the derivative of the error 
	% function(x0-x).^2+(y0-(mx+b)).
	x1=(x0-m*(b-y0))/(1+m.^2);
	y1=[x1, 1]*sol;
	Dists(j)=(x1-x0).^2+(y1-y0).^2;
end
err = mean(Dists);
