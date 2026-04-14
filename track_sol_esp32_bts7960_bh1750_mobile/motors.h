#pragma once
#include <BTS7960.h>

// ── Objets moteurs (définis dans motors.cpp) ──────────────────────────
extern BTS7960 motorEO;   // moteur Est-Ouest
extern BTS7960 motorIH;   // moteur Inclinaison

// ── Fonctions ─────────────────────────────────────────────────────────
void setupMoteurs();
void arreterMoteurs();

// Ramène l'inclinaison à plat (jusqu'au FDC_IH)
void miseAPlat();

// Ramène la rotation vers l'Est (jusqu'au FDC_EST)
void retourEst();

// Séquence de mise en sécurité : arrêt + plat
void mettreEnSecurite();

// Un step de tracking sur un axe.
// Appelle lireCapteurs() et tickServeur() en boucle — ne bloque pas le web server.
void trackSun(BTS7960& motor, int& fdc1, int& fdc2, float& lux1, float& lux2);
