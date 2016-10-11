clc;clear all;
load('mWii_training_data_v2/mWii_training_data/C.mat');
drawArrow = @(x,y,props) quiver( x(1),y(1),x(2)-x(1),y(2)-y(1),0, props{:} );
figure(1)
clf
hold on
axis([-1024 1024 -768 768])
xlabel('x-axis');
ylabel('y-axis');
actmax = [29; 25.99; 23.08];
% actmax = [all 4 or miss 2,3; miss 4; miss 1]
pi = 3.14;
threshold = 1.5;
pix2act = 29/99.40;

P1 = [0, -14.5] / pix2act;
P2 = [-10.563, 2.483] / pix2act;
P3 = [11.655, 8.741] / pix2act;
P4 = [0, 14.5] / pix2act;

plot([P1(1) P2(1) P3(1) P4(1)], [P1(2) P2(2) P3(2) P4(2)], '*')

arr_wx = [1, 100];
arr_wy = [1, 100];
drawArrow(arr_wx,[1, 1],{'MaxHeadSize',2});
drawArrow([1, 1], arr_wy,{'MaxHeadSize',2});

for  k = 2 : size(rawStarData, 1)
    data = rawStarData(k,:);
    x = [data(1) data(2) data(3) data(4)];
    y = [data(5) data(6) data(7) data(8)];
    x(4) = [];
    y(4) = [];
    % pre-process, get rid of noise
    for i = 1 : length(x)
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
            % find coincide point, determine reference vector
            for i = 1 : 2
                for j = 1 : 2
                    temp = d(indmax,i);
                    if (d(indmin, j) == temp)
                        coin_ind = temp;
                        other_ind = d(indmax, 3-i);
                    end
                end
            end          
            if (ca == 1) % not missing point 1 or point 4
                ref_pw = [0, -14.5]/pix2act;
                ref_vw = [0, 1];
            else
                if(ca == 2) % miss point 4
                    disp('miss point 4');                   
                    ref_pw = [0, 14.5]/pix2act;
                    ref_vw = [0.4483, -0.8939];
                    
                else % miss point 1
                    disp('miss point 1');
                    ref_pw = [11.655, -8.741]/pix2act;
                    ref_vw = [-0.9625, 0.2711];
                end
            end
                            
            % calculate rotation matrix
            ref_pc = [x(coin_ind), y(coin_ind)];
            ref_vc = [x(other_ind), y(other_ind)] - ref_pc;
            ref_vc = ref_vc / sqrt(ref_vc*ref_vc');
            costheta = ref_vc * ref_vw';
            sintheta = sqrt(1 - costheta^2);
            R1 = [costheta, -sintheta;sintheta,costheta]; % sin > 0
            R2 = R1';   % sin < 0
            temp1 = norm(R1*ref_vc' - ref_vw');
            temp2 = norm(R2*ref_vc' - ref_vw');
            if temp1 < temp2
                R = R1;
            else
                R = R2;
            end
            c_wii =  R * [512 - ref_pc(1); 384 - ref_pc(2)] + ref_pw';
            plot(c_wii(1), c_wii(2), 'bo')
            
            dir = R*[0, 1]';
            pointer = c_wii + 100*dir;
            arr_x = [c_wii(1), pointer(1)];
            arr_y = [c_wii(2), pointer(2)];
            drawArrow(arr_x, arr_y,{'MaxHeadSize',2, 'Color',[1,0,0]})
        end
    end
    pause(0.0001)
end    