# Introduction
Ce document à pour but de rendre compte des recherches effectuées sur les algorithmes et frameworks de recherche de chemin et/ou de navigation en général.

# ArduPilot

ArduPilot est un système de pilotage automatique fiable, polyvalent et open source prenant en charge de nombreux types de véhicules : multicoptères, hélicoptères traditionnels, avions, bateaux, sous-marins, rovers, etc. Le code source est développé par une large communauté de professionnels et de passionnés. Les nouveaux développeurs sont toujours les bienvenus ! La meilleure façon de commencer est de rejoindre le forum de l'équipe des développeurs, qui est ouvert à tous et qui regorge d'informations sur le développement quotidien.[^1]

Sources intéressantes :
- https://www.instructables.com/Building-a-Self-Driving-Boat-ArduPilot-Rover/
- https://ardupilot.org/rover/docs/boat-configuration.html

# PID

Pourquoi pas utiliser le système de PID ? Simple, mais ne promet pas d'avoir le chemin le plus efficace.

Regarder ce qui à été fait au niveau de opentransat : https://github.com/OpenTransat/OpenTransat-Navigator/blob/master/software/main/navigate.ino

    
[^1]: https://ardupilot.org/
[^2]: https://link.springer.com/article/10.1007/s13437-019-00172-0