function [] = plotResults(N)
	figure(1);
	clf;
	for idx=1:length(N)
	   idxInl = find (N{idx}(:,7) == 1);
	   idxOutl = find (N{idx}(:,7) == 0);
		plot(N{idx}(idxInl,1), N{idx}(idxInl,2),'b',N{idx}(idxInl,1), N{idx}(idxInl,2),'b.');
		axis equal
		hold on;
		figure(2);
		subplot(1,2,1)
		plot(N{idx}(idxInl,1), N{idx}(idxInl,2),'r', N{idx}(idxInl,1),N{idx}(idxInl,8).*N{idx}(idxInl,1)+N{idx}(idxInl,9), N{idx}(idxOutl,1), N{idx}(idxOutl,2),'k*');
		subplot(1,2,2)
		plot(1:length(N{idx}(idxInl,6)),N{idx}(idxInl,6))
		display(sprintf('error=%f',N{idx}(1,10)));
		pause
		figure(1);
		plot(N{idx}(idxInl,1), N{idx}(idxInl,2),'r',N{idx}(idxInl,1), N{idx}(idxInl,2),'r.', N{idx}(idxInl,1),N{idx}(idxInl,8).*N{idx}(idxInl,1)+N{idx}(idxInl,9),'g')
	end
end

