#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "defs.h"

void texture_indices_to_texture_coords(int *textures, Rectangle* texture_coords, int num_textures,
                                       int unit_size, int textures_per_row);

GLuint genTexturedCubeBuffer(int textures[6]);

GLuint genTexturedSlopeBuffer(int textures[5]);

GLuint genTexturedSlopeCornerBuffer(int textures[4]);

GLuint genTexturedGroundedSprite(int index);
