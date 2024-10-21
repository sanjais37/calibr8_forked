%  sum the load at step
clear all
close all
clc
format long
load_data = load("element_loads.dat");

no_elm = 4566;
no_load_steps =4;

load=zeros(no_load_steps,1);

for i=1:no_load_steps

    start = (i-1)*no_elm +1
    end_id = i*no_elm;

    load(i) = sum(load_data(start:end_id,2))
end

load_1 = load_data(1:no_elm,2);

load_nonZero = load_1(load_1 ~= 0.0);

n_nonzero = numel(load_nonZero)