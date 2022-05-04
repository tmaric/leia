# leia 

The "leia" project is an [OpenFOAM](https://develop.openfoam.com/Development/openfoam) module that implements unstructured Lagrangian / Eulerian Interface (LEIA) methods for multiphase flow simulations in complex geometries developed for the LEIA lecture at TU Darmstadt.

## Author

* **Tomislav Maric** [MMA, TU Darmstadt](https://www.mma.tu-darmstadt.de/index/mitarbeiter_3/mitarbeiter_details_mma_43648.en.jsp)

## Publications 

## License

This project is licensed under the GPL3.0 License - see the [LICENSE.md](LICENSE.md) file for details.

## Installation

These instructions will get your copy of the project up and running on your local machine for development and testing purposes. 

`leia` is a project that builds on [OpenFOAM](https://develop.openfoam.com/Development/openfoam) so it compiles and links against OpenFOAM libraries.  

### Compilation & Installation dependencies 

A C++ compiler that supports the C++20 standard, tested with 

* g++ (GCC) 11.2.0
* g++ (GCC) 10.3.0

#### Computing dependencies

Meshing 

* [cfmesh](https://cfmesh.com/cfmesh/), available as OpenFOAM sub-module, used for automatic generation of unstructured volume meshes

OpenFOAM

`argo` is based on OpenFOAM, git tag OpenFOAM-v2112

To install OpenFOAM follow the [instructions on installing OpenFOAM from sources](https://develop.openfoam.com/Development/openfoam/). 

1. Check out openfoam using git. 
2. Check out the git tag and build

```
    ?> git checkout OpenFOAM-v2112
    ?> ./Allwmake
```

#### Post-processing dependencies

We use [Jupyter notebooks](https://jupyter.org/) for visualization and processing of test results, and following packages (may be differently named on your Operating System) 

* python, python-pandas, python-numpy, python-jupyter

#### Compilation 

Once OpenFOAM is installed, run

```
leia> ./Allwmake  
```

## How to setup and run example cases

The template case used for advection verification is in `cases/testAdvection`. The case is set up for the 3D deformation advection. The case is a unit domain box discretized with 100x100x100 finite volumes, so serial execution is relatively slow. You can change the mesh resolution in `testAdvection/system/blockMeshDict`.  

### Serial execution

Create the mesh and initialize the fields with 

```
testAdvection> ./Allrun.init.serial
```

run the advection solver in serial  

```
testAdvection> leiaLevelSetFoam  
```

### Parallel execution

Create the mesh and initialize the fields with 

```
testAdvection> ./Allrun.init.parallel
```

run the advection solver in parallel 

```
testAdvection> mpirun -np 4 leiaLevelSetFoam -parallel 
```

### Alternative advection verification cases  

The velocity field for an alternative advection case can be set in `testAdvection/system/fvSolution`

```
velocityModel
{
    type deformation3D; 
    oscillation on;
}
```

Enter `test` ater `type` to get a list of available velocity models. Initial interface shape is also set in `testAdvection/system/fvSolution`


```
levelSet
{
    implicitSurface 
    {
        type implicitSphere; 
        center (0.35 0.35 0.35); 
        radius 0.15;
    }

    ...
```

Enter `test` after `type` to get a list of available types and their parameters.

## Contributing

Fork the project and submit a merge request.

