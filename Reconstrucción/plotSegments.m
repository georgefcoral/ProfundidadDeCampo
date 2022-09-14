function [] = plotSegments(M, pF, thr)
   l = length(M);
   clf
   hold on

   pF=pF/pF(3);
   plot (pF(1),pF(2),'r*')
   for i = 1:l
      d = size(M{i});
      crib = ones(d,1);
      for j=1:d(1)
         if M{i}(j,4)>thr
            crib(j)=0;
         end
      end
      idxs = find(crib);
      plot(M{i}(idxs,1), M{i}(idxs,2));
      pause
   end
   axis equal;
   hold off
end
