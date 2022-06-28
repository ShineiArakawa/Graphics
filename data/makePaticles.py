import numpy as np


def main():
    NUM_PARTICLES_1D = 10
    DISTANCE = 0.5
    RADIUS = 0.24
    
    coords = np.empty(shape=(NUM_PARTICLES_1D**3, 3))
    vel = np.empty(shape=(NUM_PARTICLES_1D**3, 3))
    radius = np.empty(shape=NUM_PARTICLES_1D**3)
    mass = np.empty(shape=NUM_PARTICLES_1D**3)
    
    for i in range(NUM_PARTICLES_1D):
        for j in range(NUM_PARTICLES_1D):
            for k in range(NUM_PARTICLES_1D):
                index = i + j * NUM_PARTICLES_1D + k * NUM_PARTICLES_1D * NUM_PARTICLES_1D
                coords[index][0] = i * DISTANCE
                coords[index][1] = j * DISTANCE
                coords[index][2] = k * DISTANCE
                vel[index][0] = 0.0
                vel[index][1] = 0.0
                vel[index][2] = -1.0
                radius[index] = RADIUS
                mass[index] = 1.0
    
    coords -= NUM_PARTICLES_1D*DISTANCE/2.0
    
    with open("particles.dat", "w") as file:
        for i in range(coords.shape[0]):
            line = f"{radius[i]} {coords[i][0]} {coords[i][1]} {coords[i][2]} {vel[i][0]} {vel[i][1]} {vel[i][2]} {mass[i]}\n"
            file.write(line)
            
if __name__ == "__main__":
    main()