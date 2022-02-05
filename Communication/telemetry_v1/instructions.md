# Debug

pour activer les parties du code qui sont désactivées par `#ifdef DEBUG` compiler le programme avec `gcc -DDEBUG -o telemetry.exe telemetry.c telemetry_utils.c`

# Test Unitaire

Si le fichier de test s'appelle `gps.c` lancer la commande pour compiler ET lancer le test :
```bash
gcc -o .\unit_tests\gps.exe .\unit_tests\gps.c .\telemetry.c .\telemetry_utils.c && .\unit_tests\gps.exe
```