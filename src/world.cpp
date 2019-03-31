Block OOB_BLOCK = { CUBE };


Block getBlockAt(LoadedWorld* world, V3i pos) {
  V3i loaded_pos = pos - world->origin;
  V3i chunk_pos = loaded_pos / CHUNK_SIZE;
  V3i in_chunk_pos = loaded_pos % CHUNK_SIZE;
  if(loaded_pos.x < 0 or loaded_pos.y < 0 or loaded_pos.z < 0 or
    chunk_pos.x >= LOADED_WORLD_SIZE or chunk_pos.y >= LOADED_WORLD_SIZE or chunk_pos.z >= LOADED_WORLD_SIZE) {
    return OOB_BLOCK;
  }
  Chunk* chunk = getChunkRelativeToLoadedWorld(world, chunk_pos);
  return getBlockAt(chunk, in_chunk_pos);
}

bool isPointAir(LoadedWorld* world, V3 pos) {
  BlockShape shape = getBlockAt(world, toV3i(pos)).block_shape;
  return isPointAir(shape, fractional_part(pos.x), fractional_part(pos.y), fractional_part(pos.z));
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


void renderWorld(LoadedWorld* loaded_world, GLuint terrain_shader, Matrix4x4* view, Matrix4x4* projection) {
  glUseProgram(terrain_shader);
  GLuint transformLocation = glGetUniformLocation(terrain_shader, "view");
  glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)view);
  transformLocation = glGetUniformLocation(terrain_shader, "projection");
  glUniformMatrix4fv(transformLocation, 1, GL_TRUE, (float*)projection);

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



