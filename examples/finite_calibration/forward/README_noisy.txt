#1. perturber
usage: perturber <geom.dmg> <mesh.smb> <num steps> <seed> <random factor> <outmesh.smb>

#1.  Adding noise
sk63837@ase-d22442:~/calibr8/flavor/linux-shared/build/calibr8/test/finite_calibration/forward
$ perturber /home/sk63837/calibr8/source/calibr8/test/mesh/notch2D_Asym/notch2D_Asym.dmg notch2D_Asym_finite_J2_plane_stress_synthetic/ 4 22 1e-4 noisy/



#2. Visualization
sk63837@ase-d22442:~/calibr8/flavor/linux-shared/build/calibr8/test/finite_calibration/forward
$ render
Usage: render <model> <mesh> <out prefix>
sk63837@ase-d22442:~/calibr8/flavor/linux-shared/build/calibr8/test/finite_calibration/forward$ render /home/sk63837/calibr8/source/calibr8/test/mesh/notch2D_Asym/notch2D_Asym.dmg noisy/ noisy_viz



