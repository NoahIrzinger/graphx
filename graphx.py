import ctypes
import numpy as np

def create_time_series(length=100):
    # Create an array of time steps
    x = np.arange(0, length)
    
    # Adjust the y values to start at the origin and oscillate. Also ensure integer values.
    y = (np.sin(x * 0.1) * np.cos(x * 0.2) * 100).astype(int)
    
    # Compute values as the product of x and y, then convert to integers
    values = (x * y).astype(int)
    
    return x, y, values

so_file = "./graphx.so"
lib = ctypes.CDLL(so_file)

print(type(lib))

print(lib.points_plot)

points_plot = lib.points_plot
points_plot.restype = None
points_plot.argtypes = [ctypes.POINTER(ctypes.c_int),
                        ctypes.POINTER(ctypes.c_int),
                        ctypes.POINTER(ctypes.c_int),
                        ctypes.c_uint]

# Usage
x, y, values = create_time_series(100)
print(x, y, values)

# Convert data to ctypes compatible types
x_c = x.astype(np.int32).ctypes.data_as(ctypes.POINTER(ctypes.c_int))
y_c = y.astype(np.int32).ctypes.data_as(ctypes.POINTER(ctypes.c_int))
values_c = values.astype(np.int32).ctypes.data_as(ctypes.POINTER(ctypes.c_int))

# Call the C function
print(points_plot(x_c, y_c, values_c, ctypes.c_uint(len(values))))
