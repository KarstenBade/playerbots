# Canonical vanilla 1.12 talent builds (reference)

Bots get talents at runtime via vmangos's native `player_premade_spell_template`
tables (see PlayerbotFactory::InitTalentsVanillaPremade). This file is reference
material for the *builds themselves*, in case we want to author additional/better
premade specs in the world DB. Distributions are level-60 (51 points), cross-
checked against Wowhead-classic / Icy-Veins classic / Warcraft Tavern.

Primary raid spec per class (tree split Arms/Fury/Prot-style order):
- Warrior Fury DW 17/34/0 (melee dps); Protection ~8/5/38 or 16/5/30 (tank)
- Paladin Holy 31/5/15 (healer); Ret 11/0/40; Holy/Ret hybrid 21/0/30
- Hunter MM (+BM/Surv) ~7/31/13 (ranged dps); BM 31/20/0
- Rogue Combat Swords 18/33/0 or Daggers 15/31/5 (melee dps)
- Priest Deep Holy 21/30/0 (healer); Shadow 15/0/36 (caster dps)
- Shaman Restoration ~0/5/46 (healer); Elemental 31/3/17; Enhancement 5/30/16
- Mage Frost 17/0/34 (Winter's Chill); Fire 18/33/0 (Ignite)
- Warlock SM/Ruin 30/0/21; DS/Ruin 0/30/21 (caster dps)
- Druid Restoration 11/0/40 (healer); Feral ~13/32/6 (melee dps/tank)

Note: authoritative legacy dash-string encodings were not obtainable from a
citable source; if positional strings are ever needed, validate each in
https://www.wowhead.com/classic/talent-calc/<class> before use.
