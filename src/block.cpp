bool isBlockSolidAtPoint(BlockShape shape, V3 pos) {
  BlockModel model = BLOCK_MODELS[shape];
  if (model.triangle_count == 0) {
    return false;
  }
  // TODO: this is a more generic isPointInConcaveMesh
  for (int triangle_index = 0; triangle_index < model.triangle_count; triangle_index++) {
    if (dot(pos - model.mesh[triangle_index].vertices[0], model.mesh[triangle_index].normal) >= 0.0) {
      return false;
    }
  }
  return true;
}

int getBlockMesh(Triangle* mesh_buffer, Block block, V3i int_pos) {
  V3 pos = toV3(int_pos);
  BlockModel* model = &BLOCK_MODELS[block.block_shape];
  int count = model->triangle_count;
  for (int i = 0; i < count; i++) {
    *mesh_buffer = model->mesh[i];
    mesh_buffer->vertices[0] += pos;
    mesh_buffer->vertices[1] += pos;
    mesh_buffer->vertices[2] += pos;
    mesh_buffer++;
  }
  return count;
}
