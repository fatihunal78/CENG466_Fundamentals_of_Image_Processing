function [cem,say] = cemberleribul (acc, T)
cem = [];
say = 0;
[m,n,ll] = size(acc);
k = 0;
for x = 1:m
	for y = 1:n
        t = 0;
        for z = 1:80
            t = t + acc(x,y,z);
        end;
		if (t > T) 
            r = 0;
            for z = 1:80
                if (acc(x,y,z) > r)
                    r = z;
                end;
            end;
            cem = [cem; x, y, r+10];
			say = say + 1;
		end;
	end;
end;

return;
