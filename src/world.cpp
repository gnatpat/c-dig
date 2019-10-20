Block OOB_BLOCK = { CUBE };

struct ChunkAndPosition {
  bool loaded;
  Chunk* chunk;
  V3i in_chunk_pos;
};

ChunkAndPosition absoluteValueToChunkAndPosition(LoadedWorld* world, V3i pos){
  V3i loaded_pos = pos - world->origin;
  V3i chunk_pos = loaded_pos / CHUNK_SIZE;
  V3i in_chunk_pos = loaded_pos % CHUNK_SIZE;
  if(loaded_pos.x < 0 or loaded_pos.y < 0 or loaded_pos.z < 0 or
    chunk_pos.x >= LOADED_WORLD_SIZE or chunk_pos.y >= LOADED_WORLD_SIZE or chunk_pos.z >= LOADED_WORLD_SIZE) {
    return { false, NULL, 0 };
  }
  Chunk* chunk = getChunkRelativeToLoadedWorld(world, chunk_pos);
  return { true, chunk, in_chunk_pos };
}


Block getBlockAt(LoadedWorld* world, V3i pos) {
  ChunkAndPosition chunk_and_position = absoluteValueToChunkAndPosition(world, pos);
  if (!chunk_and_position.loaded) {
    return OOB_BLOCK;
  }
  return getBlockAt(chunk_and_position.chunk, chunk_and_position.in_chunk_pos);
}

void putBlockAt(LoadedWorld* world, V3i pos, Block block) {
  ChunkAndPosition chunk_and_position = absoluteValueToChunkAndPosition(world, pos);
  if (!chunk_and_position.loaded) {
    return;
  }
  putBlockAt(chunk_and_position.chunk, chunk_and_position.in_chunk_pos, block);
  for (int x = -1; x <= 1; x++) {
    for (int y = -1; y <= 1; y++) {
      for (int z = -1; z <= 1; z++) {
        chunk_and_position = absoluteValueToChunkAndPosition(world, pos + v3i(x, y, z));
        if (!chunk_and_position.loaded) {
          return;
        }
        markChunkAsDirty(world, chunk_and_position.chunk);
      }
    }
  }
}


bool isPointSolid(LoadedWorld* world, V3 pos) {
  BlockShape shape = getBlockAt(world, toV3i(pos)).block_shape;
  return isBlockSolidAtPoint(shape, fractional_part(pos));
}


MaybeRayTraceResult doesRayHitTriangle(V3 from, V3 direction, Triangle triangle, V3i block_position) {
  MaybeRayTraceResult result;
  V3 from_in_triangle_coords = from - toV3(block_position);
  float normal_distance = signedDistanceFromTrianglePlane(triangle, from_in_triangle_coords);

  float direction_dot_normal = dot(direction, -triangle.normal);
  if (direction_dot_normal == 0.0 && normal_distance != 0.0) {
    result.hit = false;
    return result;
  }

  float distance = normal_distance == 0.0 ? 0.0 : normal_distance/direction_dot_normal;
  V3 ray_hit_position_on_triangle_plane = from_in_triangle_coords + distance * direction;

  if (!isPointInTriangle(triangle, ray_hit_position_on_triangle_plane)) {
    result.hit = false;
    return result;
  }

  result.hit = true;
  result.hit_position = ray_hit_position_on_triangle_plane + toV3(block_position);
  result.hit_face = shiftTriangle(triangle, toV3(block_position));
  result.block_position = block_position;
  return result;
}


