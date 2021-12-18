#include "text_renderer.h"
#include "file_system.h"

#include "window.h"
#include "profile.h"
#include <assert.h>
#include <script_system.h>
#include "file_loading.h"
#include "utils.h"
#include "input_manager.h"
#include "graphics/bitmap.h"
#include "resource_manager.h"
#include "sprite_renderer.h"

std::vector<uint32_t> s_EmotionIDs = { 0x4D3D1188,0x6E7A8E8F,0x403105F2,0xD3C23894,0xEDD63AB1,0xC8AA7848,0xA5D718B1,0xE0C6F0D3,0x572F2A4D,0xA1E13E27,0xB2F4A198,0xEDEBCFCF,0x3B3D19C0,0x9EEC6DE4,0x1B1B8326,0x525FCCF9,0xAD9E8BAD,0xE9A1E271,0x1C7C95C4,0x1500E768,0x30615DBC,0x3694C64F,0xFD438646,0x4FAD347C,0x743AF90F,0x853F3BC9,0xD6436048,0x74E0F5FA,0x8E0063E2,0x5BA9CF5E,0xE8E08FA9,0x888536BF,0xBEDE7D41,0xF06B6B9E,0x58FAA400,0x270D5C71,0xE5FF2DE2,0xBE3150EE,0x11C5EA40,0x73F3BF9D,0xCCD6B7E8,0x66D0E07C,0x9A8BFB91,0xCA47B474,0x590CAA9B,0x4E20C2E6,0x44B657A9,0x978F8F8A,0x522BC68F,0xA8A9B15D,0xE53DE56A,0xE88B5354,0x0417C932,0xC699AB3E,0x19CA9706,0xFCD58523,0xCD8F0AD6,0x978B9123,0x0E658C4C,0x12BE1C3E,0x85AC8CCB,0x707ABF50,0x58C9FAB0,0xAA7B3B42,0xF2FBDA6E,0x4D3D1188,0x4D3D1188,0x4D3D1188,0x4D3D1188,0x4D3D1188,0x4D3D1188,0x4D3D1188,0x4D3D1188,0x4D3D1188,0xFC4215EC,0xD086F684,0xFCCAA9B5,0xACE9C474,0x87621B9F,0xCDC95381,0x396C4E03,0xB06B70C0,0xADE1576E,0xFB472367,0xEDA67286,0x15CA26D9,0xDC9C1E87,0xB5786848,0xC2A7A47D,0x7EEB3422,0x8F20BE2E,0xA1E7B566,0x11729962,0xEF498C25,0xF95512DC,0xF5509B1C,0x7F869E1E,0x107CF5F3,0xF45DCF6A,0x99AFED62,0x4D3D1188,0xC8BBEEA3,0x225ECF82,0xD5C14B62,0xA8BC861D,0x7229A70C,0x4FF6E07A,0xDF1F56AC,0x488EBBD6,0x4806AE3B,0x09574327,0x7A9F28C7,0xB7E060C1,0x5887677B };


static int s_FontSizeSmall = 12;
static int s_FontSizeMiddle = 14;
static int s_FontSizeLarge = 16;

static const uint32_t s_ChineseRanges[] =
{
	0x0020, 0x00FF, // Basic Latin + Latin Supplement
	0x3000, 0x30FF, // Punctuations, Hiragana, Katakana
	0x31F0, 0x31FF, // Katakana Phonetic Extensions
	0xFF00, 0xFFEF, // Half-width characters
	0x4e00, 0x9FAF, // CJK Ideograms
	0,
};

bool TextRenderer::IsInRange(wchar_t c)
{
	for (int i = 0; s_ChineseRanges[i]; i += 2)
	{
		uint32_t l = s_ChineseRanges[i];
		uint32_t r = s_ChineseRanges[i + 1];
		if (c >= l && c <= r)
			return true;
	}
	return false;
}


