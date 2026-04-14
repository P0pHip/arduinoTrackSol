#pragma once
#include <Arduino.h>

// =====================================================================
//   ÉTAT PARTAGÉ ENTRE LES MODULES
//
//   Les variables sont DÉFINIES dans le fichier .ino principal.
//   Chaque module qui en a besoin les importe via cet extern.
// =====================================================================

extern bool   modeAuto;          // true = suivi automatique actif
extern bool   alerteVent;        // true = alerte vent en cours, tracker en sécurité
extern bool   enPositionRepos;   // true = position repos (plat + Est) atteinte, ne pas relancer

extern String cmdMoteur;          // commande moteur courante (mode manuel)
extern unsigned long tDernierCmd; // timestamp de la dernière commande moteur

extern String journal;      // buffer des derniers messages de log
