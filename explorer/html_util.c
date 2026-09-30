#include "explorer.h"

/**
 * html_hex - writes a buffer in hexadecimal
 *
 * @fp:  output stream
 * @buf: buffer to write
 * @len: number of bytes
 */
void html_hex(FILE *fp, uint8_t const *buf, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++)
		fprintf(fp, "%02x", buf[i]);
}

/**
 * html_short_hex - writes a shortened hash, full value shown on hover
 *
 * @fp:  output stream
 * @buf: buffer to write
 * @len: number of bytes
 */
void html_short_hex(FILE *fp, uint8_t const *buf, size_t len)
{
	fprintf(fp, "<span class=\"mono\" title=\"");
	html_hex(fp, buf, len);
	fprintf(fp, "\">");
	if (len <= 10)
		html_hex(fp, buf, len);
	else
	{
		html_hex(fp, buf, 6);
		fprintf(fp, "&hellip;");
		html_hex(fp, buf + len - 4, 4);
	}
	fprintf(fp, "</span>");
}

/**
 * html_text - writes bytes as escaped HTML text
 *
 * @fp:  output stream
 * @buf: bytes to write
 * @len: number of bytes
 */
void html_text(FILE *fp, int8_t const *buf, size_t len)
{
	size_t i;
	int c;

	for (i = 0; i < len; i++)
	{
		c = (unsigned char)buf[i];
		switch (c)
		{
		case '<':
			fputs("&lt;", fp);
			break;
		case '>':
			fputs("&gt;", fp);
			break;
		case '&':
			fputs("&amp;", fp);
			break;
		case '"':
			fputs("&quot;", fp);
			break;
		default:
			fputc(c >= 32 && c < 127 ? c : '.', fp);
		}
	}
}
