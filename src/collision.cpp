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
  float proportion_through_line = 
    (dot(velocity, line_direction) * solution + dot(sphere_pos - line_start, line_direction)) / line_length;

  if(proportion_through_line < 0.0 || proportion_through_line > 1.0) {
    collision.collided = false;
    return collision;
  }

  collision.collided = true;
  collision.time = solution;
  collision.collision_point = line_start + proportion_through_line * line_direction;
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

