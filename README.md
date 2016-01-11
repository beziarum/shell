Projet de Programmation Système

  Par Antoine Borde, Paul Beziau, Martin Bazalgette
  Pour le cours de programmation système de l'université de Bordeaux 1.
  
  
I) Présentation

Le projet consistait à implémenter un shell.
Le shell est exécutable en mode local ou en mode distant.
En mode local, le shell permet d'exécuter les même commandes que le shell bash 
Certaines commande ont été récrites et peuvent donc différer légèrement de celles de bash.
En mode distant, on bénéficie des fonctionnalités du shell en mode local, 
avec également la possibilité d'ouvrir et de controler notre shell sur une machine distante, 
via une connexion ssh.

II) Exécution 

On utilisera la commande make pour compiler l'integralité des fichiers.
Pour lancer le shell en mode local, on entrera simplement ./Shell
Pour le lancer en mode distant, on entrera cette fois ./Shell -r
