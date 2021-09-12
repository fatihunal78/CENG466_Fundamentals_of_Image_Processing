function [acc] = tdoldur (acc, t, kenar)
[m,n] = size(kenar);
%fprintf('tdoldur: m,n = %d,%d\n',m,n);
for x = 1:m
    for y = 1:n
        if (kenar(x,y) == 1)
            % her kenar için yapýlacak iþlemler
            for a = 1:m
                b = a*t(x,y) - x*t(x,y) + y;
%		    fprintf('b = %d\tn = %d\n', b, n);
		        b = round(b);
		        if ((b < 1) | (b > n) | ~(isfinite(b)))
			        continue;
		        end;
%		    [a,b]
%		        acc(a,b) = acc(a,b) + 1;
		        r = sqrt((x-a)^2 + (y-b)^2);
                r = round(r);
                if (r < 11) | (r > 80) 
                    continue;
                end;
%                fprintf('r=%d\t', r);
                acc(a,b,r-10) = acc(a,b,r-10) + 1;
	        end;
        end;
    end;
end;
