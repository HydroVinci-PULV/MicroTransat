On va envoyer les coordonnées GPS du bateau en coordonnées décimal : format [latitude, longitude].

Deux possibilités pour la précision:
- ±11,1m -> 0.0001 -> équivalent à pouvoir pointer précisement l'un des quatres coins d'une maison sur une carte.
- ±111m -> 0.001 -> équivalent à pouvoir pointer un paté de maison sur une carte.

## Exemple :
Coordonnées de l'ESILV : [48,896460; 2,236340]

- [48,8965; 2,2363] précision 10<sup>-4</sup>
- [48,897; 2,236] précision 10<sup>-3</sup>

Prenons 10<sup>-3</sup> : 

|  Format  |    Données     |     Taille     |                             Commentaire                              |
| :------: | :------------: | :------------: | :------------------------------------------------------------------: |
| `String` | `48.897,2.236` | 12 o = 96 bits |                                 `NA`                                 |
| `String` |  `48897,2236`  | 10 o = 80 bits | On convertit les valeurs en entier en multipliant par 10<sup>3</sup> |

Avec cette simple multiplication à a récupérer 2 octets sur notre transmission.

## Réduction de la taille occupée par les données

Les latitudes et longitutes que le bateau pourra occuper au cours de son trajet sont contraintes par la partie la plus à l'ouest de la côte est du continent américain (oui c'est technique), l'est de la France, le nord du Canada et le sud du Chili.

Ainsi la longitude est comprise entre la côte mexicaine du golfe du Mexique (~ -98°) et Strasbourg (~ 8°).

La latitude est comprise entre l'île Bylot (nord Canada) (~ 74°) et le cap Horn (~ -55°).

On a donc les intervals suivants: 
- latitude : [-55, 74]
- longitude : [-98, 8]

A ce niveau de précision (10<sup>-3</sup>) la latitude peut donc être convertie en un entier appartenant à l'interval [-55000, 74000] soit 129000 possibilités et la longitude en un entier appartenant à l'interval [-98000, 8000] soit 106000 possibilités.

Pour stocker un entier compris entre 0 et 129000 (on va ajouter un padding à notre valeur pour qu'elle soit toujours positive) on a besoin de 17 bits. Pareil pour les 106000 possibilités de longitude.

On a donc pour but de caser ces 34 bits dans des octets (8 bits) :

Un simple calcul nous indique qu'il nous faudra 5 octets pour stocker (avec beaucoup de marge malheureusement) les coordonnées GPS.

|       |       |       |       |       |       |       |       |
| ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- |
| `LAT` | `LAT` | `LAT` | `LAT` | `LAT` | `LAT` | `LAT` | `LAT` |
| `LAT` | `LAT` | `LAT` | `LAT` | `LAT` | `LAT` | `LAT` | `LAT` |
| `LON` | `LON` | `LON` | `LON` | `LON` | `LON` | `LON` | `LAT` |
| `LON` | `LON` | `LON` | `LON` | `LON` | `LON` | `LON` | `LON` |
|       |       |       |       |       |       | `LON` | `LON` |

On est ainsi passé de 12 octets (voir 15 dans le pire des cas) à 5 octets pour transmettre les coordonnées GPS.