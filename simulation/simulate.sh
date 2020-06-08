#!/bin/bash

settings=(settings_luma
  settings_quant_luma
  settings_quant_seg_luma
  settings_quant_seg
  settings_seg
  settings_quant_seg_4
  settings_seg_4
  settings_quant
  settings_reference)

for settings in ${settings[@]}; do
  python3 ./simulation.py -S $settings
done
