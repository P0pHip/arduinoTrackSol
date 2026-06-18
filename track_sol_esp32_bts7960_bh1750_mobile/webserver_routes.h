#pragma once

// Enregistre toutes les routes et démarre le serveur HTTP.
// À appeler une fois dans setup().
void setupServeur();

// Traite une requête en attente.
// À appeler dans loop() ET dans les boucles bloquantes des moteurs
// pour que l'interface web reste réactive pendant les mouvements.
void tickServeur();
