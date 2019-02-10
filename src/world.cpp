Block OOB_BLOCK = { AIR };


Block getBlockAt(LoadedWorld* world, V3i pos) {
  V3i loaded_pos = pos - world->origin;
  V3i chunk_pos = loaded_pos / CHUNK_SIZE;
  V3i in_chunk_pos = loaded_pos % CHUNK_SIZE;
  if(loaded_pos.x < 0 or loaded_pos.y < 0 or loaded_pos.z < 0 or
     chunk_pos.x >= LOADED_WORLD_SIZE or chunk_pos.y >= LOADED_WORLD_SIZE or chunk_pos.z >= LOADED_WORLD_SIZE) {
    return OOB_BLOCK;
  }
  Chunk* chunk = getChunkAt(world, chunk_pos);
  return getBlockAt(chunk, in_chunk_pos);
}


void initWorld(LoadedWorld* world) {
  world->origin = v3i(0, 0, 0);
  world->render_state.new_chunks = NULL;
  world->render_state.dirty_chunks = NULL;

  lockMutex(&world->render_state.new_chunk_lock);
  for(int x = 0; x < LOADED_WORLD_SIZE; x++) {
    for(int y = 0; y < LOADED_WORLD_SIZE; y++) {
      for(int z = 0; z < LOADED_WORLD_SIZE; z++) {
        Chunk* c = &world->chunks[x][y][z];
        c->origin = v3i(x * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE);
        c->render_data.state = NO_RENDER_DATA;
        addToLinkedList(&world->render_state.new_chunks, c);
      }
    }
  }
  signalCondition(&world->render_state.new_chunk_condition);
  unlockMutex(&world->render_state.new_chunk_lock);
}


void cleanChunkIfNeeded(LoadedWorld* world) {
  Chunk* dirty_chunk = (Chunk*) removefromLinkedList(&world->render_state.dirty_chunks);
  if(dirty_chunk == NULL) {
    return;
  }
  fillChunkRenderData(dirty_chunk, world);
}