TextRenderer::TextRenderer()
{
	// Configure VAO/VBO for texture quads
	m_pShader = new Shader("text.vs", "text.fs");
	glm::mat4 projection = glm::ortho(0.0f, WINDOW_INSTANCE->GetWidth() * 1.0f, 0.0f, WINDOW_INSTANCE->GetHeight() * 1.0f, -1.0f, 1.0f);
	m_pShader->Bind();
	glUniformMatrix4fv(glGetUniformLocation(m_pShader->GetProgramID(), "projection"), 1, GL_FALSE, (GLfloat*)&projection);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	m_pShader->Unbind();

	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&m_FtLib))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;


	if (FT_New_Face(m_FtLib, FileSystem::GetGameFontPath().c_str(), 0, &m_FtFace))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	//12 14 16 18 15 13
	/*for (size_t i=0;i< m_FtFace->num_fixed_sizes;i++)
	{
		FT_Bitmap_Size fsize = m_FtFace->available_sizes[i];
	}*/
	// Set size to load glyphs as
	m_ScreeHeight = WINDOW_INSTANCE->GetHeight();

	m_FontSize = 12;

	FT_Set_Char_Size(m_FtFace, 0, m_FontSize * 64, 0, 0);

	// m_IsScaleable =  m_FtLib xxx is scale bn
	m_FontHeight = m_FtFace->size->metrics.height >> 6;

	m_BearingY = m_FtFace->size->metrics.ascender >> 6;

	m_bUseKerning = FT_HAS_KERNING(m_FtFace);

	//for (size_t i=0;s_ChineseRanges[i];i+=2)
	//{
	//	wchar_t l = s_ChineseRanges[i];
	//	wchar_t e = s_ChineseRanges[i+1];
	//	while(l++<=e)
	//	{
	//		// Load character glyph 
	//		if (FT_Load_Char(m_FtFace, l, FT_LOAD_RENDER))
	//		{
	//			std::cout << "ERROR::FREETYTPE: Failed to load Glyph :" << (l) << std::endl;
	//			
	//			continue;
	//		}
	//	}
	//}

	/*for (auto id : s_EmotionIDs)
	{
		uint64_t longID = RESOURCE_MANAGER_INSTANCE->EncodeWAS(WZIFEWDF, id);
		RESOURCE_MANAGER_INSTANCE->LoadWASSprite(longID);
	}*/
}

TextRenderer::~TextRenderer()
{
	for (auto& it : m_FontGlyphs)
	{
		if (it.second->Texture != nullptr)
		{
			delete it.second->Texture;
			it.second->Texture = nullptr;
		}
		delete it.second;
		it.second = nullptr;
	}
	m_FontGlyphs.clear();

	// Destroy FreeType once we're finished
	FT_Done_Face(m_FtFace);
	FT_Done_FreeType(m_FtLib);
}

void TextRenderer::EnsureLoadText(std::wstring text, bool showEmotion)
{
	EnsureLoadText(std::vector<uint32_t>(text.begin(), text.end()), showEmotion);
}

void TextRenderer::EnsureLoadText(std::vector<uint32_t> unicodes, bool showEmotion)
{
	for (size_t i = 0; i < unicodes.size(); i++)
	{
		uint32_t charcode = -1;
		int cnt = GetCharcode(unicodes, i, charcode, showEmotion);
		if (cnt == 0)
		{
			LoadGlyph(charcode);
		}
		else
		{
			LoadGlyph(charcode, true);
			i += cnt;
		}
	}
}

int TextRenderer::GetCharcode(const std::vector<uint32_t>& unicodes, size_t i, uint32_t& charcode, bool showEmotion)
{
	if (unicodes[i] != '#' || !showEmotion)
	{
		charcode = unicodes[i];
		return 0;
	}
	else
	{
		std::vector<uint32_t> numbers;
		for (size_t j = i + 1; j < i + 4 && j < unicodes.size(); j++)
		{
			int num = unicodes[j] - '0';
			if (num >= 0 && num <= 9)
			{
				numbers.push_back(num);
			}
			else
			{
				break;
			}
		}

		size_t cnt = numbers.size();
		if (cnt != 0)
		{
			uint32_t index = 0;
			for (size_t n = 0; n < cnt; n++)
			{
				index = index * 10;
				index += numbers[n];
			}
			if (index >= s_EmotionIDs.size())
			{
				charcode = unicodes[i];
				return 0;
			}
			else
			{
				charcode = s_EmotionIDs[index];
				return (int)cnt;
			}

		}
		else
		{
			charcode = unicodes[i];
			return 0;
		}
	}
}

