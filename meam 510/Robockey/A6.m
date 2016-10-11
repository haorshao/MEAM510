clc;clear all;
load('mWii_training_data_v2/mWii_training_data/A.mat');
drawArrow = @(x,y,props) quiver( x(1),y(1),x(2)-x(1),y(2)-y(1),0, props{:} );
figure(1)
clf
hold on
axis([1 1024 1 768])
% ratio = [2.23073553239764; 1.77555057037888; 1.81255218950991; 1.29998496140435];
% ratio = [all 4 or miss 2; miss 1; miss 3; miss 4]

actmax = [29; 25.99; 23.08];
% actmax = [all 4 or miss 2,3; miss 4; miss 1]

threshold = 1;
pix2act = 29/99.40;
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
    else
        % step 1, calculate all distance
        d = [];
        for i = 1 : length(x)-1
            for j = i+1 : length(x)
                dist = (x(i) - x(j))^2 + (y(i) - y(j))^2;
                d = [d; i, j, sqrt(dist)];
            end
        end

        [dmax,indmax] = max(d(:,3));
        
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
            
        else
            if (ca == 1) % not missing point 1 or point 4
                p1 = [x(d(indmax,1)), y(d(indmax,1))];
                p4 = [x(d(indmax,2)), y(d(indmax,2))];

                % step 2, find the center, calculate vector
                center = 0.5 * (p1 + p4);
                c_wii = [1024, 768] - center;
                plot(center(1), center(2), 'ro')
                plot(c_wii(1), c_wii(2), 'bo')
%                 plot(x, y, '*')
                pause(0.001)
            end
        end
        
        
        
%         % find case
%         [dmax,indmax] = max(d(:,3));
%         [dmin,indmin] = min(d(:,3)); 

%         flag = 0;
%         for i = 1 : length(ratio)
%             if (abs(dmax/dmin - ratio(i)) < threshold)
%                 ca = i;
%                 flag = 1;
%             end
%         end
% 
%         if(flag == 0)
%             disp('error measuring; cannot calculate');
%             k 
%         else
% 
%             if (ca == 1 || ca == 3) % not missing point 1 or point 4
%                 p1 = [x(d(indmax,1)), y(d(indmax,1))];
%                 p4 = [x(d(indmax,2)), y(d(indmax,2))];
% 
%                 % step 2, find the center, calculate vector
%                 center = 0.5 * (p1 + p4);
%             else
%                 disp('miss point 1 or point 4')
%             end
%         end
    end
end    