BlockModel BLOCK_MODELS[BLOCK_SHAPE_COUNT] = {
{% for block in blocks %}
  { { {{block.triangles}} }, {{block.count}} },
{% endfor %}
};