int TextRenderer::GetCharcode(const std::wstring& text, size_t i, uint32_t& charcode, bool showEmotion)
{
	std::vector<uint32_t> unicodes(text.begin(), text.end());
	return GetCharcode(unicodes, i, charcode, showEmotion);
}

void TextRenderer::DrawGlyphTexture(uint32_t id, float xpos, float ypos, float w, float h)
{
	m_pShader->Bind();

	GLfloat vertices[6][4] = {
		{ xpos,     ypos + h,   0.0,0.0 },
	{ xpos,     ypos,       0.0,1.0 },
	{ xpos + w, ypos,       1.0, 1.0 },
	{ xpos,     ypos + h,   0.0, 0.0 },
	{ xpos + w, ypos,       1.0, 1.0 },
	{ xpos + w, ypos + h,   1.0, 0.0 }
	};
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_pShader->Unbind();
}

bool TextRenderer::LoadGlyph(uint32_t charcode, bool emotion)
{
	if (emotion)
	{
		if (m_FontGlyphs.count(charcode) > 0 && m_FontGlyphs[charcode]->IsEmotion)
			return true;
	}
	else
	{
		if (m_FontGlyphs.count(charcode) > 0 && !m_FontGlyphs[charcode]->IsEmotion)
			return true;
	}

	auto* fontglyph = new FontGlyph();
	fontglyph->IsEmotion = emotion;
	fontglyph->CharCode = charcode;

	if (emotion)
	{
		auto longID = RESOURCE_MANAGER_INSTANCE->EncodeWAS(WZIFEWDF, charcode);
		RESOURCE_MANAGER_INSTANCE->LoadWASSpriteByID(longID, true);
		//FrameAnimation* pEmotion = new FrameAnimation(longID);
		//fontglyph->Emotion = pEmotion;
		//fontglyph->GlyphIndex = 0;
		//fontglyph->Advance = pEmotion->GetWidth();

		/*fontglyph->Left =  pEmotion->GetKeyX();
		fontglyph->Top = pEmotion->GetKeyY();
		fontglyph->Width = pEmotion->GetWidth();
		fontglyph->Height = pEmotion->GetHeight();*/
	}
	else
	{
		fontglyph->GlyphIndex = FT_Get_Char_Index(m_FtFace, charcode);
		auto error = FT_Load_Glyph(m_FtFace, fontglyph->GlyphIndex, FT_LOAD_RENDER);
		if (error)
			return false;

		fontglyph->Advance = m_FtFace->glyph->advance.x >> 6;

		fontglyph->Left = m_FtFace->glyph->bitmap_left;
		fontglyph->Top = m_FtFace->glyph->bitmap_top - m_FtFace->glyph->bitmap.rows;
		fontglyph->Width = m_FtFace->glyph->bitmap.width;
		fontglyph->Height = m_FtFace->glyph->bitmap.rows;
		m_pShader->Bind();
		fontglyph->Texture = new Texture(
			Texture::Text,
			m_FtFace->glyph->bitmap.buffer,
			fontglyph->Left,
			fontglyph->Top,
			fontglyph->Width,
			fontglyph->Height
		);
		m_pShader->Unbind();
		if (!fontglyph->Texture)
			return false;
	}
	m_FontGlyphs.insert({ charcode,fontglyph });
	return true;
}

