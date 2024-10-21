% displacement error

clear all
close all
clc
format long

file1= load('notch2D_Asym_finite_J2_plane_stress_forward.csv');
file2= load('notch3D_Asym_finite_J2_surface_forward.csv');

x_2D_cood = file1(:,2);
y_2D_cood = file1(:,3);

x_3D_cood = file1(:,2);
y_3D_cood = file1(:,3)

u_2D_mod = file1(:,9);
u_3D_mod = file2(:,10);

u_2D_mod_max = max(u_2D_mod)
u_3D_mod_max = max(u_3D_mod)
u_max_diff = u_3D_mod_max - u_2D_mod_max

u_2D_mod_min = min(u_2D_mod)
u_3D_mod_min = min(u_3D_mod)
u_min_diff = u_3D_mod_min - u_2D_mod_min


nno_u_2D_mod = numel(u_2D_mod)
nno_u_3D_mod = numel(u_3D_mod)


u_diff = abs(u_3D_mod - u_2D_mod);
X_cood_diff = abs(x_3D_cood - x_2D_cood);
Y_cood_diff = abs(y_3D_cood - y_2D_cood);

X_cood_diff_norm = norm(X_cood_diff)
Y_cood_diff_norm = norm(Y_cood_diff)

figure(1)
 scatter(file1(:,2),file1(:,3),30, X_cood_diff, "filled")
 xlim([-0.3, 1.1])
 ylim([-0.2, 1.2])
 colormap("jet")
 axis equal 

 figure(2)
 scatter(file1(:,2),file1(:,3),30, Y_cood_diff, "filled")
 xlim([-0.3, 1.1])
 ylim([-0.2, 1.2])
 colormap("jet")
 axis equal  

figure(3)
 scatter(file1(:,2),file1(:,3),30, u_3D_mod, "filled")
 xlim([-0.3, 1.1])
 ylim([-0.2, 1.2])
 axis equal 

 figure(4)
 scatter(file1(:,2),file1(:,3),30, u_2D_mod, "filled")
 xlim([-0.3, 1.1])
 ylim([-0.2, 1.2])
 axis equal

 figure(5)
 scatter(file1(:,2),file1(:,3),30, u_diff, "filled")
 xlim([-0.3, 1.1])
 ylim([-0.2, 1.2])
 axis equal
%====================

