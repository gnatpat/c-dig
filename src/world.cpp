Block OOB_BLOCK = { CUBE };


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

void markChunkAsDirty(LoadedWorld* world, Chunk* chunk) {
  LinkedList* dirty_chunk_list_node = (LinkedList*)malloc(sizeof(LinkedList));
  dirty_chunk_list_node->contents = chunk;
  dirty_chunk_list_node->next = world->dirty_chunks;
  world->dirty_chunks = dirty_chunk_list_node;
}

void initWorld(LoadedWorld* world) {
  world->origin = v3i(0, 0, 0);
  for(int x = 0; x < LOADED_WORLD_SIZE; x++) {
    for(int y = 0; y < LOADED_WORLD_SIZE; y++) {
      for(int z = 0; z < LOADED_WORLD_SIZE; z++) {
        BlockShape shape = y >= LOADED_WORLD_SIZE/2 ? AIR : CUBE;
        Chunk* c = &world->chunks[x][y][z];
        initChunk(c, shape);
        c->origin = v3i(x * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE);
        c->render_data.has_render_data = false;
      }
    }
  }
  // We have to mark all chunks as dirty after k
  for(int x = 0; x < LOADED_WORLD_SIZE; x++) {
    for(int y = 0; y < LOADED_WORLD_SIZE; y++) {
      for(int z = 0; z < LOADED_WORLD_SIZE; z++) {
        markChunkAsDirty(world, &world->chunks[x][y][z]);
      }
    }
  }
}

void cleanChunkIfNeeded(LoadedWorld* world) {
  LinkedList* dirty_chunk_list_node = world->dirty_chunks;
  if(dirty_chunk_list_node == NULL) {
    return;
  }
  Chunk* dirty_chunk = (Chunk*)dirty_chunk_list_node->contents;
  world->dirty_chunks = dirty_chunk_list_node->next;
  free(dirty_chunk_list_node);

  fillChunkRenderData(dirty_chunk, world);
}
