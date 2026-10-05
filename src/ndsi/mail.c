// Gen 5 mail, as a party Pokémon carries it.

#include "nds/nds_internal.h"
#include "ndsi/ndsi.h"
#include "ndsi/ndsi_internal.h"
#include "spec_internal.h"

constexpr size_t AUTHOR_ID_OFFSET = 0x00;
constexpr size_t AUTHOR_SECRET_ID_OFFSET = 0x02;
constexpr size_t AUTHOR_GENDER_OFFSET = 0x04;
constexpr size_t LANGUAGE_OFFSET = 0x05;
constexpr size_t VERSION_OFFSET = 0x06;
constexpr size_t TYPE_OFFSET = 0x07;
constexpr size_t AUTHOR_NAME_OFFSET = 0x08;
constexpr size_t ICONS_OFFSET = 0x18;
constexpr size_t SENTENCES_OFFSET = 0x20;
constexpr size_t SENTENCE_SIZE = 8;

constexpr uint8_t NO_MAIL = 0xFF;
constexpr uint16_t NO_ENTRY = 0xFFFF;

void spec_ndsi_decode_mail(spec_ndsi_mail_t *mail, const uint8_t *bytes) {
    mail->author.id = spec_read_u16_le(&bytes[AUTHOR_ID_OFFSET]);
    mail->author.secret_id = spec_read_u16_le(&bytes[AUTHOR_SECRET_ID_OFFSET]);
    mail->author.is_female = bytes[AUTHOR_GENDER_OFFSET] != 0;
    spec_nds_read_text(mail->author.name, &bytes[AUTHOR_NAME_OFFSET], SPEC_NDSI_TRAINER_NAME_SIZE);
    mail->language = (spec_language_t)bytes[LANGUAGE_OFFSET];
    mail->version = (spec_version_t)bytes[VERSION_OFFSET];
    mail->type = bytes[TYPE_OFFSET];
    for (size_t icon = 0; icon < SPEC_NDSI_MAIL_ICON_COUNT; ++icon) {
        mail->icons[icon] = spec_read_u16_le(&bytes[ICONS_OFFSET + icon * 2]);
    }
    for (size_t sentence = 0; sentence < SPEC_NDSI_MAIL_SENTENCE_COUNT; ++sentence) {
        const uint8_t *sentence_bytes = &bytes[SENTENCES_OFFSET + sentence * SENTENCE_SIZE];
        mail->sentences[sentence].type = spec_read_u16_le(&sentence_bytes[0]);
        mail->sentences[sentence].id = spec_read_u16_le(&sentence_bytes[2]);
        mail->sentences[sentence].words[0] = spec_read_u16_le(&sentence_bytes[4]);
        mail->sentences[sentence].words[1] = spec_read_u16_le(&sentence_bytes[6]);
    }
}

void spec_ndsi_encode_mail(uint8_t *bytes, const spec_ndsi_mail_t *mail) {
    spec_write_u16_le(&bytes[AUTHOR_ID_OFFSET], mail->author.id);
    spec_write_u16_le(&bytes[AUTHOR_SECRET_ID_OFFSET], mail->author.secret_id);
    bytes[AUTHOR_GENDER_OFFSET] = mail->author.is_female ? 1 : 0;
    spec_nds_write_text(&bytes[AUTHOR_NAME_OFFSET], mail->author.name, SPEC_NDSI_TRAINER_NAME_SIZE);
    bytes[LANGUAGE_OFFSET] = mail->language;
    bytes[VERSION_OFFSET] = mail->version;
    bytes[TYPE_OFFSET] = mail->type;
    for (size_t icon = 0; icon < SPEC_NDSI_MAIL_ICON_COUNT; ++icon) {
        spec_write_u16_le(&bytes[ICONS_OFFSET + icon * 2], mail->icons[icon]);
    }
    for (size_t sentence = 0; sentence < SPEC_NDSI_MAIL_SENTENCE_COUNT; ++sentence) {
        uint8_t *sentence_bytes = &bytes[SENTENCES_OFFSET + sentence * SENTENCE_SIZE];
        spec_write_u16_le(&sentence_bytes[0], mail->sentences[sentence].type);
        spec_write_u16_le(&sentence_bytes[2], mail->sentences[sentence].id);
        spec_write_u16_le(&sentence_bytes[4], mail->sentences[sentence].words[0]);
        spec_write_u16_le(&sentence_bytes[6], mail->sentences[sentence].words[1]);
    }
}

// As Gen 4's Mail_Reset; the game also stamps its own language and version.
spec_ndsi_mail_t spec_ndsi_no_mail(void) {
    spec_ndsi_mail_t mail = {.type = NO_MAIL};
    for (size_t index = 0; index < SPEC_NDSI_TRAINER_NAME_SIZE; ++index) {
        mail.author.name[index] = NO_ENTRY;
    }
    for (size_t icon = 0; icon < SPEC_NDSI_MAIL_ICON_COUNT; ++icon) {
        mail.icons[icon] = NO_ENTRY;
    }
    for (size_t sentence = 0; sentence < SPEC_NDSI_MAIL_SENTENCE_COUNT; ++sentence) {
        mail.sentences[sentence] = (spec_ndsi_mail_sentence_t){
            .type = NO_ENTRY,
            .words = {NO_ENTRY, NO_ENTRY},
        };
    }
    return mail;
}
