# leia 

The "leia" project is an [OpenFOAM](https://develop.openfoam.com/Development/openfoam) module that implements unstructured Lagrangian / Eulerian Interface (LEIA) methods for multiphase flow simulations in complex geometries.

![Build Tests](https://github.com/tmaric/leia/actions/workflows/ci.yml/badge.svg)

## Author

* **Tomislav Maric** [ORCID](https://orcid.org/0000-0001-8970-1185)
* **Julian Reizel** [ORCID](https://orcid.org/0000-0002-3787-0283)

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

OpenFOAM-v2206

To install OpenFOAM follow the [instructions on installing OpenFOAM from sources](https://develop.openfoam.com/Development/openfoam/). 

1. Check out openfoam using git. 
2. Check out the git tag and build

```
    ?> git checkout OpenFOAM-v2206
    ?> ./Allwmake
```

#### Compilation 

Once OpenFOAM is installed, run

```
leia> ./Allwmake  
```

## How to setup and run example cases

The template cases used for advection verification are located in `cases/`.   

### Serial execution


```
 case> ./Allrun_hex_serial.sh
```

### Parallel execution

Create the mesh and initialize the fields with 

```
case> ./Allrun_hex_parallel.sh
```

### Level Set Advection  

The velocity field for an alternative advection case can be set in `3Ddeformation/system/fvSolution`

```
velocityModel
{
    type deformation3D; 
    oscillation on;
}
```

Enter `test` ater `type` to get a list of available velocity models. Initial interface shape is also set in `3Ddeformation/system/fvSolution`


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

#### Parameter studies

To run parameter studies, we use the pyFoamStudy submodule, which is a pip-installable package of scripts built on top of pyFoam.

#### Postprocessing and visualization

Currently, pyFoamStudy also includes scripts to agglomerate a study's postprocessing data into a CSV database and a script to visualize the CSV database.

## Contributing

Fork the project and submit a merge request.

