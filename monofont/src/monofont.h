#pragma once
#include <unordered_map>
#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H 
#include FT_GLYPH_H
#include FT_OUTLINE_H

namespace MONO
{
	enum map_state
	{
		MAP_ERROR = 0,
		MAP_EMPTY,
		MAP_FREE,
		MAP_FULL
	};

	struct MONOMap
	{
		unsigned int width;
		unsigned int height;
		unsigned int row;
		unsigned int col;
		unsigned int TID;
		unsigned int count;
		unsigned int max;
		unsigned int state;
		unsigned char *data;
	};

	struct MONOGlyph
	{
		unsigned int width;
		unsigned int height;
		int offset_x;
		int offset_y;
		float advance_x;
		float advance_y;
		float s0;
		float t0;
		float s1;
		float t1;
	};

	class MONOFont
	{
	private:
		FT_Library m_Library;
		FT_Face m_Face;

		const char *m_FilePath;
		unsigned int m_Size;
		MONOMap* m_Map;
		std::unordered_map<unsigned int, MONOGlyph*> m_Glyphs;
	public:
		MONOFont(const char *filepath, unsigned int size);
		~MONOFont();

		void createMap(unsigned int width, unsigned int height);
		MONOGlyph *getGlyph(wchar_t ch);
		FT_Vector getKerning(wchar_t previous, wchar_t current, FT_Kerning_Mode mode);

		void listState();
		void listMap();
		void listGlyphs();

		inline const unsigned char &getSize() const { return m_Size; }
		inline const unsigned int &getMapWidth() const { return m_Map->width; }
		inline const unsigned int &getMapHeight() const { return m_Map->height; }
		inline unsigned int &getMapTID() const { return m_Map->TID; }
		inline const unsigned int &getMapCount() const { return m_Map->count; }
		inline const unsigned int &getMapMax() const { return m_Map->max; }
		inline const unsigned char *getMapData() const { return m_Map->data; }
	private:
		MONOGlyph *loadChar(wchar_t ch);
		int init();
	};
}