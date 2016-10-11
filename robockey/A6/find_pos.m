data = [492	330	10	473	368	8	567	367	10	513	402	5];
x = [data(1) data(4) data(7) data(10)];
y = [data(2) data(5) data(8) data(11)];
figure(1)
hold on
axis([1 1024 1 768])
% step 1, find maximum distance
d = [];
for i = 1 : 3
    for j = i+1 : 4
        if (x(i)>1 && x(i)<1023 && x(j)>4 && x(j)<1020 && ...
            y(i)>1 && y(i)<767 && y(j)>1 && y(j)<767)
            dist = (x(i) - x(j))^2 + (y(i) - y(j))^2;
        else
            dist = 0;
        end
        d = [d; i, j, dist];
    end
end
[v,ind] = max(d(:,3));
p1 = [x(d(ind,1)), y(d(ind,1))];
p2 = [x(d(ind,2)), y(d(ind,2))];

% step 2, find the center, calculate vector
c = 0.5 * (p1 + p2);
%     plot(c(1), c(2), 'go')

% step 3, find the upper star 
vx = x - c(1);
vy = y - c(2);
flag = 0;
q = 1;
while(flag == 0)
    i = d(ind, 1);
    j = d(ind, 2);
    if (q ~= i && q~= j)
        temp = [vx(q), vy(q)] * [vx(i); vy(i)];
        if (temp > 0)
            up = i;
        else
            up = j;
      end
        flag = 1;
    end
    q = q + 1;
end

% step 4, calculate center of m_wii
c_wii = [1024, 768] - c;
plot(c_wii(1), c_wii(2), 'ro')

% step 5, calculate the vector
dir_wii = [-vx(up), vy(up)];
pointer = c_wii + 2*dir_wii; % enlarge 2 times
arr_x = [c_wii(1), pointer(1)];
arr_y = [c_wii(2), pointer(2)];

drawArrow(arr_x, arr_y,{'MaxHeadSize',2})
pause(0.001)
% data analysis

