#!/usr/bin/env bash

printf '\n\e[1mANSI 256-Color Palette\e[0m\n\n'

# Standard colors: 0-15
printf '\e[1mStandard colors (0-15)\e[0m\n'
for i in {0..15}; do
    printf '\e[48;5;%sm %3d \e[0m' "$i" "$i"
    (( (i + 1) % 8 == 0 )) && printf '\n'
done

printf '\n\e[1m216-color cube (16-231)\e[0m\n'
for i in {16..231}; do
    printf '\e[48;5;%sm %3d \e[0m' "$i" "$i"
    (( (i - 15) % 18 == 0 )) && printf '\n'
done

printf '\n\e[1mGrayscale (232-255)\e[0m\n'
for i in {232..255}; do
    printf '\e[48;5;%sm %3d \e[0m' "$i" "$i"
    (( (i - 231) % 12 == 0 )) && printf '\n'

done

printf '\n'

printf '\n\e[1mANSI 256-Color Palette\e[0m\n\n'

# Use a dark background for all labels.
# The actual color is shown as a colored block.

for i in {0..255}; do
    printf '\e[38;5;%sm %3d \e[0m' "$i" "$i"
    (( (i + 15) % 16 == 0 )) && printf '\n'
done

printf '\n'
