void initBasicRenderObject(BasicRenderObject* debug_triangles, int max_triangle_count);
void blowOutRenderObject(BasicRenderObject* basic_render_object, float distance);
void markDirty(BasicRenderObject* basic_render_object);
void render(BasicRenderObject* basic_render_object, GLuint shader, Matrix4x4 view, Matrix4x4 projection);
void render(BasicRenderObject* basic_render_object, GLuint shader, Matrix4x4 model, Matrix4x4 view, Matrix4x4 projection);
void make3dTriangle(ObjectTriangle* buffer, V3 direction, V3 perp_dir, V3 colour, float offset_fraction);
