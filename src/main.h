#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "defs.h"

#include "chunk.h"
#include "dig_math.h"
#include "models.h"
#include "opengl.h"
#include "shaders.h"
#include "pthread_aliases.h"
#include "textures.h"
#include "utils.h"
#include "world.h"