int TextRenderer::MeasureRange(std::wstring text, int start, int end, bool showEmotion)
{
	EnsureLoadText(text);
	int pen_x = 0;
	int previous = 0;
	int kerning = 0;
	for (int n = start; n < end; n++)
	{
		uint32_t charcode = -1;
		int cnt = GetCharcode(text, n, charcode, showEmotion);
		n += cnt;
		if (charcode == '\n') continue;
		auto& fontGlyph = m_FontGlyphs[charcode];
		if (m_bUseKerning && previous && fontGlyph->GlyphIndex)
		{
			FT_Vector  delta;
			FT_Get_Kerning(m_FtFace, previous, fontGlyph->GlyphIndex,
				FT_KERNING_DEFAULT, &delta);
			kerning = delta.x >> 6;
		}
		pen_x = pen_x + fontGlyph->Advance + kerning;
		previous = fontGlyph->GlyphIndex;
	}
	return pen_x;
}

TextRenderer::MeasureInfo TextRenderer::MeasureText(std::wstring text, int bound_width, bool showEmotion)
{
	EnsureLoadText(text);

	std::vector<int> lineSteps;
	lineSteps.push_back(0);
	int pen_x = 0;
	int pen_y = 0;
	int total_width = 0;
	int total_height = m_FontHeight;
	int previous = 0;
	int kerning = 0;
	for (size_t n = 0; n < text.size(); n++)
	{
		uint32_t charcode = -1;
		int cnt = GetCharcode(text, n, charcode, showEmotion);
		n += cnt;
		const auto& fontGlyph = m_FontGlyphs[charcode];
		if ((charcode == '\n') ||
			(bound_width > 0 && pen_x + kerning + fontGlyph->Advance >= bound_width))
		{
			total_width = total_width + pen_x;
			pen_x = 0;
			pen_y += m_FontHeight;
			total_height += m_FontHeight;
			lineSteps.push_back((int)n);
			if (charcode != '\n')
			{
				n--;
			}
			previous = 0;
			kerning = 0;
		}
		else
		{
			if (m_bUseKerning && previous && fontGlyph->GlyphIndex)
			{
				FT_Vector  delta;
				FT_Get_Kerning(m_FtFace, previous, fontGlyph->GlyphIndex,
					FT_KERNING_DEFAULT, &delta);
				kerning = delta.x >> 6;
			}
			previous = fontGlyph->GlyphIndex;
			pen_x = pen_x + kerning + fontGlyph->Advance;

		}
	}
	total_width = total_width + pen_x;
	lineSteps.push_back((int)text.size());
	MeasureInfo info;
	info.Height = pen_y + m_FontHeight;
	info.LastLineWidth = MeasureRange(text, lineSteps[lineSteps.size() - 2], lineSteps[lineSteps.size() - 1]);
	info.Width = total_width >= bound_width ? bound_width : total_width;
	info.MaxWidth = total_width;
	info.MaxHeight = total_height;
	return info;
}

void TextRenderer::DrawTextC(const char* cstr, int x, int y, Alighment align, int bound_width, glm::vec3 color, bool showEmotion)
{
	std::string str = utils::Utf8ToGB2312(cstr);
	std::wstring text = utils::StringToWstring(str);
	DrawTextW(text, x, y, align, bound_width, color, showEmotion);
}

