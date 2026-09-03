import math

def angle_from_extension(extension_in):
  # TODO: when testing with minibot, change these numbers to see how they effect results
  # - maybe make variables in future
    x = extension_in + 11
    return 261.44 - math.acos((231.3 - x**2) / 160.74)

def height_from_extension(extension_in):
  # TODO: when testing with minibot, change these numbers to see how they effect results
  # - maybe make variables in future
    return 30.6 * math.sin(angle_from_extension(extension_in)) + 14.1
