#pragma once

// Ce fichier n'est actif que sur le tracker MAÎTRE (IS_MASTER = true dans config.h).
// L'esclave ne compile pas ce code mais possède les endpoints HTTP
// /alerte_vent et /reset_alerte_vent dans webserver.cpp.

#if IS_MASTER

// Valeur brute lue sur le capteur (0 – 4095), exposée pour l'affichage web.
extern int valeurVent;

// Initialise le pin ADC du capteur de vent.
void setupVent();

// À appeler périodiquement dans loop().
// Lit le capteur, déclenche la mise en sécurité et notifie l'esclave si besoin.
void verifierVent();

// Envoie une requête HTTP POST à l'esclave.
// enAlerte = true  → POST /alerte_vent
// enAlerte = false → POST /reset_alerte_vent
void notifierEsclave(bool enAlerte);

#endif // IS_MASTER
