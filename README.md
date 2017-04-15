# Rapport de projet

## Introduction

Ce projet est réalisé dans le cadre du MODAL INF473I.
Mes remerciements à M. KERIVEN pour la conduite du cours et le suivi de projet.
Il s'agit d'un projet réalisé en 8 semaines sur la thématique du traitement d'image. La seule bibliothèque utilisée est la version C++ de [OpenCV](http://opencv.org/) disponible en license BSD.
Les vidéos utilisées dans les tests sont des extraits aux droits réservés, qui ne sont utilisés qu'à des fins personnelles et expérimentales, et non de divertissement.

**Objectif :** Expérimenter des méthodes de tracking en essayant de suivre une balle dans un extrait de match sportif brut, c'est à dire sans utiliser une caméra slow motion, 3D, HD, etc.

## Usage
Pour générer un build pour le projet, utilisez le générateur de build [CMake](https://cmake.org/) avec votre suite de compilation préférée.
Vous pouvez ensuite lancer le projet en donnant en paramètre le nom de l'input à charger. Cela correspond au fichier texte ayant le même nom, dans le dossier `./inputs/`. Les inputs existants sont pour l'instant `foot1`, `foot2`, et `tennis`.

Une fois le programme lancé, vous pouvez utiliser les commandes suivantes :

 * `echap` pour quitter le programme
 * `espace` pour lancer la vidéo en continu
 * `n` pour avancer frame par frame
 * `-` ou `+` pour changer la vitesse
 * `clic gauche` pour indiquer au programme la position de la balle, à tout moment.

## Démarche scientifique

### Organisation du code
Au travers de mes recherches, je me suis rapidement rendu compte des difficultés que je rencontrais, et de la nécessité d'utiliser plusieurs méthodes. C'est la raison pour laquelle j'ai créé une base commune au programme, avec de multiples implémentations pour chacune des méthodes.

* La gestion des inputs du programme a lieu dans le fichier `main.cpp`
* L'implémentation de base des techniques est la `Scene` (fichiers `scene.hpp`, `scene.cpp`). Sa fonction est de charger la vidéo, et d'interagir avec les inputs utilisateurs sur la lecture.
* Les diverses implémentations ont lieu via les classes suivantes :
 * LktScene, par méthode Lucas-Kanade (`lkt_scene.cpp`, `lkt_scene.hpp`)
 * RoiScene, par région d'intérêt (`roi_scene.cpp`, `roi_scene.hpp`)
 * CCScene, par calcul de contour (`ccontour_scene.cpp`, `ccontour_scene.hpp`)

L'input donné au `main` est un fichier texte qui contient diverses informations. Sur la première ligne figure le nom du fichier vidéo à charger. La seconde ligne contient l'identifiant de la méthode à utiliser. Les lignes suivantes contiennent diverses informations spécifiques à chaque implémentation.

### La suppression de fond

Une méthode classique pour repérer les éléments se déplaçant dans une image est la suppression du fond. Il existe diverse techniques, dont la plupart utilise les premières images d'une vidéo pour en déduire un fond, qu'elles vont ensuite pouvoir supprimer en temps réel.

Le problème de cette méthode est qu'elle ne fonctionne qu'avec des images provenant d'une camera fixe, ou, à la limite, ayant un mouvement homographique de faible intensité. Dans notre cas, le mouvement est relativement important, et est en plus 3D, ce qui fausse tout bonnement les algorithmes de suppression de fond.

Je n'ai donc pas pu utiliser cette méthode pour isoler la balle.

### La méthode LKT
La méthode LKT est la seconde technique que j'ai tenté d'utiliser. Il s'agit de la méthode Lucas-Kanade Tracking.

Cette méthode calcule le flux optique d'un pixel. Pour cela, on suppose que le flux est faible et constant sur un voisinage du pixel, ce qui permet grandement de simplifier les calculs.

Pour obtenir une méthode de tracking, il suffit d'utiliser cet algorithme en calculant le flux optique autour du point concerné, pour en déduire la position du pixel dans l'image suivante.

**Avantages :** Cette méthode s'est avérée efficace lorsqu'elle est appliquée à des points-clés qui vérifient bien l'hypothèse faite sur le flux optique. Cela peut être par exemple les bords du terrain, des tribunes, etc. On pourra ainsi en déduire le mouvement de la caméra.

**Inconvénients :** Pour les objets qui bougent rapidement, la méthode ne fonctionne pas du tout. Cela est dû à plusieurs raisons :

* Les calculs sont faussés par l'invalidité de l'approximation
* De plus, la qualité d'image pour un objet en mouvement est très faible. La couleur des pixels change énormément d'une image à l'autre, à moitié mélangée avec la couleur de fond. La forme de l'objet est aussi déformée à cause de sa vitesse.

Très souvent, le pixel prédit bouge au sein de la balle, jusqu'à parfois en sortir, et donner un mauvais suivi par la suite.

*Bilan :* Cette technique peut être utilisée pour suivre le mouvement relatif du terrain à la caméra, mais pas celui du ballon.

### Le filtre de Kalman

Afin de pallier aux défauts de la méthode précédente, il m'a fallu penser à une méthode différente. La première chose que l'on peut faire, c'est essayer d'anticiper sur le mouvement de la balle. En effet, dans la méthode précédente, on ne tient compte que de l'évolution d'une image à l'autre, mais pas de la dynamique que peut suivre un objet. Une méthode par prédiction nous permettrait d'anticiper la position où l'on peut espérer trouver la balle.

Un algorithme implémentant une telle méthode existe : il s'agit du filtre de Kalman.
Il est composé de deux étapes distinctes :

* La prédiction, durant laquelle, à partir de l'état actuel et de la matrice de transition (celle décrivant la dynamique, donc), l'algorithme donne une prédiction de la position de la balle.
* La correction : à partir de la position mesurée, il corrige sa prédiction, et donne ainsi une position réelle probable de l'objet.

### Méthode par région d'intérêt

En revanche, pour pouvoir appliquer cet algorithme, il est nécessaire d'avoir une méthode pour mesurer la position de la balle à chaque frame.

L'idée de cette seconde méthode est d'utiliser la prédiction effectuée par le filtre de Kalman pour isoler une région d'intérêt de l'image. 
On commence par sélectionner le channel de couleur correspondant à la couleur du fond, puis appliquer un threshold. Cela nous permet ainsi de récupérer les formes présentes dans la région d'intérêt.

A l'aide d'une détection de contour, on peut ainsi isoler la balle et effectuer le suivi.

**Démonstration :**
*Input :* type ROI, avec sur la première ligne le channel et le seuil minimal du threshold

![Démonstration](https://media.giphy.com/media/6vx8RvFNrSsgg/giphy.gif)

Le point jaune correspond à la mesure de la position de la balle, que l'on peut clairement identifier sur la région d'intérêt qui apparaît ici en threshold. Le point rouge, au centre de la région d'intérêt, correspond à la position prédite de la balle.

**Avantages :** Par rapport à la méthode précédente, on ne procède plus en suivant un pixel, mais en suivant la balle de manière globale. Ainsi il n'y a plus de risque de perdre le suivi. Si la position prédite n'est pas parfaitement précise, la balle, elle, est toujours proche et identifiable.
De plus, travailler avec des contours et des formes permet d'identifier le moment où la balle s'approche d'un autre objet non identifié (par exemple un joueur), et où le suivi peut être compromis. La version précédente ne le permettait pas et se contentait de continuer un suivi erroné.

**Inconvénients :** La gestion locale rend difficile le suivi lorsque l'on arrive à proximité d'un joueur, on ne sait plus quelle forme suivre.

### Analyse des formes

La dernière version à laquelle je suis arrivé est inspirée de la précédente, mais cette fois ci, au lieu d'isoler une zone d'intérêt, nous allons étudier la scène de manière globale. On utilise la même technique, à savoir sélectionner un channel et effectuer un threshold, puis une détection des contours à fin d'obtenir une liste des formes fermées isolées du terrain. L'idée est ensuite de récupérer la forme qui représente le plus le ballon en leur attribuant un score.

J'ai pour cela effectué une combinaison linéaire des paramètres suivants :

* La circularité de la forme (ratio aire/périmètre² proche de 1/4π)
* Son périmètre
* Son aire
* Sa distance à la position prédite

Si le score maximal n'est pas suffisant, on peut simplement passer l'étape de mesure. Cela permet de continuer la dynamique du filtre de Kalman, plutôt que l'induire en erreur. Par l'aspect global de la méthode, il est probable que, lorsque la balle réapparaisse clairement, l'algorithme soit capable de l'identifier.

**Démonstration :**
*Input :* type CC, avec sur la seconde ligne, l'aire, le périmètre, et la circularité (en pourcent) de la balle.

![Démonstration](https://media.giphy.com/media/CtPrBCJeBjeU0/giphy.gif)

Les points jaunes et rouges sont les mêmes que précédemment. Les zones en bleues sont les formes identifiées par le threshold, et la zone à la bordure épaisse est celle de score maximal.

**Avantages :** Cette méthode permet, dans certains cas, de retrouver la balle même lorsqu'elle passe par un joueur. En effet, la vision globale permet à la fois de trouver la balle en ce qu'elle est proche de l'endroit prédit, mais elle permet aussi de la localiser lorsque les estimations sont faussées, grâce à sa description qui est celle qui match le mieux.

**Inconvénients :** Il existe des parties de joueurs qui se trouvent, dans certains cas, être en tout point semblables à la balle, et qui trompent donc l'algorithme.

## Conclusion

J'ai pu découvrir au fil de mes recherches différentes méthodes de suivi d'objet dans une image, leurs avantages, leurs inconvénients, et leurs pistes d'amélioration.
Ainsi, j'ai pu notamment réaliser à quel point la tâche pouvait être difficile quand de bonnes conditions ne sont pas réunies : images compressées, balles rendues ovales, floues, par la vitesse, parfois cachées par les joueurs pouvant avoir la même couleur, etc.

Les deux pistes d'améliorations auxquelles j'ai songé sont :

* Utiliser un device adapté. On peut utiliser des caméras à profondeur de champ, avec une meilleure définition d'image, ou encore des émetteurs et récepteurs infrarouge (méthode sans traitement d'image)
* Améliorer l'algorithme précédent par le machine learning. On sait qu'en matière de reconnaissance d'objet, et donc de suivi, ils sont ceux qui donnent les meilleurs résultats. A l'image de ma dernière version, où je calcule un score en me basant sur une combinaison linéaire de paramètres préalablement sélectionnés, et dont les coefficients sont fixés à l'avance, ces derniers arrivent à deviner par eux-mêmes quels sont les paramètres et coefficients cohérents pour reconnaître un objet.
