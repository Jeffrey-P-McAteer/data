
#include <filesystem>
#include <chrono>

#include "BillBoard.h"

// For string manipulation + system() call
#include "utility.h"


Billboard::Billboard(void)
{
}

Billboard::~Billboard(void)
{
}

void Billboard::ReadFile(string fileName)
{
  // Handle directories by calling .ReadFile() multiple times and pushing
  // this->textureNumber to
  if (std::filesystem::is_directory(fileName)) {
    for (const auto& dir_entry : std::filesystem::directory_iterator(fileName)) {
      if (dir_entry.is_regular_file()) {
        
        this->ReadFile( dir_entry.path() );

        // Push textureNumber into 
        this->textureNumbers.push_back(this->textureNumber);

        //std::cout << "this->textureNumber = " << this->textureNumber << std::endl;
        
      }
    }

    return;
  }

	// Read the texture file, generate the texture object, and configure
	// the texturing parameters.

  // Shell out to imagemagic to do an on-the-fly conversion to PPM
  if (!endsWith(fileName, ".ppm") || endsWith(fileName, ".PPM")) {
    std::string original_texture_file_name(fileName);

    replaceAll(fileName, ".jpg", ".ppm");
    replaceAll(fileName, ".jpeg", ".ppm");
    replaceAll(fileName, ".png", ".ppm");
    replaceAll(fileName, ".JPG", ".ppm");
    replaceAll(fileName, ".JPEG", ".ppm");
    replaceAll(fileName, ".PNG", ".ppm");
    // TODO any other image type possibilities? Shotgunning it here,
    // we could use std::filesystem to parse off an extension dynamically, but this
    // already strays further than the assignment wants.

    if (!std::filesystem::exists(fileName)) {
      std::string cmd;
      cmd.append("convert \"");
      cmd.append(original_texture_file_name);
      cmd.append("\" -flip ");
      cmd.append("\"");
      cmd.append(fileName);
      cmd.append("\"");

      std::cout << "Conversion command: " << cmd << std::endl;
      system(cmd.c_str());
    }
    
  }

  this->textureImage.ReadFile(fileName);

  //std::cout << "fileName=" << fileName << " w=" << this->textureImage.width << " h=" << this->textureImage.height << std::endl;

  glGenTextures(1, &this->textureNumber);
  glBindTexture(GL_TEXTURE_2D, this->textureNumber);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->textureImage.width, this->textureImage.height, 0, 
    GL_RGB, GL_UNSIGNED_BYTE, this->textureImage.image);

}

void Billboard::SetSize(float width, float height)
{
  // Safety: negative values mean calculate the ratio from this->textureImage.width / .height
  if (width > 0.0) {
  	this->width = width;
  }
  if (height > 0.0) {
    this->height = height;
  }
  if (width < 0.0) {
    this->width = this->height * ((double)  this->textureImage.width / (double) this->textureImage.height);
  }
  if (height < 0.0) {
    this->height = this->width * ((double)  this->textureImage.height / (double) this->textureImage.width);
  }
}

void Billboard::SetLocation(Vector3 location)
{
	// Your code
  this->location.x = location.x;
  this->location.y = location.y;
  this->location.z = location.z;
}

void Billboard::SetOrientation(float orientation)
{
	// Your code
  this->orientation = orientation;
}

void Billboard::SetDelayMs(int delay_ms) {
  this->delay_ms = delay_ms;
}

bool Billboard::IsAnimated() {
  return this->textureNumbers.size() > 1;
}

GLuint Billboard::ActiveTextureNumber() {
  if (!this->IsAnimated()) {
    return this->textureNumber;
  }
  std::chrono::milliseconds utc_ms = std::chrono::duration_cast< std::chrono::milliseconds >(
    std::chrono::system_clock::now().time_since_epoch()
  );
  long utc_ms_l = utc_ms.count();
  long num_textures = this->textureNumbers.size();
  long texture_duration_ms = num_textures * this->delay_ms;

  // Time is now constrained to animation loop
  utc_ms_l = utc_ms_l % texture_duration_ms;
  // Dividing by the delay_ms gives us a texture index to return
  int texture_idx = (int) (utc_ms_l / this->delay_ms);
  return this->textureNumbers[texture_idx];
}

