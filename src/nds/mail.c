// Gen 4 mail, as a party Pokémon carries it.

#include "nds/nds.h"
#include "nds/nds_internal.h"
#include "spec_internal.h"

constexpr size_t AUTHOR_ID_OFFSET = 0x00;        // pret/pokeplatinum Mail trainerID
constexpr size_t AUTHOR_SECRET_ID_OFFSET = 0x02; // pret/pokeplatinum Mail trainerID, upper half
constexpr size_t AUTHOR_GENDER_OFFSET = 0x04;    // pret/pokeplatinum Mail trainerGender
constexpr size_t LANGUAGE_OFFSET = 0x05;         // pret/pokeplatinum Mail language
constexpr size_t VERSION_OFFSET = 0x06;          // pret/pokeplatinum Mail gameVersion
constexpr size_t TYPE_OFFSET = 0x07;             // pret/pokeplatinum Mail mailType
constexpr size_t AUTHOR_NAME_OFFSET = 0x08;      // pret/pokeplatinum Mail trainerName
constexpr size_t ICONS_OFFSET = 0x18;            // pret/pokeplatinum Mail iconData
constexpr size_t ICON_FORMS_OFFSET = 0x1E;       // pret/pokeplatinum Mail platExclusiveFormIcons
constexpr size_t SENTENCES_OFFSET = 0x20;        // pret/pokeplatinum Mail sentences
constexpr size_t SENTENCE_SIZE = 8;              // pret/pokeplatinum EasyChatSentence

constexpr unsigned ICON_SPRITE_BIT_COUNT = 12;
constexpr unsigned ICON_PALETTE_BIT = 12;
constexpr unsigned ICON_PALETTE_BIT_COUNT = 4;
constexpr unsigned ICON_FORM_BIT_COUNT = 5;

constexpr uint8_t NO_MAIL = 0xFF;
constexpr uint16_t NO_ENTRY = 0xFFFF;
// MAIL_MON_ICON_NONE, 0xFFFF, split into its fields.
constexpr uint16_t NO_ICON_SPRITE = 0xFFF;
constexpr uint8_t NO_ICON_PALETTE = 0xF;

void spec_nds_decode_mail(spec_nds_mail_t *mail, const uint8_t *bytes) {
    mail->author.id = spec_read_u16_le(&bytes[AUTHOR_ID_OFFSET]);
    mail->author.secret_id = spec_read_u16_le(&bytes[AUTHOR_SECRET_ID_OFFSET]);
    mail->author.is_female = bytes[AUTHOR_GENDER_OFFSET] != 0;
    spec_nds_read_text(mail->author.name, &bytes[AUTHOR_NAME_OFFSET], SPEC_NDS_TRAINER_NAME_SIZE);
    mail->language = (spec_language_t)bytes[LANGUAGE_OFFSET];
    mail->version = (spec_version_t)bytes[VERSION_OFFSET];
    mail->type = bytes[TYPE_OFFSET];
    uint16_t icon_forms = spec_read_u16_le(&bytes[ICON_FORMS_OFFSET]);
    for (unsigned icon = 0; icon < SPEC_NDS_MAIL_ICON_COUNT; ++icon) {
        uint16_t icon_word = spec_read_u16_le(&bytes[ICONS_OFFSET + icon * 2]);
        mail->icons[icon] = (spec_nds_mail_icon_t){
            .sprite = (uint16_t)spec_get_bits(icon_word, 0, ICON_SPRITE_BIT_COUNT),
            .palette = (uint8_t)spec_get_bits(icon_word, ICON_PALETTE_BIT, ICON_PALETTE_BIT_COUNT),
            .form =
                (uint8_t)spec_get_bits(icon_forms, icon * ICON_FORM_BIT_COUNT, ICON_FORM_BIT_COUNT),
        };
    }
    spec_nds_decode_mail_sentences(mail->sentences, &bytes[SENTENCES_OFFSET]);
}

void spec_nds_decode_mail_sentences(
    spec_nds_mail_sentence_t sentences[static SPEC_NDS_MAIL_SENTENCE_COUNT], const uint8_t *bytes) {
    for (size_t sentence = 0; sentence < SPEC_NDS_MAIL_SENTENCE_COUNT; ++sentence) {
        const uint8_t *sentence_bytes = &bytes[sentence * SENTENCE_SIZE];
        sentences[sentence].type = spec_read_u16_le(&sentence_bytes[0]);
        sentences[sentence].id = spec_read_u16_le(&sentence_bytes[2]);
        sentences[sentence].words[0] = spec_read_u16_le(&sentence_bytes[4]);
        sentences[sentence].words[1] = spec_read_u16_le(&sentence_bytes[6]);
    }
}

