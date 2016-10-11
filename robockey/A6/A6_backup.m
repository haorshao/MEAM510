clc;clear all;
load('mWii_training_data_v2/mWii_training_data/A.mat');
drawArrow = @(x,y,props) quiver( x(1),y(1),x(2)-x(1),y(2)-y(1),0, props{:} );
figure(1)
clf
hold on
axis([1 1024 1 768])
xlabel('x-axis');
ylabel('y-axis');
actmax = [29; 25.99; 23.08];
% actmax = [all 4 or miss 2,3; miss 4; miss 1]

threshold = 1.5;
pix2act = 29/99.40;

P1 = [1024/2 768/2] + [0, -14.5] / pix2act;
P2 = [1024/2 768/2] + [-10.563, 2.483] / pix2act;
P3 = [1024/2 768/2] + [11.655, 8.741] / pix2act;
P4 = [1024/2 768/2] + [0, 14.5] / pix2act;

plot([P1(1) P2(1) P3(1) P4(1)], [P1(2) P2(2) P3(2) P4(2)], '*')

arr_wx = [1, 100];
arr_wy = [1, 100];
drawArrow(arr_wx,[1, 1],{'MaxHeadSize',2});
drawArrow([1, 1], arr_wy,{'MaxHeadSize',2});

for  k = 1 : size(rawStarData, 1)
    data = rawStarData(k,:);
    x = [data(1) data(2) data(3) data(4)];
    y = [data(5) data(6) data(7) data(8)];

    % pre-process, get rid of noise
    for i = 1 : 4
        if (x(i)==1023 || y(i)==1023)
            x(i) = 0;
            y(i) = 0;
        end
    end
    x(x == 0) = [];
    y(y == 0) = [];
    
    if (length(x) < 3)
        disp('less than 3 points');
        k
    else
        % step 1, calculate all distance
        d = [];
        for i = 1 : length(x)-1
            for j = i+1 : length(x)
                dist = (x(i) - x(j))^2 + (y(i) - y(j))^2;
                d = [d; i, j, sqrt(dist)];
            end
        end
        
        
        % Step2, calculate center
        [dmax,indmax] = max(d(:,3));
        [dmin,indmin] = min(d(:,3));
        actdmax = dmax * pix2act;
        flag = 0;
        for i = 1 : length(actmax)
            if (abs(actdmax - actmax(i))<threshold)
                ca = i;
                flag = 1;
            end
        end
        if(flag == 0)
            disp('error measuring; cannot calculate');
            k 
        else
            if (ca == 1) % not missing point 1 or point 4
                p1 = [x(d(indmax,1)), y(d(indmax,1))];
                p4 = [x(d(indmax,2)), y(d(indmax,2))];

                % step 2, find the center, calculate vector
                center = 0.5 * (p1 + p4);
                c_wii = [1024, 768] - center;
                plot(c_wii(1), c_wii(2), 'bo')
%                 plot(x, y, 'r*')

                % step 3, find the upper star  % Not sure if we need it
                vx = x - center(1);
                vy = y - center(2);
                flag = 0;
                q = 1;
                while(flag == 0)
                    i = d(indmax, 1);
                    j = d(indmax, 2);
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
                dir_wii = [-vx(up), vy(up)];
                dir_wii = dir_wii / sqrt(dir_wii*dir_wii');
                pointer = c_wii + 70*dir_wii;
                arr_x = [c_wii(1), pointer(1)];
                arr_y = [c_wii(2), pointer(2)];
                drawArrow(arr_x, arr_y,{'MaxHeadSize',2})
            end
            
           
        end
    end
    pause(0.0001)
end    