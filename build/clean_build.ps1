arduino-cli compile --fqbn esp32:esp32:esp32da --output-dir build --verbose track_sol_esp32_bts7960_bh1750_mobile
if ($LASTEXITCODE -eq 0) {
    Remove-Item build\*.bootloader.bin, build\*.partitions.bin, build\*.merged.bin, build\*.elf, build\*.map -ErrorAction SilentlyContinue
}