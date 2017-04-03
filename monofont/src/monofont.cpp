#include "monofont.h"

namespace MONO
{
	MONOFont::MONOFont(const char *filepath, unsigned int size)
		: m_FilePath(filepath), m_Size(size), m_Map(nullptr)
	{
		init();
	}

	MONOFont::~MONOFont()
	{
		delete[] m_Map->data;
		delete m_Map;
		FT_Done_Face(m_Face);
		FT_Done_FreeType(m_Library);
	}

	void MONOFont::createMap(unsigned int width, unsigned int height)
	{
		m_Map = new MONOMap;
		m_Map->width = width;
		m_Map->height = height;
		m_Map->row = (unsigned int)(m_Map->width / m_Size);
		m_Map->col = (unsigned int)(m_Map->height / m_Size);
		m_Map->TID = 0;
		m_Map->state = MAP_EMPTY;
		m_Map->count = 0;
		m_Map->max = m_Map->row * m_Map->col;
		m_Map->data = new unsigned char[m_Map->width * m_Map->height]{ 0 };
	}

	MONOGlyph *MONOFont::getGlyph(wchar_t ch)
	{
		FT_UInt glyph_index = FT_Get_Char_Index(m_Face, ch);

		
		if (m_Glyphs.find(glyph_index) != m_Glyphs.end())
		{
			return m_Glyphs[glyph_index];
		}
		else
		{
			MONOGlyph *glyph = loadChar(ch);
			return glyph;
		}
	}

	FT_Vector MONOFont::getKerning(wchar_t previous, wchar_t current, FT_Kerning_Mode mode)
	{
		FT_UInt previous_index = FT_Get_Char_Index(m_Face, previous);
		FT_UInt current_index = FT_Get_Char_Index(m_Face, current);
		FT_Vector delta;
		FT_Get_Kerning(m_Face, previous_index, current_index, FT_KERNING_UNFITTED, &delta);
		return delta;
	}

	void MONOFont::listState()
	{
		std::cout << "   Family Name: " << m_Face->family_name << std::endl;

		std::cout << "   Style Name: " << m_Face->style_name << std::endl;

		std::cout << "   Face count: " << m_Face->num_faces << std::endl;

		std::cout << "   glyphs count: " << m_Face->num_glyphs << std::endl;

		std::cout << "   Flags: " << m_Face->face_flags << std::endl;

		std::cout << "   Units_per_EM: " << m_Face->units_per_EM << std::endl;

		std::cout << "   Num_fixed_size: " << m_Face->num_fixed_sizes << std::endl;

		std::cout << "   charmaps: " << m_Face->num_charmaps << std::endl;

		std::cout << "   size: " << m_Face->size << std::endl;

		std::cout << "   memory: " << m_Face->memory << std::endl;

		std::cout << std::endl;
	}

	void MONOFont::listMap()
	{
			std::cout << "width: " << m_Map->width << " height: " << m_Map->height << "row: " << m_Map->row << " col: " << m_Map->col <<std::endl;
			for (unsigned int x = 0; x < m_Map->width + 1; x++)
			{
				const char *jjj;
				if (x < 100 && x >= 10)
				{
					jjj = " ";
				}
				else if (x < 10)
				{
					jjj = "  ";
				}
				else
				{
					jjj = "";
				}
				std::cout << jjj << x;
			}
			std::cout << std::endl;
			for (unsigned int y = 0; y < m_Map->height; y++)
			{
				unsigned int yyy = y + 1;
				const char *jjj;
				if (yyy < 100 && yyy >= 10)
				{
					jjj = " ";
				}
				else if (yyy < 10)
				{
					jjj = "  ";
				}
				else
				{
					jjj = "";
				}
				std::cout << jjj << yyy;
				for (unsigned int x = 0; x < m_Map->width; x++)
				{
					unsigned int iii = m_Map->data[x + y * m_Map->width];
					if ((x % m_Size == m_Size - 1 || y % m_Size == m_Size - 1) && iii == 0)
					{
						std::cout << "  ■" << std::endl;
					}
					else
					{
						const char *jjj;
						if (iii < 100 && iii >= 10)
						{
							jjj = " ";
						}
						else if (iii < 10)
						{
							jjj = "  ";
						}
						else
						{
							jjj = "";
						}
						std::cout << jjj << iii;
					}
				}
				std::cout << std::endl;
			}

		std::cout << "m_Glyphs.size(): " << m_Glyphs.size() << std::endl;
	}

