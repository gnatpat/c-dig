float VELOCITY_EPSILON = 0.000001;
V3 move(V3 position, V3 velocity, Triangle* triangles, int triangle_count, int depth) {
  printf("Moving from ");
  printV3(position);
  printf(" with velocity ");
  printV3(velocity);
  printf(".\n");
  MaybeCollision first_collision;
  first_collision.collided = false;
  first_collision.time = 2.0;

  for (int i = 0; i < triangle_count; i++) {
    Triangle triangle = triangles[i];

    float relative_velocity = dot(triangle.normal, velocity);
    float signed_distance = signedDistanceFromTrianglePlane(triangle, position);
    float t0, t1;
    if (relative_velocity == 0.0) {
      if (fabsf(signed_distance) < 1.0) {
        t0 = 0.0;
        t1 = 1.0;
      } else {
        continue;
      }
    } else {
      t0 = (1 - signed_distance) / (relative_velocity);
      t1 = (-1 - signed_distance) / (relative_velocity);
    }
    if (t0 > t1) {
      continue;
    }
    if (t0 > 1.0 || t1 < 0.0) {
      continue;
    }

    V3 plane_intersection_point = position - triangle.normal + t0 * velocity;
    if(isPointInTriangle(triangle, plane_intersection_point)) {
      MaybeCollision collision;
      collision.collided = true;
      collision.time = t0;
      collision.collision_point = plane_intersection_point;

      first_collision = earliestCollision(first_collision, collision);
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
    printf("No collision.\n");
    return new_position;
  }
  printf("Collision at %f!\n", first_collision.time);

  V3 sphere_position_at_collision = position + first_collision.time * velocity;

  V3 sliding_plane_normal = normalise(sphere_position_at_collision - first_collision.collision_point);
  V3 remaining_velocity = (1-first_collision.time) * velocity;
  printV3(remaining_velocity);
  printf(" remaining before slide.\n");
  remaining_velocity -= dot(remaining_velocity, sliding_plane_normal) * sliding_plane_normal;
  printV3(remaining_velocity);
  printf(" remaining after slide.\n");

  V3 new_position = sphere_position_at_collision - normalise(velocity) * VELOCITY_EPSILON;

  if(len(remaining_velocity) < VELOCITY_EPSILON) {
    return new_position;
  }

  return move(new_position, remaining_velocity, triangles, triangle_count, depth+1);
}


V3 move(V3 position, V3 velocity, Triangle* triangles, int triangle_count) {
  return move(position, velocity, triangles, triangle_count, 0);
}