float EPSILON = 0.000001;
MaybeRayTraceResult blockRayTrace(LoadedWorld* world, V3 from, V3 direction, float max_distance) {
  float distance_left = max_distance;
  while(distance_left > 0) {
    V3 current_location = from + direction * (max_distance - distance_left);
    V3i current_block = toV3i(current_location + copysign(v3(EPSILON, EPSILON, EPSILON), direction));
    BlockModel block_model = BLOCK_MODELS[getBlockAt(world, current_block).block_shape];

    printf("==RAY TRACE==\n");
    printf("Starting at ");
    printV3(current_location);
    printf("\nSo block is ");
    printV3i(current_block);
    printf("\nMoving in direction ");
    printV3(direction);
    printf("\nWith %.6f distance left.\n", distance_left);
    printf("Block has %d triangles.\n", block_model.triangle_count);


    float closest_result_distance = distance_left;
    MaybeRayTraceResult closest_result;
    closest_result.hit = false;
    for (int triangle_index = 0; triangle_index < block_model.triangle_count; triangle_index++) {
      MaybeRayTraceResult result = doesRayHitTriangle(current_location, direction, block_model.mesh[triangle_index], current_block);
      if (result.hit) {
        float distance_to_hit = len(result.hit_position - current_location);
        if (distance_to_hit < closest_result_distance) {
          closest_result = result;
          closest_result_distance = distance_to_hit;
        }
      }
    }
    if (closest_result.hit) {
      return closest_result;
    }

    V3 next_block = toV3(current_block) + v3(0.5, 0.5, 0.5) + copysign(v3(0.5, 0.5, 0.5), direction);
    V3 distance_to_next_block = next_block - current_location;
    V3 step_distances = distance_to_next_block / direction;

    float step_distance = max(step_distances);
    if(step_distances.x < step_distance && step_distances.x != 0.0) {
      step_distance = step_distances.x;
    }
    if(step_distances.y < step_distance && step_distances.y != 0.0) {
      step_distance = step_distances.y;
    }
    if(step_distances.z < step_distance && step_distances.z != 0.0) {
      step_distance = step_distances.z;
    }
    distance_left -= step_distance;
  }
  MaybeRayTraceResult result;
  result.hit = false;
  return result;
}


void setUpNewChunk(LoadedWorld* world, V3i in_memory_pos) {
  // We should assert we have the lock here.
  Chunk* c = &world->chunks[in_memory_pos.x][in_memory_pos.y][in_memory_pos.z];
  c->origin = world->origin + ((in_memory_pos - world->wrap_break + LOADED_WORLD_SIZE) % LOADED_WORLD_SIZE) * CHUNK_SIZE;
  c->render_data.state = NO_RENDER_DATA;
  if (c->render_data.vertices != NULL) {
    free(c->render_data.vertices);
  }
  initChunk(c);
  addToLinkedList(&world->render_state.new_chunks, c);
}

void markChunkAsDirty(LoadedWorld* world, Chunk* c) {
  if (c->render_data.state == NO_RENDER_DATA) {
    return;
  }
  c->render_data.state = NO_RENDER_DATA;
  if (c->render_data.vertices != NULL) {
    free(c->render_data.vertices);
  }
  addToLinkedList(&world->render_state.dirty_chunks, c);
}

void initWorld(LoadedWorld* world) {
  world->origin = v3i(0, 0, 0);
  world->wrap_break = v3i(0, 0, 0);
  world->render_state.new_chunks = NULL;
  world->render_state.dirty_chunks = NULL;

  pthread_mutex_init(&world->render_state.new_chunk_lock, NULL);
  pthread_cond_init(&world->render_state.new_chunk_condition, NULL);
  lockMutex(&world->render_state.new_chunk_lock);
  for(int x = 0; x < LOADED_WORLD_SIZE; x++) {
    for(int y = 0; y < LOADED_WORLD_SIZE; y++) {
      for(int z = 0; z < LOADED_WORLD_SIZE; z++) {
        setUpNewChunk(world, v3i(x, y, z));
      }
    }
  }
  signalCondition(&world->render_state.new_chunk_condition);
  unlockMutex(&world->render_state.new_chunk_lock);
}


