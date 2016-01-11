Projet de Programmation Système

  Par Antoine Borde, Paul Beziau, Martin Bazalgette
  Pour le cours de programmation système de l'université de Bordeaux 1.
  
  
I) Présentation

Le projet consistait à implémenter un shell.
Le shell est exécutable en mode local ou en mode distant.
En mode local, le shell permet d'exécuter les commandes de la même façon que bash.
Certaines  commandes (hostname, echo, etc…) ayant été réecrite, leur comportement
peut légèrement différer qu'avec bash.
En mode distant les fonctionalitées sont les mêmes cepandant la gestion de l'historique
est désactivé et le prompt n'es pas affiché, afin de faciliter l'utilisation via une
machine distante

II) Exécution 

On utilisera la commande make pour compiler l'integralité des fichiers.
Pour lancer le shell en mode local, on entrera simplement ./Shell
Pour le lancer en mode distant, on entrera cette fois ./Shell -r
