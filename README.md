# Projet Master2 informatique ID3D
# MMV: Modélisation de monde Virtuel

<img src=".\WIKI\images\1.JPG" alt="Image APP" style="zoom:100%;" />

## Installation et compilation

Télécharger [Gkit2light](https://forge.univ-lyon1.fr/Alexandre.Meyer/gkit2light) 

Suivre les étapes d'installation de Gkit2light pour l'IDE et l'OS voulut. [Lien](http://perso.univ-lyon1.fr/jean-claude.iehl/Public/educ/M1IMAGE/html/group__installation.html)

Ajouter le dépôt de ce projet à la racine de Gkit2light

Ajouter `include "Gkit2light_id3d_mmv"` dans `premake4.lua` à la racine, puis utiliser Premake (exemple: `premake5.exe vs2019`).



## Analyse et Modification du terrain

[Rapport](./WIKI/Rapport/MMV_TP1_Rapport.pdf)

### Analyse

Aire de drainage

<img src=".\WIKI\images\aire_drainage.JPG" alt="aire_drainage" style="zoom:50%;" />



Slope

<img src=".\WIKI\images\slope.JPG" alt="slope" style="zoom:50%;" />



Laplacien

<img src=".\WIKI\images\laplacien.JPG" alt="laplacien" style="zoom:50%;" />



Wetness Index

<img src=".\WIKI\images\wetness.JPG" alt="wetness" style="zoom:50%;" />



Shading

<img src=".\WIKI\images\shading.JPG" alt="shading" style="zoom:50%;" />



### Modification

Breaching terrain

|                       Before Breaching                       |                Convergence smoothed breaching                |
| :----------------------------------------------------------: | :----------------------------------------------------------: |
| <img src=".\WIKI\images\breaching\0.JPG" alt="0.JPG" style="zoom: 50%;" /> | <img src=".\WIKI\images\breaching\smoothedBreaching_convergence.JPG" alt="smoothedBreaching_convergence" style="zoom: 50%;" /> |



Smooth Terrain

<img src=".\WIKI\images\smooth\progressif.png" alt="progressif" style="zoom: 50%;" />



Erosion Paramétrer

<img src=".\WIKI\images\erosion\progressif.jpg" alt="progressif" style="zoom:50%;" />



## Végétations

[Rapport](./WIKI/Rapport/MMV_TP2_Rapport.pdf)

<img src=".\WIKI\images\5.JPG" alt="5.JPG" style="zoom:50%;" />


## Utiliser

- [Gkit2light](http://perso.univ-lyon1.fr/jean-claude.iehl/Public/educ/M1IMAGE/html/index.html) Moteur de jeu léger en C++ OpenGL

- [ImGui](https://github.com/ocornut/imgui) Interfaces graphiques

