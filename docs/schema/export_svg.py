"""Exporte les schémas de docs/schema-electrique.html en SVG autonomes (affichables sur GitHub).

Usage : python docs/schema/export_svg.py
Relancer après chaque modification de schema-electrique.html.
"""
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent
HTML = ROOT.parent / "schema-electrique.html"
# Ordre d'apparition des <svg class="sch"> dans la page
NOMS = ["principe.svg", "bornier-capteurs.svg", "cablage.svg", "liaison-maitre-esclave.svg"]

page = HTML.read_text(encoding="utf-8")
style = re.search(r"<style>(.*?)</style>", page, re.S).group(1)

# Valeurs claires des tokens (premier bloc :root)
root = re.search(r":root\s*\{(.*?)\}", style, re.S).group(1)
tokens = dict(re.findall(r"--([\w-]+):\s*([^;]+);", root))
def resoudre(css):
    return re.sub(r"var\(--([\w-]+)\)", lambda m: tokens[m.group(1)].strip(), css)

regles = "\n".join(r.strip() for r in re.findall(r"(\.sch[^{]*\{[^}]*\})", style))
css = resoudre(f"svg {{ color: var(--ink); font-family: var(--sans); }}\n{regles}")

svgs = re.findall(r'(<svg class="sch[^"]*".*?</svg>)', page, re.S)
assert len(svgs) == len(NOMS), f"{len(svgs)} schémas trouvés, {len(NOMS)} attendus"

for nom, svg in zip(NOMS, svgs):
    vb = re.search(r'viewBox="([\d.\s-]+)"', svg).group(1).split()
    x, y, w, h = vb
    label = re.search(r'aria-label="([^"]*)"', svg)
    titre = f"<title>{label.group(1)}</title>" if label else ""
    tete = re.match(r"<svg[^>]*>", svg).group(0)
    nouvelle_tete = tete.replace("<svg ", f'<svg xmlns="http://www.w3.org/2000/svg" width="{w}" height="{h}" ', 1)
    fond = f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="{tokens["sheet"].strip()}"/>'
    corps = svg[len(tete):]
    (ROOT / nom).write_text(
        f'<?xml version="1.0" encoding="UTF-8"?>\n{nouvelle_tete}{titre}<style>{css}</style>{fond}{corps}\n',
        encoding="utf-8")
    print("écrit", nom)
