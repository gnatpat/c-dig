void initBasicRenderObject(BasicRenderObject* debug_triangles, int size);
void blowOutRenderObject(BasicRenderObject* basic_render_object, float distance);
void markDirty(BasicRenderObject* basic_render_object);
void render(BasicRenderObject* basic_render_object, GLuint shader, Matrix4x4 view, Matrix4x4 projection, V3 col);
