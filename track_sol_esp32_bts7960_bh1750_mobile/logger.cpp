#include <Arduino.h>
#include "config.h"
#include "state.h"
#include "logger.h"

void ajouterLog(const String& msg) {
  Serial.println(msg);
  journal += String(millis() / 1000) + "s: " + msg + "\n";

  // Supprimer les lignes les plus anciennes si le buffer dépasse la limite
  int nbLignes = 0;
  for (int i = (int)journal.length() - 1; i >= 0; i--) {
    if (journal[i] == '\n' && ++nbLignes > JOURNAL_MAX_LIGNES) {
      journal = journal.substring(i + 1);
      break;
    }
  }
}

String echapperJson(const String& s) {
  String out = "";
  for (unsigned int i = 0; i < s.length(); i++) {
    char c = s.charAt(i);
    if      (c == '"')  out += "\\\"";
    else if (c == '\\') out += "\\\\";
    else if (c == '\n') out += "\\n";
    else if (c == '\r') { /* ignoré */ }
    else                out += c;
  }
  return out;
}
