# Bouncing Sphere
Projet de moteur physique de jeu avec une sphère qui rebondi sur des obstacles statiques.

## Utilisation
Chemin relatif du fichier de la solution : `./raylib.sln`.
Afin de pouvoir exécuter correctement le projet, il est nécessaire de **recibler la solution**, puis de définir le projet `BouncingSphere` comme **projet de démarrage**.

Au lancement, l'application arrive sur l'**écran d'accueil**.
On peut **démarrer** une scène de jeu en appuyant sur `Entrée`.
Une fois dans la scène de jeu, il est possible de **déplacer la caméra** (rotation) avec la souris en maintenant le clic gauche enfoncé.
Pour effectuer une **translation** de la caméra, on peut utiliser la touche `Ctrl` en plus du clic.
Le niveau de **zoom** (distance au point central visé) est réglable à partir de la molette.

Pour mettre en **pause**, utiliser la touche `Espace`. Lorsque la scène est en pause, il est tout de même possible de se déplacer pour avoir tous les angles de vue.
Pour mettre la fenêtre en **plein écran**, utiliser la touche `F1`.
Pour revenir à l'**écran d'accueil**, utiliser la touche `Echap`.

## Ressources

* Vidéo de présentation : `Bouncing Sphere - Jenny CAO & Théo SZANTO.mp4`
* Icône GitHub appartenant à GitHub.
* Effets sonores récupérés grâce à Quentin Zalio.

## Remarques
### Structure du code
Le code est structuré en 3 modules et le fichier principal :

* `Models.h / .cpp` : Modélisation mathématiques des objets, systèmes de coordonnées, référentiels.
* `Drawing.h / .cpp` : Méthodes de dessin des objets pour Raylib.
* `Utils.h / .cpp` : Méthodes utilitaires pour le code (et opérateurs surchargés).
* `BouncingSphere.cpp` : Programme principal

### Surcharge d'opérateurs (vecteurs et quaternions)
Pour simplifier l'écriture et la lecture de code, nous avons surchargé plusieurs opérateurs à propos des vecteurs et quaternions.
Ils se trouvent dans le fichier `Utils.h` :

* `Vector3Add` : `v1 + v2`
* `Vector3Scale` : `v * f` et `f * v` (commutatif)
* `Vector3Subtract` : `v1 - v2`
* `Vector3Negate` : `-v`
* `Vector3DotProduct` : `v1 * v2`
* `Vector3CrossProduct` : `v1 ^ v2`
* `Vector3Normalize` : `!v`
* `Vector3LengthSqr` : `~v` (attention : il s'agit ici de la norme au carré, utilisée beaucoup plus fréquemment dans le code, il n'y a pas d'opérateur pour la norme tout court)
* `Vector3Reflect` : `v / normal`
* `Vector3RotateByQuaternion` : `v * q` ou `q * v` (commutatif)
* `QuaternionMultiply` : `q1 * q2`

Deux opérateurs ont également été surchargés sur les structures `Referential` :

* `ref + vector` : Translation du référentiel par un vecteur (exprimé dans ce même référentiel)
* `ref * quaternion` : Rotation du référentiel par un quaternion

### Bugs connus
La cause du bug n'a **pas pu être identifiée** pour l'instant (les investigations n'ont pas réellement été poussées très loin...), mais il arrive que la **balle se coince** dans un obstacle et provoque un **crash de l'application** au bout d'un certain nombre de collisions successives détectées.
La **détection statique** de collision n'a pas été suffisamment développée pour correctement **repositionner** la balle, ce qui peut être une explication potentielle du bloquage.

### Voies d'amélioration
Bien sûr, **régler le bug** cité plus haut serait l'amélioration principale.
Il est possible d'améliorer également le **design** et l'**environnement** de la balle (qui n'est qu'une boîte avec des obstacles répartis équitablement dedans pour l'instant).

## Attribution
Le projet ayant été réalisé en quasi intégralité **en classe** (excepté quelques méthodes de dessin durant la période entreprise), les contributions visibles sur GitHub ne sont **pas représentatives**.
En effet, quasiment tout à été développé sur **un seul ordinateur**, donc les commits sont à un seul nom.

Les **systèmes de coordonnées** et la **modélisation des objets** (structures C) ont été réalisés **individuellement** puis mis en commun au début du projet.
Les premières méthodes d'**intersection** (segment / plan et sphère), ainsi que les méthodes de **dessin** associées (plan, sphère) ont été réalisées **ensemble** durant les séances de cours.
Les méthodes de **dessin partiels** (disque, cylindre et sphère) ont été développée par **Jenny**.
Les méthodes d'intersection avec les **cylindres** (infini, fini et aux coins arrondis) ont été commencées **en commun** (avant la période entreprise) et finies par **Théo**.
La gestion des **boîtes aux coins arrondis**, ainsi que les **collisions** ont été terminées **en commun** durant les séances de cours dédiées au projet.
Tous les petits extras non demandées initialement mais ajoutées par plaisir ont été développés par Théo :)