/**
http://www.angelcode.com/products/bmfont/doc/pixel_shader.html
http://www.angelcode.com/products/bmfont/doc/render_text.html
Rendering colored text with outline
When the bitmap font has been generated with a baked outline, i.e. the outline is pre-generated in the font texture, the text is best rendered in two passes to avoid the outline for a character to overlap the previous character.

In the first pass the application should use only the alpha channel of the font texture as the transparency value when rendering the border.

In the second pass the application should use only the color channels of the font texture as the transparency value to render the characters over the border.

With a two-pass rendering like this it is also very easy to apply different colors to the border and the internal characters dynamically without the need to pre-color the font in the texture. The application simply needs to multiply the transparency value the desired color before blending it to the screen.
**/
void TextRenderer::DrawTextW(std::wstring text, int x, int y, Alighment align, int bound_width, glm::vec3 color, bool showEmotion)
{
	EnsureLoadText(text, showEmotion);

	for (size_t i = 0; i < text.size(); i++)
	{
		uint32_t charcode = -1;
		int cnt = GetCharcode(text, i, charcode, showEmotion);
		i += cnt;

		auto* fontGlyph = m_FontGlyphs[charcode];
		if (fontGlyph->IsEmotion)
			fontGlyph->EmotionUpdated = false;
	}

	m_pShader->Bind();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUniform3f(glGetUniformLocation(m_pShader->GetProgramID(), "textColor"), color.x, color.y, color.z);
	switch (align)
	{
	case TextRenderer::LEFT:
	{
		int pen_x = x;
		int pen_y = y + m_BearingY;
		int kerning = 0;
		int previous = 0;
		for (size_t i = 0; i < text.size(); i++)
		{
			uint32_t charcode = -1;
			int cnt = GetCharcode(text, i, charcode, showEmotion);
			i += cnt;

			auto* fontGlyph = m_FontGlyphs[charcode];
			if ((charcode == '\n') ||
				(bound_width > 0 && pen_x + fontGlyph->Advance + kerning >= bound_width + x))
			{
				pen_x = x;
				pen_y += m_FontHeight;

				previous = 0;
				kerning = 0;
				if (charcode != '\n')i--;
			}
			else
			{
				if (!fontGlyph->IsEmotion)
				{
					GLfloat xpos = (float)(pen_x + fontGlyph->Left);
					GLfloat ypos = (float)(m_ScreeHeight - pen_y + fontGlyph->Top);

					GLfloat h = (float)fontGlyph->Height;
					GLfloat w = (float)fontGlyph->Width;
					DrawGlyphTexture(fontGlyph->Texture->GetTextureID(), xpos, ypos, w, h);

					if (m_bUseKerning && previous && fontGlyph->GlyphIndex)
					{
						FT_Vector  delta;
						FT_Get_Kerning(m_FtFace, previous, fontGlyph->GlyphIndex,
							FT_KERNING_DEFAULT, &delta);
						kerning = delta.x >> 6;
					}
					pen_x = pen_x + fontGlyph->Advance + kerning;
					previous = fontGlyph->GlyphIndex;
				}
				else
				{
					//fontGlyph->Emotion->SetPos({ (int)pen_x,(int)(pen_y-m_BearingY + m_FontHeight/2 - fontGlyph->Height/2) });
					/*if (!fontGlyph->EmotionUpdated)
						fontGlyph->Emotion->OnUpdate();*/
					fontGlyph->EmotionUpdated = true;
					//fontGlyph->Emotion->Draw();

					pen_x = pen_x + fontGlyph->Advance;
					previous = 0;
				}
			}
		}
	}
	break;
	case TextRenderer::CENTER:
	{
		MeasureInfo info = MeasureText(text, bound_width);
		int pen_x = x - info.MaxWidth / 2;
		int pen_y = y + m_BearingY - info.MaxHeight / 2;
		int kerning = 0;
		int previous = 0;
		for (size_t i = 0; i < text.size(); i++)
		{
			uint32_t charcode = -1;
			int cnt = GetCharcode(text, i, charcode, showEmotion);
			i += cnt;

			auto* fontGlyph = m_FontGlyphs[charcode];
			if ((charcode == '\n') ||
				(bound_width > 0 && pen_x + fontGlyph->Advance + kerning >= bound_width + x))
			{
				int width = MeasureRange(text, (int)i, (int)(text.size() - 1));
				if (width > bound_width) width = bound_width;
				pen_x = x - width / 2;
				pen_y += m_FontHeight;
				if (charcode != '\n')i--;
				previous = 0;
				kerning = 0;
			}
			else
			{

				if (!fontGlyph->IsEmotion)
				{
					GLfloat xpos = (float)(pen_x + fontGlyph->Left);
					GLfloat ypos = (float)(m_ScreeHeight - pen_y + fontGlyph->Top);
					GLfloat h = (float)fontGlyph->Height;
					GLfloat w = (float)fontGlyph->Width;
					DrawGlyphTexture(fontGlyph->Texture->GetTextureID(), xpos, ypos, w, h);

					if (m_bUseKerning && previous && fontGlyph->GlyphIndex)
					{
						FT_Vector  delta;
						FT_Get_Kerning(m_FtFace, previous, fontGlyph->GlyphIndex,
							FT_KERNING_DEFAULT, &delta);
						kerning = delta.x >> 6;
					}
					pen_x = pen_x + fontGlyph->Advance + kerning;
					previous = fontGlyph->GlyphIndex;
				}
				else
				{
					//fontGlyph->Emotion->SetPos({ (int)pen_x,(int)(pen_y - m_BearingY + m_FontHeight / 2 - fontGlyph->Height / 2) });
					//if (!fontGlyph->EmotionUpdated)
						//fontGlyph->Emotion->OnUpdate();
					fontGlyph->EmotionUpdated = true;
					//fontGlyph->Emotion->Draw();

					pen_x = pen_x + fontGlyph->Advance;
					previous = 0;
				}
			}
		}
	}
	break;
	case TextRenderer::RIGHT:
		break;
	case TextRenderer::TOP:
		break;
	case TextRenderer::BOTTOM:
		break;
	default:
		break;
	}
	m_pShader->Unbind();
}

