//Generation 1&2
//The GB Games

//JPRed/JPGreen (gen 0)
//Red/Blue/Yellow
//Gold/Silver/Crystal

//Unlike later games, the GB games have no encryption or checksums
//Which makes working with them relatively easy

#ifndef __GB_H__
#define __GB_H__

void gb_text_to_utf16(char16_t *dst, char8_t *src, size_t size);

#endif //__GB_H__
