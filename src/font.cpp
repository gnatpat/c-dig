void loadFontDataOrDie(FontData* font_data, const char* filepath) {
  FILE *fp = fopen(filepath, "r");
  if (fp == NULL) {
    printf("Couldn't open %s.\n", filepath);
  }

  // Skip first config line
  fscanf(fp, "%*[^\n]\n");
  
  fscanf(fp, "common lineHeight=%d base=%d scaleW=%d scaleH=%d",
      &font_data->line_height,
      &font_data->line_base,
      &font_data->texture_width,
      &font_data->texture_height);
  fscanf(fp, "%*[^\n]\n");

  fscanf(fp, "%*[^\n]\n");

  int char_count;
  fscanf(fp, "chars count=%d\n", &char_count);

  for(int i = 0; i < char_count; i++) {
    FontCharData char_data;
    int char_id;
    fscanf(fp,
          "char id=%d x=%d y=%d width=%d height=%d xoffset=-%d yoffset=%d xadvance=%d",
           &char_id,
           &char_data.x,
           &char_data.y,
           &char_data.width,
           &char_data.height,
           &char_data.x_offset,
           &char_data.y_offset,
           &char_data.x_advance);
    fscanf(fp, "%*[^\n]\n");
    font_data->chars[char_id] = char_data;
  }

  for(int i = 0; i < 128; i++) {
    for(int j = 0; j < 128; j++) {
      font_data->kerning[i][j] = 0;
    }
  }

  int kerning_count;
  fscanf(fp, "kernings count=%d\n", &kerning_count);
  for(int i = 0; i < kerning_count; i++) {
    int first, second, amount;
    fscanf(fp, "kerning first=%d second=%d amount=%d\n", &first, &second, &amount);
    font_data->kerning[first][second] = amount;
  }

  fclose(fp);
}

void loadFontOrDie(Font* font, const char* font_texture_filepath, const char* font_data_filepath) {
  font->texture = loadTexture(font_texture_filepath);
  assert(font->texture != 0);
  loadFontDataOrDie(&font->data, font_data_filepath);
}
