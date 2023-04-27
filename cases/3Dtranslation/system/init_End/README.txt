This is an empty support case for initialisation of the field psiEnd.

Goal: Have the psiEnd field in the 0/ directory of the original case.

Idea:
- call ´leiaSetfield -case system/init_End.sh´ to init a psi field according to system/init_End/system/fvSolution
- in fvSolution the implicitSphere is located at the exact end position of the 3Dtranslation test case.
- the psiEnd, alphaEnd fields change according to the mesh resolution. Because of that psiEnd, alphaEnd get initialised each time according to the Allrun scripts.
- Allrun scripts link the created mesh into the empty support case system/init_End, calls leiaSetFields on that and moves system/init_End/0/{psi,alpha} to 0/{psiEnd,alphaEnd}

