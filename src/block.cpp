float getBlockHeightAtPoint(BlockShape shape, float x, float z) {
  assert(x >= 0 && x <= 1 && z >= 0 && z <= 1);
  
  switch(shape) {
    case AIR:
      return 0.0f;

    case CUBE:
    case POS_Z_NEG_Y_SLOPE:
    case POS_X_NEG_Y_SLOPE:
    case NEG_Z_NEG_Y_SLOPE:
    case NEG_X_NEG_Y_SLOPE:
    case POS_POS_NEG_CORNER:
    case NEG_POS_NEG_CORNER:
    case NEG_POS_POS_CORNER:
    case POS_POS_POS_CORNER:
    case POS_NEG_NEG_CORNERLESS:
    case NEG_NEG_NEG_CORNERLESS:
    case NEG_NEG_POS_CORNERLESS:
    case POS_NEG_POS_CORNERLESS:
      return 1.0f;

    case POS_Z_POS_Y_SLOPE:
      return 1.0f-z;

    case POS_NEG_NEG_CORNER:
      if(z > x) return 0.0f;
      return x-z;

    case POS_POS_NEG_CORNERLESS:
      if (z > x) return 1.0f;
      return z-x+1.0f;

    case POS_NEG_DIAGONAL:
      return (z > x) ? 0.0f : 1.0f;

    case POS_X_POS_Y_SLOPE:
    case NEG_Z_POS_Y_SLOPE:
    case NEG_X_POS_Y_SLOPE:
    case NEG_NEG_NEG_CORNER:
    case NEG_NEG_POS_CORNER:
    case POS_NEG_POS_CORNER:
    case NEG_POS_NEG_CORNERLESS:
    case NEG_POS_POS_CORNERLESS:
    case POS_POS_POS_CORNERLESS:
    case NEG_NEG_DIAGONAL:
    case NEG_POS_DIAGONAL:
    case POS_POS_DIAGONAL:
      // NOTE: this is a solution that simplifies the code, but this might casuse 4 function calls, which might be slow.
      return getBlockHeightAtPoint((BlockShape)((int)shape-1), 1-z, x);

    case BLOCK_SHAPE_COUNT:
      assert(false);
      return 0.0f;
  }
  assert(false);
  return 0.0f;
}

bool isPointAir(BlockShape shape, float x, float y, float z) {
  // TODO: this doesn't account for blocks with empty parts underneath them.
  assert(x >= 0 && x <= 1 && y >= 0 && y <= 1 && z >= 0 && z <= 1);
  return y >= getBlockHeightAtPoint(shape, x, z);
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
