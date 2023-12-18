#!/bin/sh

echo "Exporting to UPS.."
cd tools/ups/windows-arm/
./ups.exe -b ../POKEMON_EMER_BPEE00.gba -m pokeemerald.gba -o pokeemerald.ups
echo "Done."
