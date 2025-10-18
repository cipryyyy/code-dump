clc, clearvars, close all

l = -4;
r = 4;
magnitude = 0.1;

x = linspace(l, r, 1000);
y = exp(-x.^2);
noise = ((2)*rand(1,length(y))-1)*magnitude;
y = y + noise;

figure(1)
hold on

plot(x,y)
plot(x,noise, "r--")

window = 20;
b = (1/window) * ones(1,window);

y = filter(b,1,y);
plot(x,y, "LineWidth", 2);

title("Filter a random signal"), xlabel("time"), ylabel("read")
subtitle(strcat("Randomness: ", num2str(magnitude*100)) + strcat("%, Window size: ", num2str(window)))
legend("input", "noise", "filtered")
xlim([l r]), ylim([0 inf]), grid on