	void MONOFont::listGlyphs()
	{
		std::unordered_map<unsigned int, MONOGlyph*>::iterator iter;
		for (iter = m_Glyphs.begin(); iter != m_Glyphs.end(); iter++)
		{
			std::cout << "unicode: %i\n" << iter->first << std::endl;
			std::cout << "width: %i\n" << iter->second->width << std::endl;
			std::cout << "height: %i\n" << iter->second->height << std::endl;
			std::cout << "offset_x: %i\n" << iter->second->offset_x << std::endl;
			std::cout << "offset_y: %i\n" << iter->second->offset_y << std::endl;
			std::cout << "advance_x: %f\n" << iter->second->advance_x << std::endl;
			std::cout << "advance_y: %f\n" << iter->second->advance_y << std::endl;
			std::cout << "myGlyph.s0: %f\n" << iter->second->s0 << std::endl;
			std::cout << "myGlyph.t0: %f\n" << iter->second->t0 << std::endl;
			std::cout << "myGlyph.s1: %f\n" << iter->second->s1 << std::endl;
			std::cout << "myGlyph.t1: %f\n" << iter->second->t1 << std::endl;
			std::cout << std::endl;
		}
	}

	MONOGlyph *MONOFont::loadChar(wchar_t ch)
	{
		if (m_Map->state == MAP_FULL)
		{
			std::unordered_map<unsigned int, MONOGlyph*>::iterator it;
			for (it = m_Glyphs.begin(); it != m_Glyphs.end();/*it++*/)
			{
				delete it->second;
				it->second = NULL;
				m_Glyphs.erase(it++);
			}
			m_Map->state = MAP_EMPTY;
			m_Map->count = 0;
			//for (unsigned int y = 0; y < m_Map->height; y++)
			//{
			//	for (unsigned int x = 0; x < m_Map->width; x++)
			//	{
			//		m_Map->data[x + y * m_Map->width] = 0;
			//	}
			//}
		}
		//if (m_Map == nullptr || m_Map->state == MAP_FULL)
		//{
		//	createMap();
		//}

		/* 从字符码检索字形索引 */
		FT_UInt glyph_index = FT_Get_Char_Index(m_Face, ch);

		std::unordered_map<unsigned int, MONOGlyph*>::iterator iter;
		for (iter = m_Glyphs.begin(); iter != m_Glyphs.end(); iter++)
		{
			if (glyph_index == iter->first)
			{
				exit(1);
			}
		}

		/* 装载字形图像到字形槽（将会抹掉先前的字形图像） */
		int error = FT_Load_Glyph(m_Face, glyph_index, FT_LOAD_NO_BITMAP);//FT_LOAD_DEFAULT //FT_LOAD_NO_BITMAP //FT_LOAD_MONOCHROME
		if (error)
		{
			std::cout << "Error: " << __FILE__ << " Line: " << __LINE__ << std::endl;
			exit(1);
		}

		//得到字模 
		FT_Glyph glyph;
		if (FT_Get_Glyph(m_Face->glyph, &glyph))
		{
			std::cout << "Error: " << __FILE__ << " Line: " << __LINE__ << std::endl;
			exit(1);
		}

		/* 转换为一个no抗锯齿位图 */
		error = FT_Render_Glyph(m_Face->glyph, ft_render_mode_mono);//ft_render_mode_normal //ft_render_mode_mono //FT_RENDER_MODE_MONO
		if (error)
		{
			std::cout << "Error: " << __FILE__ << " Line: " << __LINE__ << std::endl;
			exit(1);
		}
		FT_Glyph_To_Bitmap(&glyph, ft_render_mode_mono, 0, 1);//ft_render_mode_normal //ft_render_mode_mono
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

		//取位图数据 
		FT_Bitmap& bitmap = bitmap_glyph->bitmap;
		//bitmap.pixel_mode = FT_PIXEL_MODE_MONO;

		MONOGlyph *myGlyph = new MONOGlyph;
		//把位图数据拷贝自己定义的数据区里.这样旧可以画到需要的东西上面了。 
		myGlyph->width = bitmap.width;
		myGlyph->height = bitmap.rows;
		myGlyph->advance_x = m_Face->glyph->advance.x >> 6;
		myGlyph->advance_y = m_Face->size->metrics.y_ppem;
		myGlyph->offset_x = bitmap_glyph->left;
		myGlyph->offset_y = bitmap_glyph->top - bitmap.rows;

		unsigned int cX = (unsigned int)(m_Map->count % m_Map->row);
		unsigned int cY = (unsigned int)(m_Map->count / m_Map->col);
		myGlyph->s0 = cX * m_Size;
		myGlyph->t0 = cY * m_Size;

		myGlyph->s1 = myGlyph->s0 + (float)myGlyph->width;
		myGlyph->t1 = myGlyph->t0 + (float)myGlyph->height;

		//std::cout << "width: " << myGlyph.width << " height: " << myGlyph.height << " pitch: " << bitmap.pitch << std::endl;

		//for (unsigned int y = 0; y < myGlyph->height; y++)
		//{
		//	for (unsigned int x = 0; x < myGlyph->width; x++)
		//	{
		//		unsigned int iii = bitmap.buffer[x + y * myGlyph->width];
		//		const char *jjj;
		//		if (iii < 100 && iii >= 10)
		//		{
		//			jjj = " ";
		//		}
		//		else if (iii < 10)
		//		{
		//			jjj = "  ";
		//		}
		//		else
		//		{
		//			jjj = "";
		//		}
		//		std::cout << jjj << iii << std::endl;
		//	}
		//	std::cout << std::endl;
		//}

		unsigned char *image = new unsigned char[myGlyph->width * myGlyph->height + 8];
		//for each row in the image
		for (unsigned int r = 0; r < myGlyph->height; r++)
		{
			//where does this row go in our texture
			unsigned int index = r * myGlyph->width;

			//if its 1bit
			//if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
			//{
				//for each byte
				for (unsigned int c = 0, byteIndex = r * bitmap.pitch; c < bitmap.width; c += 8, byteIndex++)
				{
					image[index++] = ((bitmap.buffer[byteIndex] & 128) == 128 ? 255 : 0);
					image[index++] = ((bitmap.buffer[byteIndex] &  64) ==  64 ? 255 : 0);
					image[index++] = ((bitmap.buffer[byteIndex] &  32) ==  32 ? 255 : 0);
					image[index++] = ((bitmap.buffer[byteIndex] &  16) ==  16 ? 255 : 0);
					image[index++] = ((bitmap.buffer[byteIndex] &   8) ==   8 ? 255 : 0);
					image[index++] = ((bitmap.buffer[byteIndex] &   4) ==   4 ? 255 : 0);
					image[index++] = ((bitmap.buffer[byteIndex] &   2) ==   2 ? 255 : 0);
					image[index++] = ((bitmap.buffer[byteIndex] &   1) ==   1 ? 255 : 0);
				}
			//}
		}

		//std::cout << std::endl;

		for (unsigned int y = 0; y < myGlyph->height; y++)
		{
			for (unsigned int x = 0; x < myGlyph->width; x++)
			{
				unsigned int offsetx = (unsigned int)myGlyph->s0;
				unsigned int offsety = (unsigned int)myGlyph->t0;

				unsigned int location = (x + offsetx) + (y + offsety) * m_Map->width;

				m_Map->data[location] = image[x + y * myGlyph->width];


				//unsigned int iii = image[x + y *  myGlyph->width];
				//const char *jjj;
				//if (iii < 100 && iii >= 10)
				//{
				//	jjj = " ";
				//}
				//else if (iii < 10)
				//{
				//	jjj = "  ";
				//}
				//else
				//{
				//	jjj = "";
				//}
				//std::cout << jjj << iii std::endl;
			}
			//std::cout << std::endl;
		}
		//std::cout << std::endl;
		delete[] image;
		FT_Done_Glyph(glyph);
		m_Map->count++;
		if (m_Map->count >= m_Map->max)
		{
			m_Map->state = MAP_FULL;
		}
		else
		{
			m_Map->state = MAP_FREE;
		}
		m_Glyphs.insert(std::pair<unsigned int, MONOGlyph*>(glyph_index, myGlyph));
		return myGlyph;
	}

	int MONOFont::init()
	{
		if(FT_Init_FreeType(&m_Library))
		{
			std::cout << "Error: " << __FILE__ << " Line: " << __LINE__ << std::endl;
			exit(1);
		}
		unsigned int error = FT_New_Face(m_Library, m_FilePath, 0, &m_Face);
		if (error == FT_Err_Unknown_File_Format)
		{
			std::cout << "Error: " << __FILE__ << " Line: " << __LINE__ << std::endl;
			exit(1);
		}
		else if (error)
		{
			std::cout << "Error: " << __FILE__ << " Line: " << __LINE__ << std::endl;
			exit(1);
		}

		FT_Select_Charmap(m_Face, FT_ENCODING_UNICODE);

		//FT_Set_Char_Size(m_Face, 0, m_w << 6, 96, 96);
		FT_Set_Pixel_Sizes(m_Face, m_Size, m_Size);


		return 1;
	}
}