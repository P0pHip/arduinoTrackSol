#pragma once
#include <Arduino.h>

// Ajoute un message dans le journal (Serial + buffer web).
// Le buffer est limité à JOURNAL_MAX_LIGNES (voir config.h).
void ajouterLog(const String& msg);

// Échappe les caractères spéciaux d'une chaîne pour l'inclure dans du JSON.
String echapperJson(const String& s);
