#pragma once

// =====================================================================
//   SÉLECTEUR DE TRACKER — Template
//   Copier ce fichier en config.h, puis décommenter UNE ligne #include.
//
//   Avant de flasher, s'assurer que config_master.h et config_slave.h
//   existent (les créer depuis leurs .example.h respectifs).
// =====================================================================

#include "config_master.h"
// #include "config_slave.h"

#if !defined(IS_MASTER)
  #error "Aucune config incluse — decommenter UNE ligne dans config.h"
#endif
