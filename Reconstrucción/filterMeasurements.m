function [nM,crib, objTray] = filterMeasurements(M, lThresh)
   l = length(M);
   crib=ones(l,1);
   objTray = zeros(l,1);
   for idx = 1:l
      acum = 0;
      d = size(M{idx});
      for j = 2:d(1)
         acum += sqrt((M{idx}(j,1)-M{idx}(j-1,1)).^2+(M{idx}(j,2)-M{idx}(j-1,2)).^2);
      end
      objTray(idx,1) = acum;

      if acum > lThresh
         crib(idx)=0;
      end
   end
   nl=sum(crib);
   nM=cell(nl,1);
   k = 1;
   for idx=1:l
      if crib(idx) ~= 0
         nM(k)=M{idx};
         k+=1;
      end
   end
end
