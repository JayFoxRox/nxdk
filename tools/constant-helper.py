#!/usr/bin/env python3

# This helps to find values of NV register combiner constant-colors.
# Also can do reverse search so you can encode non-normalized values.

import sys

#FIXME: Clamping is ignored!
input_modifiers = {
  "x": (False, 0.0, 1.0),
  "unsigned(x)": (True, 0.0, 1.0),
  "expand(x)": (True, -1.0, 2.0),
  "half_bias(x)": (True, -0.5, 1.0),
  "-x": (False, 0.0, -1.0),
  "unsigned_invert(x)": (True, 1.0, -1.0),
  "-expand(x)": (True, 1.0, -2.0),
  "-half_bias(x)": (True, 0.5, -1.0)
}

output_modifiers = {
  "": (0.0,  1.0),
  "scale_by_one_half()": (0.0, 0.5),
  "scale_by_two()": (0.0, 2.0),
  "scale_by_four()": (0.0, 4.0),
  "bias_by_negative_one_half()": (-0.5, 1.0),
  "bias_by_negative_one_half_scale_by_two()": (-0.5, 2.0)
}

modifiers = []
for output_modifier in output_modifiers:
  for input_modifier in input_modifiers:
    modifier = (input_modifier, output_modifier)
    modifiers += [modifier]

def find_value(value, input_modifier, output_modifier="", display=True):
  assert(value >= 0.0)
  assert(value <= 1.0)
  input_clamp, input_bias, input_scale = input_modifiers[input_modifier]
  output_bias, output_scale = output_modifiers[output_modifier]

  if display:
    print("x: %f" % value)
  input_value = value * input_scale + input_bias
  if display:
    print("%s: %f*%f%+f = %f" % (input_modifier, value, input_scale, input_bias, input_value))
  output_value = input_value * output_scale + output_bias
  if display:
    if output_modifier != "":
      print("%s: %f*%f%+f = %f" % (output_modifier, input_value, output_scale, output_bias, output_value))
    print()
  
  return output_value

def find_value_reverse(output_value, input_modifier, output_modifier="", display=True):
  input_clamp, input_bias, input_scale = input_modifiers[input_modifier]
  output_bias, output_scale = output_modifiers[output_modifier]

  input_value = (output_value - output_bias) / output_scale
  if display:
    if output_modifier != "":
      print("%s: %f*%f%+f = %f" % (output_modifier, input_value, output_scale, output_bias, output_value))
  value = (input_value - input_bias) / input_scale
  if display:
    print("%s: %f*%f%+f = %f" % (input_modifier, value, input_scale, input_bias, input_value))
    print("x: %f" % value)
    print()
  
  return value

def display_fit(modifier_candidates=modifiers, values=[]):
  print("Fit for %s" % values)
  for modifier in modifier_candidates:

    # Display basic info
    input_modifier, output_modifier = modifier
    if output_modifier:
      print("%s; %s;" % (input_modifier, output_modifier))
    else:
      print("%s;" % input_modifier)

    # Display range
    min_value = find_value(0.0, input_modifier, output_modifier, display=False)
    max_value = find_value(1.0, input_modifier, output_modifier, display=False)
    if min_value > max_value:
      min_value, max_value = max_value, min_value
      print("v ??? v")
    print("    range: [%f, %f]" % (min_value, max_value))

    # Display values
    for value in values:
      result = find_value_reverse(value, input_modifier, output_modifier, display=False)

      # Avoid negative 0
      result = abs(result)

      #FIXME: Verify that this is what GPU does
      result8 = round(result * 0xFF)
      result8f = result8 / 0xFF

      value8f = find_value(result8f, input_modifier, output_modifier, display=False)
      delta = value8f - value
      print("    value: %+f -> %f -> 0x%02X -> %f -> %+f (delta: %+f)" % (value, result, result8, result8f, value8f, delta))

  return

def _find_fit_one(value, modifier_candidates=modifiers, display=True):

  _modifier_candidates = []

  for modifier in modifier_candidates:
    input_modifier, output_modifier = modifier
    min_value = find_value(0.0, input_modifier, output_modifier, display=False)
    max_value = find_value(1.0, input_modifier, output_modifier, display=False)
    if min_value > max_value:
      #assert(False)
      min_value, max_value = max_value, min_value
      #continue

    # Filter results
    if True:
      if (value < min_value):
        continue
      if (value > max_value):
        continue

    if display:
      display_fit([modifier], [value])

    _modifier_candidates += [modifier]

  return _modifier_candidates

def find_fit(values, modifier_candidates=modifiers, display=True):
  _modifier_candidates = modifier_candidates

  for value in values:
    _modifier_candidates = _find_fit_one(value, _modifier_candidates, display=False)

  if display:
    display_fit(_modifier_candidates, values)

  return _modifier_candidates
  

find_value(0.0, "half_bias(x)", "scale_by_four()")
find_value(0.5, "half_bias(x)", "scale_by_four()")
find_value(1.0, "half_bias(x)", "scale_by_four()")

print("----------------------")
print("----------------------")
find_fit([-3.0])

print("----------------------")
print("----------------------")
find_fit([-1.0, 2.0])

print("----------------------")
print("----------------------")
find_fit([3.0, 5.0])

print("----------------------")
print("----------------------")
find_fit([0.000, -0.343, 1.765])

print("----------------------")
print("----------------------")
find_fit([1.400, -0.711, 0.000])

print("----------------------")
print("----------------------")
find_fit([-3.14159265359, 3.14159265359])

print("----------------------")
print("----------------------")
find_fit([-4, -3, -2, -1, 1, 2, 3, 4])

print("----------------------")
print("----------------------")

if len(sys.argv) > 2:
  value = float(sys.argv[1])
  input_modifier = sys.argv[2]
  if len(sys.argv) > 3:
    output_modifier = sys.argv[3]
  else:
    output_modifier = ""

  find_value(value, input_modifier, output_modifier)



#scale_by_two(expand(0.5), expand(0.3), expand(1.0), half_bias(0.5))
