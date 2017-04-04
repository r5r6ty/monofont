# monofont
##### a small pixel font lib depend on freetype lib

**Usage:**

_Init:_

	using namespace MONO;

	MONOFont *font = new MONOFont("微软雅黑.ttf", 26);

	font->createMap(1024,1024);

	glGenTextures(1, &font->getMapTID());
	glBindTexture(GL_TEXTURE_2D, font->getMapTID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, font->getMapWidth(), font->getMapHeight(), 0, GL_ALPHA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
  
_Make a function:_

	void drawtext(const wchar_t *text, MONO::MONOFont *font, float px, float py, float sx = 0.0f, float sy = 0.0f)
	{
		using namespace MONO;
	
		glBindTexture(GL_TEXTURE_2D, font->getMapTID());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, font->getMapWidth(), font->getMapHeight(), GL_ALPHA, GL_UNSIGNED_BYTE, font->getMapData());
		glBindTexture(GL_TEXTURE_2D, 0);
	
		float myX = px;
		float myY = py;
	
		for (unsigned int i = 0; i < wcslen(text); i++)
		{
			MONOGlyph *glyph = font->getGlyph(text[i]);
			if(i >0)
			{
				FT_Vector delta = font->getKerning(text[i - 1], text[i], ft_kerning_unfitted);
				myX += delta.x >> 6;
				myY += delta.y >> 6;
			}
	
			int x0 = myX + glyph->offset_x;
			int y0 = myY + glyph->offset_y;
			int x1 = x0 + glyph->width;
			int y1 = y0 + glyph->height;
	
			float u0 = glyph->s0 / font->getMapWidth();
			float v0 = glyph->t1 / font->getMapHeight();
			float u1 = glyph->s1 / font->getMapWidth();
			float v1 = glyph->t0 / font->getMapHeight();
	
			glBindTexture(GL_TEXTURE_2D, font->getMapTID());
			glBegin(GL_QUADS);
			glTexCoord2f(u0, v0); glVertex3f(x0, y0, 0.0f);
			glTexCoord2f(u0, v1); glVertex3f(x0, y1, 0.0f);
			glTexCoord2f(u1, v1); glVertex3f(x1, y1, 0.0f);
			glTexCoord2f(u1, v0); glVertex3f(x1, y0, 0.0f);
			glEnd();
			glBindTexture(GL_TEXTURE_2D, 0);
	
			myX += glyph->advance_x * (sx + 1);
			myY += glyph->advance_y * sy;
		}
	}

_Call:_

	wchar_t str33[128] = L"中文输出测AVav试abcdefghigklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWKYZ123456789";
	
	drawtext(str33, font, 0.0f, 0.0f);
