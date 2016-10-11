clc;clear all;
load('mWii_training_data_v2/mWii_training_data/A.mat');
data = rawStarData;
figure(1)
hold on
axis([1 1024 1 768])
for i = 1 : size(data)
    plot(data(i,1),data(i,5),'r*')
    plot(data(i,2),data(i,6),'g*')
    plot(data(i,3),data(i,7),'b*')
    plot(data(i,4),data(i,8),'y*')
    pause(0.001)
end