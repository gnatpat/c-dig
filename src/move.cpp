float VELOCITY_EPSILON = 0.001;

MoveResult move(V3 position, V3 velocity, Triangle* triangles, int triangle_count, int depth, bool moving_xz) {
  MaybeCollision first_collision;
  first_collision.collided = false;
  first_collision.time = 2.0;

  for (int i = 0; i < triangle_count; i++) {
    Triangle triangle = triangles[i];

    MaybeCollision triangle_collision = triangleCollision(triangle, position, velocity);
    if (triangle_collision.collided) {
      first_collision = earliestCollision(first_collision, triangle_collision);
      continue;
    }

    V3* vs = &triangle.vertices[0];
    first_collision = earliestCollision(first_collision, vertexCollision(vs[0], position, velocity));
    first_collision = earliestCollision(first_collision, vertexCollision(vs[1], position, velocity));
    first_collision = earliestCollision(first_collision, vertexCollision(vs[2], position, velocity));
    first_collision = earliestCollision(first_collision, lineCollision(vs[0], vs[1], position, velocity));
    first_collision = earliestCollision(first_collision, lineCollision(vs[0], vs[2], position, velocity));
    first_collision = earliestCollision(first_collision, lineCollision(vs[1], vs[2], position, velocity));
  }

  if (!first_collision.collided) {
    V3 new_position = position + velocity;
    return { new_position, false, first_collision };
  }

  V3 sphere_position_at_collision = position + first_collision.time * velocity;
  V3 new_position = sphere_position_at_collision - normalise(velocity) * VELOCITY_EPSILON;
  V3 remaining_velocity = (1-first_collision.time) * velocity;
  V3 sliding_plane_normal = normalise(sphere_position_at_collision - first_collision.collision_point);

  bool on_ground;
  if(moving_xz) {
    on_ground = false;
    remaining_velocity -= dot(remaining_velocity, sliding_plane_normal) * sliding_plane_normal;
  } else {
    on_ground = (sliding_plane_normal.y > 0.2);
    remaining_velocity = v3(0, 0, 0);
  }

  if(len(remaining_velocity) < VELOCITY_EPSILON) {
    return { new_position, on_ground, first_collision };
  }

  return move(new_position, remaining_velocity, triangles, triangle_count, depth+1, moving_xz);
}


MoveResult move(V3 position, V3 velocity, Triangle* triangles, int triangle_count, bool moving_xz) {
  return move(position, velocity, triangles, triangle_count, 0, moving_xz);
}