void TextRenderer::RenderText(std::wstring text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{

}

void TextRenderer::RenderFontText(std::string path, std::wstring text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{

}

void TextRenderer::RenderPlotText(std::wstring text, GLfloat x, GLfloat y)
{
	RenderFontText(FileSystem::GetGameFontPath(), text,
		x, y, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void text_renderer_draw_text(const char* str, int x, int y)
{
	TextRenderer::GetInstance()->DrawTextC(str, x, y, TextRenderer::LEFT, 0, { 128,128,128 });
}

void text_renderer_init()
{
	TextRenderer::GetInstance();
}


void TextView::OnCharacterInput(uint32_t charcode)
{
	TextCache.push_back(charcode);
}

bool TextView::OnClick(int button, int x, int y)
{
	INPUT_MANAGER_INSTANCE->RequestFocus(this);
	return true;
};

Bound TextView::GetViewBounds()
{
	return { (float)X,(float)X + (float)Width,(float)Y,(float)Y + (float)Height };
};

int TextView::GetViewLayer()const
{
	return 1;
};

bool TextView::CheckDrag(int dx, int dy)
{
	return pow(dx, 2) + pow(dy, 2) >= 16;
}

void TextView::OnDragMove(int dx, int dy)
{
	X += dx;
	Y += dy;
}

void TextView::OnFocusChanged(bool focus)
{
	m_IsEditing = focus;
}

TextView::TextView()
	:m_LastTime(0),
	m_Alpha(1.f),
	m_Bounce(false),
	X(0),
	Y(0),
	Width(0),
	Height(0),
	PaddingHorizontal(0),
	PaddingVertical(0),
	Color(1.f),
	Background(nullptr),
	m_IsEditing(false),
	OnEnterHit(nullptr),
	ShowEmotion(false)
{
	BitmapFile bpfile;
	Bitmap::Load(FileSystem::GetIconPath("text_cursor_w.bmp"), bpfile);
	Cursor = new Texture(bpfile.infoHeader.biWidth, bpfile.infoHeader.biHeight, true, bpfile.imageData);
	if (bpfile.imageData != nullptr)
	{
		delete bpfile.imageData;
		bpfile.imageData = nullptr;
	}
	BackgroundResID = RESOURCE_MANAGER_INSTANCE->EncodeWAS(WZIFEWDF, 0x39D3BD99);
	INPUT_MANAGER_INSTANCE->RegisterView(this);
	RefreshText();
}

TextView::~TextView()
{
	INPUT_MANAGER_INSTANCE->UnRegisterView(this);
	SafeDelete(Cursor);
	SafeDelete(Background);
}


void TextView::SetBackground(uint64_t resID)
{
	BackgroundResID = resID;
}

void TextView::RefreshText()
{
	Text = std::wstring(TextCache.begin(), TextCache.end());
	m_MeasureInfo = TextRenderer::GetInstance()->MeasureText(Text, Width - PaddingHorizontal * 2);
}

void TextView::OnKeyDownEvent(int keyCode)
{
	if (m_IsEditing)
	{
		if (keyCode == GLFW_KEY_ENTER)
		{
			if (OnEnterHit)
			{
				OnEnterHit();
			}
			else
			{
				TextCache.push_back('\n');
				RefreshText();
			}
		}
		else if (keyCode == GLFW_KEY_BACKSPACE)
		{
			if (TextCache.size() > 0)
				TextCache.pop_back();
		}
		else if (keyCode == GLFW_KEY_DELETE)
		{
			TextCache.clear();
		}
	}
}

void TextView::OnKeyRepeatEvent(int keyCode)
{
	if (m_IsEditing)
	{
		if (keyCode == GLFW_KEY_BACKSPACE)
		{
			if (TextCache.size() > 0)
				TextCache.pop_back();
		}
	}
}

void TextView::DrawCenter(float x, float y)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void TextView::OnUpdate()
{
	// drawBackgroud
	auto backgroud = RESOURCE_MANAGER_INSTANCE->LoadWASSpriteByID(BackgroundResID);
	if (backgroud != nullptr && Background == nullptr)
	{
		auto& frame = backgroud->Frames[0];
		Background = new Texture(frame.Width, frame.Height, true, (uint8*)frame.Src.data());
		Width = frame.Width;
		Height = frame.Height;
	}
	auto now = std::chrono::system_clock::now();
	uint64_t currentTime = now.time_since_epoch().count() / 10000;

	if (m_LastTime == 0)
	{
		m_LastTime = currentTime;
	}

	if (currentTime - m_LastTime > 500)
	{
		m_LastTime = currentTime;
		m_Bounce = !m_Bounce;
		RefreshText();
	}
	else
	{
		if (m_Bounce)
		{
			if (currentTime - m_LastTime < 300)
				m_Alpha = ((currentTime - m_LastTime)) / 300.f;
			else
				m_Alpha = 1.f;
		}
		else
		{
			if (currentTime - m_LastTime < 300)
				m_Alpha = 1.f - ((currentTime - m_LastTime)) / 300.f;
			else
				m_Alpha = 0.f;
		}
	}
}

void TextView::OnDraw()
{
	// drawBackgound
	if (Background != nullptr)
		SPRITE_RENDERER_INSTANCE->DrawFrameSprite(
			Background,
			{ X ,Y },
			{ Width, Height });

	// drawText
	TextRenderer::GetInstance()->DrawTextW(
		Text,
		X + PaddingHorizontal,
		Y + PaddingVertical,
		TextRenderer::LEFT,
		Width - PaddingHorizontal * 2,
		Color,
		ShowEmotion);

	// drawCursor
	if (Cursor && m_IsEditing)
		SPRITE_RENDERER_INSTANCE->DrawBitmap(
			Cursor,
			{ X + PaddingHorizontal + m_MeasureInfo.LastLineWidth,
			Y + PaddingVertical + m_MeasureInfo.Height - TextRenderer::GetInstance()->GetFontHeight() },
			{ Cursor->GetWidth() ,Cursor->GetHeight() },
			{ 1.f,1.f,1.f },
			m_Alpha
		);
}


Button::Button()
{

}

Button::~Button()
{

}

void Button::OnUpdate()
{

}

void Button::OnDraw()
{

}


void luaopen_text_renderer(lua_State* L)
{
	script_system_register_function(L, text_renderer_init);
	script_system_register_function(L, text_renderer_draw_text);
}