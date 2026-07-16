# VMANGOS port — features stubbed for a later session

To reach a building server, some subsystems that depend on cmangos APIs with
no straightforward vmangos equivalent were stubbed out (compile to safe
no-ops / defaults) rather than fully ported. Each stub is marked in code with
`// VMANGOS-TODO:` and guarded by `#ifdef VMANGOS`. This file is the index of
what still needs real reimplementation.

| Feature / subsystem | Status | Notes |
|---------------------|--------|-------|
| (to be filled in as stubs are added) | | |