void spec_nds_encode_mail(uint8_t *bytes, const spec_nds_mail_t *mail) {
    spec_write_u16_le(&bytes[AUTHOR_ID_OFFSET], mail->author.id);
    spec_write_u16_le(&bytes[AUTHOR_SECRET_ID_OFFSET], mail->author.secret_id);
    bytes[AUTHOR_GENDER_OFFSET] = mail->author.is_female ? 1 : 0;
    spec_nds_write_text(&bytes[AUTHOR_NAME_OFFSET], mail->author.name, SPEC_NDS_TRAINER_NAME_SIZE);
    bytes[LANGUAGE_OFFSET] = mail->language;
    bytes[VERSION_OFFSET] = mail->version;
    bytes[TYPE_OFFSET] = mail->type;
    uint32_t icon_forms = 0;
    for (unsigned icon = 0; icon < SPEC_NDS_MAIL_ICON_COUNT; ++icon) {
        uint32_t icon_word = mail->icons[icon].sprite;
        icon_word = spec_set_bits(icon_word, ICON_PALETTE_BIT, ICON_PALETTE_BIT_COUNT,
                                  mail->icons[icon].palette);
        spec_write_u16_le(&bytes[ICONS_OFFSET + icon * 2], (uint16_t)icon_word);
        icon_forms = spec_set_bits(icon_forms, icon * ICON_FORM_BIT_COUNT, ICON_FORM_BIT_COUNT,
                                   mail->icons[icon].form);
    }
    spec_write_u16_le(&bytes[ICON_FORMS_OFFSET], (uint16_t)icon_forms);
    spec_nds_encode_mail_sentences(&bytes[SENTENCES_OFFSET], mail->sentences);
}

void spec_nds_encode_mail_sentences(
    uint8_t *bytes, const spec_nds_mail_sentence_t sentences[static SPEC_NDS_MAIL_SENTENCE_COUNT]) {
    for (size_t sentence = 0; sentence < SPEC_NDS_MAIL_SENTENCE_COUNT; ++sentence) {
        uint8_t *sentence_bytes = &bytes[sentence * SENTENCE_SIZE];
        spec_write_u16_le(&sentence_bytes[0], sentences[sentence].type);
        spec_write_u16_le(&sentence_bytes[2], sentences[sentence].id);
        spec_write_u16_le(&sentence_bytes[4], sentences[sentence].words[0]);
        spec_write_u16_le(&sentence_bytes[6], sentences[sentence].words[1]);
    }
}

void spec_nds_init_mail_sentences(
    spec_nds_mail_sentence_t sentences[static SPEC_NDS_MAIL_SENTENCE_COUNT]) {
    for (size_t sentence = 0; sentence < SPEC_NDS_MAIL_SENTENCE_COUNT; ++sentence) {
        sentences[sentence] = (spec_nds_mail_sentence_t){
            .type = NO_ENTRY,
            .words = {NO_ENTRY, NO_ENTRY},
        };
    }
}

const char *spec_nds_unencodable_mail_field_of(const spec_nds_mail_t *mail) {
    for (size_t icon = 0; icon < SPEC_NDS_MAIL_ICON_COUNT; ++icon) {
        if (!spec_fits_in_bits(mail->icons[icon].sprite, ICON_SPRITE_BIT_COUNT)) {
            return "a mail icon's sprite does not fit in 12 bits";
        }
        if (!spec_fits_in_bits(mail->icons[icon].palette, ICON_PALETTE_BIT_COUNT)) {
            return "a mail icon's palette does not fit in 4 bits";
        }
        if (!spec_fits_in_bits(mail->icons[icon].form, ICON_FORM_BIT_COUNT)) {
            return "a mail icon's form does not fit in 5 bits";
        }
    }
    return nullptr;
}

// As Mail_Reset.
void spec_nds_init_mail(spec_nds_mail_t *mail) {
    *mail = (spec_nds_mail_t){.type = NO_MAIL};
    for (size_t index = 0; index < SPEC_NDS_TRAINER_NAME_SIZE; ++index) {
        mail->author.name[index] = NO_ENTRY;
    }
    for (size_t icon = 0; icon < SPEC_NDS_MAIL_ICON_COUNT; ++icon) {
        mail->icons[icon] =
            (spec_nds_mail_icon_t){.sprite = NO_ICON_SPRITE, .palette = NO_ICON_PALETTE};
    }
    spec_nds_init_mail_sentences(mail->sentences);
}
