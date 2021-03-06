/**
 * @author      Christoph Schaefer cm.schaefer@gmail.com and Thomas I. Maindl
 *
 * @section     LICENSE
 * Copyright (c) 2019 Christoph Schaefer
 *
 * This file is part of miluphcuda.
 *
 * miluphcuda is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * miluphcuda is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with miluphcuda.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef _PARAMETER_H
#define _PARAMETER_H

// helper definitions
#define SPH_VERSION1 1
#define SPH_VERSION2 2
// Dimension of the problem
#define DIM 3
#define DEBUG 0

// add additional point masses to the simulation, read from file <filename>.mass
// format is location velocity mass r_min r_max feeling-flag, where location and velocity are vectors with size DIM, r_min/r_max are min/max distances of SPH particles
// to the bodies before they are taken out of the simulation, and feeling-flag specifies whether point masses shall feel gravity of SPH particles or not (1/0)
#define GRAVITATING_POINT_MASSES 0

// sink particles (set point masses to be sink particles)
#define PARTICLE_ACCRETION 0 // check if particle is bound to one of the sink particles (crossed the accretion radius, rmin); if also UPDATE_SINK_VALUES 1: particle is accreted and ignored afterwards, else: continues orbiting without being accreted
#define UPDATE_SINK_VALUES 0 // add to sink the quantities of the accreted particle: mass, velocity and COM

// integrate the energy equation
// when setting up a SOLID simulation with Tillotson or ANEOS, it must be set to 1
#define INTEGRATE_ENERGY 1

// integrate the continuity equation
// if set to 0, the density will be calculated using the standard SPH sum \sum_i m_j W_ij
// Note that the input file format (density column) changes with this option (check with -Y flag).
#define INTEGRATE_DENSITY 1

// basic physical model:
// SOLID set to 1 solves continuum mechanics with material strength, and stress tensor \sigma^{\alpha \beta} = -p \delta^{\alpha \beta} + S^{\alpha \beta}
// SOLID set to 0 solves only the Euler equation, and there is only (scalar) pressure
#define SOLID 1

// adds viscosity to the Euler equation
#define NAVIER_STOKES 0
// choose between two different viscosity models
#define SHAKURA_SUNYAEV_ALPHA 0
#define CONSTANT_KINEMATIC_VISCOSITY 0

// damage model following Benz & Asphaug (1995)
// this needs some preprocessing of the initial particle distribution since activation thresholds have to be distributed among the particles
#define FRAGMENTATION 1

// SPH stuff
// here, you have to define which kind of SPH representation you want to solve the
// momentum and internal energy equation
// SPH_VERSION1: original version with HYDRO dv_a/dt ~ - (p_a/rho_a**2 + p_b/rho_b**2)  \nabla_a W_ab
//                                     SOLID dv_a/dt ~ (sigma_a/rho_a**2 + sigma_b/rho_b**2) \nabla_a W_ab
// SPH_VERSION2: slighty different version with
//                                     HYDRO dv_a/dt ~ - (p_a+p_b)/(rho_a*rho_b)  \nabla_a W_ab
//                                     SOLID dv_a/dt ~ (sigma_a+sigma_b)/(rho_a*rho_b)  \nabla_a W_ab
//  if you do not know what to do, choose SPH_VERSION1
#define SPHEQUATIONS SPH_VERSION1
// for the tensile instability fix
// you do not need this
#define ARTIFICIAL_STRESS 0

// standard SPH alpha/beta viscosity
#define ARTIFICIAL_VISCOSITY 1
// Balsara switch: lowers the artificial viscosity in regions without shocks
#define BALSARA_SWITCH 0

// INVISCID SPH (see Cullen & Dehnen paper)
#define INVISCID_SPH 0

// consistency switches
// for zeroth order consistency
#define SHEPARD_CORRECTION 1
// for linear consistency
// add tensorial correction tensor to dSdt calculation -> better conservation of angular momentum
#define TENSORIAL_CORRECTION 1

// plastic flow conditions
// you can choose between
// 1 simple von Mises plasticity with a constant yield strength ->
//          yield_stress =   in material.cfg file
// 2 Drucker Prager yield criterion -> yield strength is given by
//   the condition \sqrt(J_2) + A * I_1 + B = 0
//   with I_1: first invariant of stress tensor
//        J_2: second invariant of stress tensor
//        A, B: Drucker Prager constants
//              which are calculated from angle of internal friction and cohesion
//      in material.cfg: friction_angle =
//                       cohesion =
//  3 Mohr-Coulomb yield criterion -> yield strength is given by
//         yield_stress = tan(friction_angle) \times pressure + cohesion
//      in material.cfg: friction_angle =
//                       cohesion =
//  4 a pressure dependent yield strength following Gareth Collins' 2004 paper and
//   Martin Jutzi's implementation in his 2015 paper.
//          yield_stress is different for damaged and intact rock
//      first, the yield stress for intact rock y_i is given by
//      y_i =  cohesion + \mu P / (1 + \mu P/(Y_m - cohesion) )
//      where yield_stress is the yield stress for P=0 and Y_m is the shear strength at P=\infty
//      \mu is the coefficient of internal friction
//      the yield strength for (fully) damaged rock y_d is given by
//      y_d = \mu_d \times P
//      where \mu_d is the coefficient of friction of the *damaged* material
//      y_d is limited to y_d <= y_i
//      for this model, following parameters in material.cfg are obligatory
//          yield_stress = Y_M
//          cohesion =
//          friction_angle =
//  NOTE: units are: friction angle = rad
//                   cohesion = Pascal
//  if you do not know what this is, choose 1 or nothing

#define VON_MISES_PLASTICITY 1
//  WARNING: choose only one of the following three options
//  this will be fixed in a later version of the code
#define MOHR_COULOMB_PLASTICITY 0
#define DRUCKER_PRAGER_PLASTICITY 0
#define COLLINS_PRESSURE_DEPENDENT_YIELD_STRENGTH 1

// model regolith as viscous fluid -> experimental setup, only for powerusers
#define VISCOUS_REGOLITH 0
// use Bui model for regolith -> experimental setup, only for powerusers
#define PURE_REGOLITH 0
// use Johnson-Cook plasticity model -> experimental setup, only for powerusers
#define JC_PLASTICITY 0

// porosity models:
// P-Alpha model implemented following Jutzi (200x)
#define PALPHA_POROSITY 1        // pressure depends on distention
#define STRESS_PALPHA_POROSITY 0 // deviatoric stress is also affected by distention
//
// Sirono model modified by Geretshauser (2009/10)
#define SIRONO_POROSITY 0
// Epsilon-Alpha model implemented following Wuennemann
#define EPSALPHA_POROSITY 0

// constants
// maximum number of activation threshold per particle -> fixed array size, only needed for
// FRAGMENTATION. if not used, set to 1
#define MAX_NUM_FLAWS 32
// maximum number of interactions per particle -> fixed array size
#define MAX_NUM_INTERACTIONS 300

// gravitational constant in SI
#define C_GRAVITY_SI 6.67408e-11
// gravitational constant in AU
#define C_GRAVITY_AU 3.96425141E-14
// set G to 1
#define C_GRAVITY_SIMPLE 1.0

//Choose your fighter, beware of units !!
#define C_GRAVITY C_GRAVITY_SI

// sets a reference density for the ideal gas eos (if used) - 1ppm of that is used as DENSITY_FLOOR (if activated) of ideal gas
#define IDEAL_GAS_REFERENCE_RHO 1.0

// if set to 1 (and INTEGRATE_DENSITY is used), the floor density for all particles is 1e-6 of the lowest density in material.cfg
// note: see additionally boundary.cu with functions beforeRHS and afterRHS for boundary conditions
#define DENSITY_FLOOR 0

// set p to 0 if p < 0
#define REAL_HYDRO 0

// if set to 1, the smoothing length is not fixed for each material type
// choose either FIXED_NOI for a fixed number of interaction partners following
// the ansatz by Hernquist and Katz
// or choose INTEGRATE_SML if you want to additionally integrate an ODE for
// the sml following the ansatz by Benz and integrate the ODE for the smoothing length
// d sml / dt  = sml/DIM * 1/rho  \nabla velocity
// if you want to specify an individual initial smoothing length for each particle (instead of the material
// specific one in material.cfg) in the initial particle file, set READ_INITIAL_SML_FROM_PARTICLE_FILE to 1
#define VARIABLE_SML 1
#define FIXED_NOI 0
#define INTEGRATE_SML 1
#define READ_INITIAL_SML_FROM_PARTICLE_FILE 1

// if set to 0, h = (h_i + h_j)/2  is used to calculate W_ij
// if set to 1, W_ij = ( W(h_i) + W(h_j) ) / 2
#define AVERAGE_KERNELS 0

// important switch: if the simulations yields at some point too many interactions for
// one particle (given by MAX_NUM_INTERACTIONS), then its smoothing length will be set to 0
// and the simulation continues. It will be announced on *stdout* when this happens
// if set to 0, the simulation stops in such a case unless DEAL_WITH_TOO_MANY_INTERACTIONS is used
#define TOO_MANY_INTERACTIONS_KILL_PARTICLE 0
// important switch: if the simulations yields at some point too many interactions for
// one particle (given by MAX_NUM_INTERACTIONS), then its smoothing length will be lowered until
// the interactions are lower than MAX_NUM_INTERACTIONS
#define DEAL_WITH_TOO_MANY_INTERACTIONS 0

// additional smoothing of the velocity field
// hinders particle penetration
// see Morris and Monaghan 1984
#define XSPH 0

// boundaries EXPERIMENTAL, please do not use this....
#define BOUNDARY_PARTICLE_ID -1
#define GHOST_BOUNDARIES 0

// IO options
#define HDF5IO 1             // use HDF5 (needs libhdf5-dev and libhdf5)
#define MORE_OUTPUT 0        //produce additional output to HDF5 files (p_max, p_min, rho_max, rho_min); only ueful when HDF5IO is set
#define MORE_ANEOS_OUTPUT 0  // produce additional output to HDF5 files (T, cs, entropy, phase-flag); only useful when HDF5IO is set; set only if you use the ANEOS eos, but currently not supported for porosity+ANEOS
#define OUTPUT_GRAV_ENERGY 0 // compute and output gravitational energy (at times when output files are written); of all SPH particles (and also w.r.t. gravitating point masses and between them); direct particle-particle summation, not tree; option exists to control costly computation for high particle numbers
#define BINARY_INFO 0        // generates additional output file (binary_system.log) with info regarding binary system: semi-major axis, eccentricity if GRAVITATING_POINT_MASSES == 1

#endif