void Billboard::Draw()
{
	// Draw the board and pillar.  Use texture mapping for the board only.
	// The pillar is drawn just using simple geometry, i.e., a rectangle.
	// Use glEnable() and glDisable() to change OpenGL states.

  // Performance improvement for animated items
  GLuint active_gl_tex = this->ActiveTextureNumber();

  if (this->IsAnimated()) {
    std::cout << "animated active_gl_tex=" << active_gl_tex << std::endl;
  }

  if (this->gl_texture_num_to_list_ids.count(active_gl_tex)) {
    glCallList(this->gl_texture_num_to_list_ids[active_gl_tex]);
    return;
  }
  else {
    this->gl_texture_num_to_list_ids[active_gl_tex] = glGenLists(1);
    glNewList(this->gl_texture_num_to_list_ids[active_gl_tex], GL_COMPILE);

    float pillar_h = this->location.y;
    float pillar_w = 0.45f;

    // Draw pillar as a rectangle
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glPushMatrix();
      glRotatef(this->orientation, 0, 1, 0);
      glTranslatef(
        this->location.x,
        0.0f, // this->location.y,
        this->location.z
      );
      glScalef(1.0, 1.0, 1.0);

      glBegin(GL_POLYGON);
        glColor3f(0.0f, 0.0f, 0.0f);

        glVertex3f(-pillar_w/2.0f, pillar_h,    pillar_w / 2.0f);
        glVertex3f(0,           pillar_h,    0.0);
        glVertex3f(0,           0.0,       0.0);
        glVertex3f(-pillar_w/2.0f, 0.0,       pillar_w / 2.0f);
        //glVertex3f(-pillar_w/2.0f, pillar_h,    pillar_w / 2.0f);
      glEnd();
      
    glPopMatrix();

    // Now draw texture
    glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, this->textureNumber);
    glBindTexture(GL_TEXTURE_2D, active_gl_tex );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    glDisable(GL_BLEND);
    float rgba_diffuse_reflectance[4] = {0.8, 0.8, 0.8, 1.0}; // default reflectance values
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, rgba_diffuse_reflectance);
    float rgba_ambient_reflectance[4] = {0.2, 0.2, 0.2, 1.0}; // default reflectance values
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, rgba_ambient_reflectance);
    float rgba_spectacular_reflectance[4] = {0.0, 0.0, 0.0, 1.0}; // default reflectance values
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, rgba_spectacular_reflectance);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);


    glPushMatrix();
      glRotatef(this->orientation, 0, 1, 0);
      glTranslatef(
        this->location.x,
        this->location.y,
        this->location.z
      );
      glScalef(1.0, 1.0, 1.0);

      glBegin(GL_POLYGON);
          glColor3f(1, 1, 1);

          glTexCoord2f(0.0, 1.0);
          glVertex3f(-(this->width)/2.0f, this->height,    (this->width) / 2.0f);

          glTexCoord2f(1.0, 1.0);
          glVertex3f((this->width) / 2.0f,                   this->height,    -(this->width) / 2.0f);
          
          glTexCoord2f(1.0, 0.0);
          glVertex3f((this->width) / 2.0f,                   0.0,             -(this->width) / 2.0f);
          
          glTexCoord2f(0.0, 0.0);
          glVertex3f(-(this->width)/2.0f, 0.0,             (this->width) / 2.0f);

      glEnd();

    glPopMatrix();


    glDisable(GL_TEXTURE_2D);

    glEndList();

    // and call the list to render 1st time
    glCallList(this->gl_texture_num_to_list_ids[active_gl_tex]);

  }

}
