#include <pgmspace.h>
#include "html_page.h"

// =====================================================================
//   PAGE HTML DE L'INTERFACE WEB
//   Stockée en flash pour ne pas consommer la RAM de l'ESP32.
// =====================================================================
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Tracker Solaire</title>
  <style>
    *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      font-family: 'Segoe UI', Arial, sans-serif;
      background: #0d1117;
      color: #e6edf3;
      padding: 12px;
      max-width: 480px;
      margin: 0 auto;
    }
    h1 { color: #f9a825; text-align: center; margin: 8px 0 16px; font-size: 1.35em; }

    .card {
      background: #161b22;
      border: 1px solid #30363d;
      border-radius: 10px;
      padding: 14px;
      margin-bottom: 12px;
    }
    .card-title {
      font-size: 0.72em;
      font-weight: 700;
      letter-spacing: 1.5px;
      text-transform: uppercase;
      color: #8b949e;
      margin-bottom: 10px;
    }

    /* ── Mode switch ─────────────────────────────────── */
    .mode-row { display: flex; align-items: center; justify-content: space-between; }
    .mode-badge {
      font-size: 1.05em; font-weight: bold;
      padding: 4px 14px; border-radius: 20px;
      background: #1f6b2e; color: #56d364;
      transition: background .3s, color .3s;
    }
    .mode-badge.manuel { background: #6b1f1f; color: #f47067; }

    .switch { position: relative; display: inline-block; width: 56px; height: 28px; }
    .switch input { display: none; }
    .slider {
      position: absolute; inset: 0;
      background: #f47067; border-radius: 28px;
      cursor: pointer; transition: background .3s;
    }
    .slider::before {
      content: ""; position: absolute;
      width: 22px; height: 22px; left: 3px; top: 3px;
      background: white; border-radius: 50%;
      transition: transform .3s;
    }
    input:checked + .slider { background: #56d364; }
    input:checked + .slider::before { transform: translateX(28px); }

    /* ── Vent ────────────────────────────────────────── */
    .vent-ok {
      background: #1f6b2e22; border: 1px solid #56d36466;
      color: #56d364; padding: 8px 12px; border-radius: 8px;
      text-align: center; font-weight: bold;
    }
    .vent-alert {
      background: #6b1f1f44; border: 1px solid #f4706766;
      color: #f47067; padding: 8px 12px; border-radius: 8px;
      text-align: center; font-weight: bold;
      animation: clignoter 1s infinite;
    }
    @keyframes clignoter { 50% { opacity: .4; } }
    .vent-val { font-size: 0.78em; color: #8b949e; margin-top: 5px; text-align: center; }

    /* ── Luminosité ──────────────────────────────────── */
    .lux-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 8px; }
    .lux-item {
      background: #0d1117; border: 1px solid #30363d;
      border-radius: 8px; padding: 10px; text-align: center;
    }
    .lux-label { font-size: 0.7em; color: #8b949e; margin-bottom: 4px; }
    .lux-val   { font-size: 1.3em; font-weight: bold; color: #f9a825; }
    .lux-unit  { font-size: 0.65em; color: #8b949e; }

    /* ── Fins de course ──────────────────────────────── */
    .fdc-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 8px; }
    .fdc-item {
      display: flex; align-items: center; gap: 8px;
      background: #0d1117; border: 1px solid #30363d;
      border-radius: 8px; padding: 8px 10px;
    }
    .fdc-dot { width: 12px; height: 12px; border-radius: 50%; flex-shrink: 0; background: #30363d; }
    .fdc-dot.actif   { background: #f47067; box-shadow: 0 0 6px #f4706788; }
    .fdc-dot.inactif { background: #56d364; box-shadow: 0 0 6px #56d36488; }
    .fdc-label { font-size: 0.8em; }

    /* ── Contrôle manuel ─────────────────────────────── */
    #ctrl-manuel { display: none; }
    .btn-grid {
      display: grid;
      grid-template-columns: 1fr 1fr 1fr;
      grid-template-rows: auto auto auto;
      gap: 10px;
      max-width: 240px;
      margin: 0 auto;
    }
    .btn-mot {
      background: #21262d; border: 1px solid #30363d;
      color: #e6edf3; padding: 16px 8px; border-radius: 10px;
      font-size: 1em; cursor: pointer;
      user-select: none; -webkit-user-select: none; touch-action: none;
      transition: background .15s, border-color .15s;
    }
    .btn-mot:active { background: #f9a825; color: #000; border-color: #f9a825; }
    .btn-vide { background: transparent; border: none; cursor: default; }

    /* ── Journal ─────────────────────────────────────── */
    #log-zone {
      background: #0d1117; border: 1px solid #30363d;
      border-radius: 8px; padding: 8px 10px;
      height: 150px; overflow-y: auto;
      font-size: 0.72em; font-family: 'Consolas', monospace;
    }
    .log-ligne { padding: 1px 0; color: #c9d1d9; }

    /* ── Bouton Admin ────────────────────────────────── */
    .btn-admin-open {
      display: block; width: 100%; padding: 10px;
      background: #21262d; border: 1px solid #30363d;
      border-radius: 10px; color: #8b949e;
      font-size: 0.88em; cursor: pointer;
      text-align: center; margin-bottom: 12px;
      transition: background .2s, color .2s;
    }
    .btn-admin-open:active { background: #30363d; color: #e6edf3; }

    /* ── Modal Admin ─────────────────────────────────── */
    .admin-overlay {
      display: none; position: fixed; inset: 0;
      background: #000000bb; z-index: 100;
      align-items: center; justify-content: center;
    }
    .admin-overlay.visible { display: flex; }
    .admin-modal {
      background: #161b22; border: 1px solid #30363d;
      border-radius: 12px; padding: 22px;
      width: 92%; max-width: 360px;
    }
    .admin-modal-title {
      text-align: center; font-weight: bold;
      font-size: 1.05em; margin-bottom: 16px;
    }
    .admin-pin-input {
      width: 100%; padding: 12px; margin: 4px 0 6px;
      background: #0d1117; border: 1px solid #30363d;
      border-radius: 8px; color: #e6edf3;
      font-size: 1.4em; text-align: center; letter-spacing: 6px;
      outline: none;
    }
    .admin-pin-input:focus { border-color: #f9a825; }
    .admin-err { color: #f47067; font-size: 0.8em; text-align: center; min-height: 18px; }
    .admin-label {
      display: block; font-size: 0.76em;
      color: #8b949e; margin-top: 10px; margin-bottom: 3px;
    }
    .admin-field {
      width: 100%; padding: 9px 10px;
      background: #0d1117; border: 1px solid #30363d;
      border-radius: 8px; color: #e6edf3; font-size: 1em;
      outline: none;
    }
    .admin-field:focus { border-color: #f9a825; }
    .admin-save-msg { font-size: 0.82em; text-align: center; min-height: 20px; margin-top: 8px; }
    .admin-row { display: flex; gap: 8px; margin-top: 14px; }
    .btn-admin-cancel {
      flex: 1; padding: 11px; border-radius: 8px;
      border: 1px solid #30363d; background: #21262d;
      color: #e6edf3; cursor: pointer; font-size: 0.9em;
    }
    .btn-admin-ok {
      flex: 2; padding: 11px; border-radius: 8px;
      border: none; background: #f9a825;
      color: #000; cursor: pointer; font-size: 0.9em; font-weight: bold;
    }
  </style>
</head>
<body>

<h1>&#9728; Tracker Solaire</h1>

<!-- MODE -->
<div class="card">
  <div class="card-title">Mode de fonctionnement</div>
  <div class="mode-row">
    <div class="mode-badge" id="mode-badge">AUTO</div>
    <label class="switch" title="Basculer Auto / Manuel">
      <input type="checkbox" id="mode-toggle" onchange="basculerMode(this)">
      <span class="slider"></span>
    </label>
  </div>
</div>

<!-- VENT -->
<div class="card">
  <div class="card-title">Capteur de vent</div>
  <div class="vent-ok" id="vent-status">&#10003; Pas d'alerte vent</div>
  <div class="vent-val" id="vent-val"></div>
</div>

<!-- LUMINOSITE -->
<div class="card">
  <div class="card-title">Luminosit&eacute; (lux)</div>
  <div class="lux-grid">
    <div class="lux-item">
      <div class="lux-label">Nord</div>
      <div class="lux-val" id="lux-n">&#8212;</div>
      <div class="lux-unit">lux</div>
    </div>
    <div class="lux-item">
      <div class="lux-label">Sud</div>
      <div class="lux-val" id="lux-s">&#8212;</div>
      <div class="lux-unit">lux</div>
    </div>
    <div class="lux-item">
      <div class="lux-label">Est</div>
      <div class="lux-val" id="lux-e">&#8212;</div>
      <div class="lux-unit">lux</div>
    </div>
    <div class="lux-item">
      <div class="lux-label">Ouest</div>
      <div class="lux-val" id="lux-o">&#8212;</div>
      <div class="lux-unit">lux</div>
    </div>
  </div>
</div>

<!-- FINS DE COURSE -->
<div class="card">
  <div class="card-title">Fins de course</div>
  <div class="fdc-grid">
    <div class="fdc-item"><div class="fdc-dot" id="fdc-ih"></div><span class="fdc-label">Incl. Haut (Nord)</span></div>
    <div class="fdc-item"><div class="fdc-dot" id="fdc-iv"></div><span class="fdc-label">Incl. Bas (Sud)</span></div>
    <div class="fdc-item"><div class="fdc-dot" id="fdc-es"></div><span class="fdc-label">Est</span></div>
    <div class="fdc-item"><div class="fdc-dot" id="fdc-ou"></div><span class="fdc-label">Ouest</span></div>
  </div>
</div>

<!-- CONTRÔLE MANUEL -->
<div class="card" id="ctrl-manuel">
  <div class="card-title">Contr&ocirc;le manuel &#8212; maintenir appuy&eacute;</div>
  <div class="btn-grid">
    <div class="btn-vide"></div>
    <button class="btn-mot" id="btn-ih-b">&#8593; Haut</button>
    <div class="btn-vide"></div>

    <button class="btn-mot" id="btn-eo-b">&#8592; Est</button>
    <button class="btn-mot btn-vide" style="color:#555;font-size:.7em;">&#9632;</button>
    <button class="btn-mot" id="btn-eo-f">Ouest &#8594;</button>

    <div class="btn-vide"></div>
    <button class="btn-mot" id="btn-ih-f">&#8595; Bas</button>
    <div class="btn-vide"></div>
  </div>
</div>

<!-- JOURNAL -->
<div class="card">
  <div class="card-title">Journal temps r&eacute;el</div>
  <div id="log-zone"></div>
</div>

<!-- BOUTON ADMIN -->
<button class="btn-admin-open" onclick="ouvrirAdmin()">&#128274; Admin</button>

<!-- MODAL ADMIN -->
<div class="admin-overlay" id="admin-overlay" onclick="if(event.target===this)fermerAdmin()">
  <div class="admin-modal">

    <!-- Étape 1 : saisie du PIN -->
    <div id="admin-pin-step">
      <div class="admin-modal-title" style="color:#f9a825;">&#128272; Acc&egrave;s Administrateur</div>
      <input class="admin-pin-input" type="password" id="admin-pin-input"
             maxlength="8" inputmode="numeric" pattern="[0-9]*"
             placeholder="&#9679; &#9679; &#9679; &#9679; &#9679; &#9679; &#9679; &#9679;"
             onkeydown="if(event.key==='Enter')verifierPin()">
      <div class="admin-err" id="admin-pin-err"></div>
      <div class="admin-row">
        <button class="btn-admin-cancel" onclick="fermerAdmin()">Annuler</button>
        <button class="btn-admin-ok"     onclick="verifierPin()">D&eacute;verrouiller</button>
      </div>
    </div>

    <!-- Étape 2 : formulaire de configuration -->
    <div id="admin-config-step" style="display:none;">
      <div class="admin-modal-title" style="color:#56d364;">&#9881;&#65039; Configuration</div>

      <label class="admin-label">Vitesse moteur Est-Ouest (0 &ndash; 255)</label>
      <input class="admin-field" type="number" id="cfg-viteo" min="0" max="255">

      <label class="admin-label">Vitesse moteur Inclinaison (0 &ndash; 255)</label>
      <input class="admin-field" type="number" id="cfg-vih"   min="0" max="255">

      <label class="admin-label">Seuil luminosit&eacute; (lux)</label>
      <input class="admin-field" type="number" id="cfg-lum"   min="0" max="100000">

      <label class="admin-label">Seuil vent (km/h)</label>
      <input class="admin-field" type="number" id="cfg-vent"  min="0" max="300">

      <div class="admin-save-msg" id="admin-save-msg"></div>
      <div class="admin-row">
        <button class="btn-admin-cancel" onclick="fermerAdmin()">&#128274; Fermer</button>
        <button class="btn-admin-ok"     onclick="sauvegarderConfig()">&#128190; Sauvegarder</button>
      </div>
    </div>

  </div>
</div>

<script>
// ── État local ───────────────────────────────────────────────────────
let modeAutoLocal  = true;
let timerMoteur    = null;
let dernierJournal = "";
let dernierData    = null;   // dernière réponse /data, pour pré-remplir le formulaire admin
let adminPin       = "";

// ── Bascule Auto / Manuel ────────────────────────────────────────────
function basculerMode(cb) {
  modeAutoLocal = cb.checked;
  fetch('/mode?auto=' + (cb.checked ? '1' : '0'), { method: 'POST' });
  majAffichageMode(cb.checked);
}

function majAffichageMode(auto) {
  const badge = document.getElementById('mode-badge');
  const ctrl  = document.getElementById('ctrl-manuel');
  badge.textContent = auto ? 'AUTO' : 'MANUEL';
  badge.className   = 'mode-badge' + (auto ? '' : ' manuel');
  ctrl.style.display = auto ? 'none' : 'block';
}

// ── Contrôle moteur (maintien appuyé + keep-alive) ───────────────────
function demarrerMoteur(cmd) {
  if (modeAutoLocal) return;
  envoyerCmd(cmd);
  timerMoteur = setInterval(() => envoyerCmd(cmd), 300);
}

function arreterMoteur() {
  if (timerMoteur) { clearInterval(timerMoteur); timerMoteur = null; }
  envoyerCmd('STOP');
}

function envoyerCmd(cmd) {
  fetch('/moteur?cmd=' + cmd, { method: 'POST' }).catch(() => {});
}

// Attacher les events aux boutons (souris + tactile)
document.addEventListener('DOMContentLoaded', function () {
  const btns = {
    'btn-eo-f': 'EO_F',
    'btn-eo-b': 'EO_B',
    'btn-ih-f': 'IH_F',
    'btn-ih-b': 'IH_B'
  };
  for (const [id, cmd] of Object.entries(btns)) {
    const el = document.getElementById(id);
    el.addEventListener('mousedown',  () => demarrerMoteur(cmd));
    el.addEventListener('mouseup',    () => arreterMoteur());
    el.addEventListener('mouseleave', () => arreterMoteur());
    el.addEventListener('touchstart', (e) => { e.preventDefault(); demarrerMoteur(cmd); });
    el.addEventListener('touchend',   (e) => { e.preventDefault(); arreterMoteur(); });
  }
});

// ── Admin : ouverture / fermeture du modal ───────────────────────────
function ouvrirAdmin() {
  document.getElementById('admin-overlay').classList.add('visible');
  document.getElementById('admin-pin-step').style.display   = 'block';
  document.getElementById('admin-config-step').style.display = 'none';
  document.getElementById('admin-pin-input').value = '';
  document.getElementById('admin-pin-err').textContent = '';
  setTimeout(() => document.getElementById('admin-pin-input').focus(), 50);
}

function fermerAdmin() {
  document.getElementById('admin-overlay').classList.remove('visible');
  adminPin = '';
}

// ── Admin : vérification du PIN ──────────────────────────────────────
async function verifierPin() {
  const pin = document.getElementById('admin-pin-input').value;
  const err = document.getElementById('admin-pin-err');
  if (!/^[0-9]{8}$/.test(pin)) {
    err.textContent = 'Le PIN doit contenir exactement 8 chiffres';
    return;
  }
  try {
    const r = await fetch('/admin/login?pin=' + encodeURIComponent(pin), { method: 'POST' });
    const d = await r.json();
    if (d.ok) {
      adminPin = pin;
      document.getElementById('admin-pin-step').style.display    = 'none';
      document.getElementById('admin-config-step').style.display = 'block';
      if (dernierData) {
        document.getElementById('cfg-viteo').value = dernierData.vitEO;
        document.getElementById('cfg-vih').value   = dernierData.vitIH;
        document.getElementById('cfg-lum').value   = dernierData.seuilLum;
        document.getElementById('cfg-vent').value  = dernierData.seuilVent;
      }
    } else {
      err.textContent = '\u274C PIN incorrect';
    }
  } catch(_) {
    err.textContent = '\u26A0 Erreur r\u00e9seau';
  }
}

// ── Admin : sauvegarde de la configuration ───────────────────────────
async function sauvegarderConfig() {
  const params = new URLSearchParams({
    pin:       adminPin,
    vitEO:     document.getElementById('cfg-viteo').value,
    vitIH:     document.getElementById('cfg-vih').value,
    seuilLum:  document.getElementById('cfg-lum').value,
    seuilVent: document.getElementById('cfg-vent').value
  });
  const msg = document.getElementById('admin-save-msg');
  try {
    const r = await fetch('/admin/config?' + params.toString(), { method: 'POST' });
    if (r.ok) {
      msg.style.color = '#56d364';
      msg.textContent = '\u2713 Param\u00e8tres sauvegard\u00e9s !';
    } else {
      msg.style.color = '#f47067';
      msg.textContent = '\u2717 Erreur lors de la sauvegarde';
    }
  } catch(_) {
    msg.style.color = '#f47067';
    msg.textContent = '\u26A0 Erreur r\u00e9seau';
  }
  setTimeout(() => { msg.textContent = ''; }, 3000);
}

// ── Mise à jour de l'interface ───────────────────────────────────────
function majInterface(d) {
  dernierData = d;
  // Luminosité
  document.getElementById('lux-n').textContent = d.luxN.toFixed(1);
  document.getElementById('lux-s').textContent = d.luxS.toFixed(1);
  document.getElementById('lux-e').textContent = d.luxE.toFixed(1);
  document.getElementById('lux-o').textContent = d.luxO.toFixed(1);

  // Fins de course (LOW = déclenché = rouge)
  [['fdc-ih', d.fdcIH], ['fdc-iv', d.fdcIV], ['fdc-es', d.fdcES], ['fdc-ou', d.fdcOU]]
    .forEach(([id, val]) => {
      document.getElementById(id).className = 'fdc-dot ' + (val === 0 ? 'actif' : 'inactif');
    });

  // Vent
  const vs = document.getElementById('vent-status');
  if (d.alerteVent) {
    vs.className   = 'vent-alert';
    vs.textContent = '\u26A0 ALERTE VENT \u2014 Tracker en s\u00e9curit\u00e9 !';
  } else {
    vs.className   = 'vent-ok';
    vs.textContent = '\u2713 Pas d\'alerte vent';
  }
  if (typeof d.ventVal !== 'undefined') {
    document.getElementById('vent-val').textContent = 'Valeur capteur\u00a0: ' + d.ventVal;
  }

  // Sync mode si changé côté serveur (ex: alerte vent force manuel)
  const tog = document.getElementById('mode-toggle');
  if (tog.checked !== d.modeAuto) {
    tog.checked   = d.modeAuto;
    modeAutoLocal = d.modeAuto;
    majAffichageMode(d.modeAuto);
  }

  // Journal : ajouter seulement les nouvelles lignes
  if (d.journal && d.journal !== dernierJournal) {
    const zone   = document.getElementById('log-zone');
    const lignes = d.journal.split('\n').filter(l => l.trim());
    const deja   = zone.querySelectorAll('.log-ligne').length;
    lignes.slice(deja).forEach(msg => {
      const div = document.createElement('div');
      div.className   = 'log-ligne';
      div.textContent = msg;
      zone.appendChild(div);
    });
    zone.scrollTop    = zone.scrollHeight;
    dernierJournal    = d.journal;
  }
}

// ── Polling (refresh toutes les secondes) ────────────────────────────
async function poll() {
  try {
    const r = await fetch('/data');
    if (r.ok) majInterface(await r.json());
  } catch (_) {}
  setTimeout(poll, 1000);
}

poll();
</script>
</body>
</html>
)rawliteral";
