MaybeCollision vertexCollision(V3 vertex, V3 sphere_pos, V3 velocity) {
  MaybeCollision collision;

  V3 distance = sphere_pos - vertex;
  float a = lenSq(velocity);
  float b = 2 * dot(distance, velocity);
  float c = lenSq(distance) - 1;

  QuadraticSolutions solutions = solveQuadratic(a, b, c);
  if (!solutions.solution_exists || solutions.max < 0.0f || solutions.min > 1.0f) {
    collision.collided = false;
    return collision;
  }

  collision.collided = true;
  collision.collision_point = vertex;
  collision.collision_type = POINT_COLLISION;

  if (solutions.min < 0.0) {
    collision.time = solutions.max;
    return collision;
  }
  collision.time = solutions.min;
  return collision;
}


MaybeCollision lineCollision(V3 line_start, V3 line_end, V3 sphere_pos, V3 velocity) {
  MaybeCollision collision;

  V3 line_direction = normalise(line_end - line_start);
  V3 u = velocity - dot(velocity, line_direction) * line_direction;
  V3 q = (sphere_pos - line_start) - dot(sphere_pos - line_start, line_direction) * line_direction;

  float a = lenSq(u);
  float b = 2 * dot(u, q);
  float c = lenSq(q) - 1;

  QuadraticSolutions solutions = solveQuadratic(a, b, c);
  if (!solutions.solution_exists || solutions.max < 0.0f || solutions.min > 1.0f) {
    collision.collided = false;
    return collision;
  }

  float solution;

  if (solutions.min < 0.0) {
    solution = solutions.max;
  } else {
    solution = solutions.min;
  }
  if(isnan(solution)) {
    printf("quadratic solver returned nan in linecollision.\n");
  }

  float line_length = len(line_end - line_start);
  V3 collision_point = sphere_pos + velocity * solution;
  float proportion_through_line = dot(collision_point - line_start, line_direction) / line_length;

  if(proportion_through_line < 0.0 || proportion_through_line > 1.0) {
    collision.collided = false;
    return collision;
  }

  collision.collided = true;
  collision.time = solution;
  collision.collision_point = line_start + proportion_through_line * line_direction * line_length;
  collision.collision_type = LINE_COLLISION;
  collision.line_info = { line_start, line_end };
  return collision;
}

MaybeCollision triangleCollision(Triangle triangle, V3 sphere_pos, V3 velocity) {
  MaybeCollision collision;

  float relative_velocity = dot(triangle.normal, velocity);
  float signed_distance = signedDistanceFromTrianglePlane(triangle, sphere_pos);
  float t0, t1;
  if (relative_velocity == 0.0) {
    if (fabsf(signed_distance) < 1.0) {
      t0 = 0.0;
      t1 = 1.0;
    } else {
      collision.collided = false;
      return collision;
    }
  } else {
    t0 = (1 - signed_distance) / (relative_velocity);
    t1 = (-1 - signed_distance) / (relative_velocity);
  }
  if (t0 > t1 || t0 > 1.0 || t1 < 0.0) {
    collision.collided = false;
    return collision;
  }

  V3 plane_intersection_point = sphere_pos - triangle.normal + t0 * velocity;
  if(!isPointInTriangle(triangle, plane_intersection_point)) {
    collision.collided = false;
    return collision;
  }
  collision.collided = true;
  collision.time = t0;
  collision.collision_point = plane_intersection_point;
  collision.collision_type = FACE_COLLISION;
  collision.triangle_info = triangle;
  return collision;
}


MaybeCollision earliestCollision(MaybeCollision r1, MaybeCollision r2) {
  if (!r2.collided || r2.time < 0 || r2.time > 1) {
    return r1;
  }
  if (!r1.collided || r1.time < 0 || r1.time > 1) {
    return r2;
  }
  if (r1.time < r2.time) {
    return r1;
  }
  return r2;
}