void renderWorld(LoadedWorld* loaded_world, GLuint terrain_shader, Matrix4x4 view, Matrix4x4 projection) {
  Chunk* chunk = (Chunk*) removefromLinkedList(&loaded_world->render_state.dirty_chunks);
  while (chunk != NULL) {
    fillChunkRenderData(chunk, loaded_world);
    chunk = (Chunk*) removefromLinkedList(&loaded_world->render_state.dirty_chunks);
  }
  glUseProgram(terrain_shader);
  GLuint transformLocation = glGetUniformLocation(terrain_shader, "view");
  glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&view);
  transformLocation = glGetUniformLocation(terrain_shader, "projection");
  glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&projection);

  for(int x = 0; x < LOADED_WORLD_SIZE; x++) {
    for(int y = 0; y < LOADED_WORLD_SIZE; y++) {
      for(int z = 0; z < LOADED_WORLD_SIZE; z++) {
        Chunk* c = getChunkRelativeToLoadedWorld(loaded_world, v3i(x, y, z));
        switch (c->render_data.state) {
          case NO_RENDER_DATA:
          case CHUNK_RENDER_DATA_STATE_COUNT:
            break;

          case NOT_PASSED_TO_OPENGL:
            fillChunkVao(c);
            // The break here is missing on purpose, so that a Chunk with a new VAO is rendered.
          case OKAY:
            Matrix4x4 model = translate(v3(x * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE));
            model *= translate(toV3(loaded_world->origin));
            transformLocation = glGetUniformLocation(terrain_shader, "model");
            glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)&model);
            ChunkRenderData* render_data = &c->render_data;
            glBindVertexArray(render_data->vao);
            glDrawArrays(GL_TRIANGLES, 0, render_data->num_vertices);
            break;
        }
      }
    }
  }
}


void shiftLoadedWorld(LoadedWorld* loaded_world, Direction direction) {
  // TODO: This function isn't quite done. The chunks next to the new ones still need to be re rendered, and there are
  // some threading issues when the map is shifted and chunks need to be double re rendered.
  V3i diff = getDirectionVector(direction);
  loaded_world->origin += diff * CHUNK_SIZE;
  loaded_world->wrap_break += diff;
  loaded_world->wrap_break += LOADED_WORLD_SIZE;
  loaded_world->wrap_break %= LOADED_WORLD_SIZE;

  lockMutex(&loaded_world->render_state.new_chunk_lock);
  for(int i = 0; i < LOADED_WORLD_SIZE; i++) {
    for(int j = 0; j < LOADED_WORLD_SIZE; j++) {
      int x = 0, y = 0, z = 0;
      if(diff.x != 0) {
        if(diff.x == -1) {
          x = loaded_world->wrap_break.x;
          y = i;
          z = j;
        } else {
          x = (loaded_world->wrap_break.x + 15) % LOADED_WORLD_SIZE;
          y = i;
          z = j;
        }
      } else if(diff.y != 0) {
        if(diff.y == -1) {
          y = loaded_world->wrap_break.y;
          x = i;
          z = j;
        } else {
          y = (loaded_world->wrap_break.y + 15) % LOADED_WORLD_SIZE;
          x = i;
          z = j;
        }
      } else if(diff.z != 0) {
        if(diff.z == -1) {
          z = loaded_world->wrap_break.z;
          x = i;
          y = j;
        } else {
          z = (loaded_world->wrap_break.z + 15) % LOADED_WORLD_SIZE;
          x = i;
          y = j;
        }
      }
      glDeleteVertexArrays(1, &loaded_world->chunks[x][y][z].render_data.vao);
      setUpNewChunk(loaded_world, v3i(x, y, z));
    }
  }
  signalCondition(&loaded_world->render_state.new_chunk_condition);
  unlockMutex(&loaded_world->render_state.new_chunk_lock);
}

int getMeshAroundPosition(Triangle* mesh_buffer, LoadedWorld* world, V3i from, V3i to) {
  int count = 0;
  for (int x = from.x; x <= to.x; x++) {
    for (int y = from.y; y <= to.y; y++) {
      for (int z = from.z; z <= to.z; z++) {
        V3i cursor = v3i(x, y, z);
        int added = getBlockMesh(mesh_buffer, getBlockAt(world, cursor), cursor);
        mesh_buffer += added;
        count += added;
      }
    }
  }
  return count;
}


