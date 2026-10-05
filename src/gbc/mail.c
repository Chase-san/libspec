// Gen 2 mail, as party Pokémon carry it, with the backup copy the game loads it from.

#include <string.h>

#include "gbc/gbc.h"
#include "gbc/gbc_internal.h"
#include "spec_internal.h"

constexpr size_t MESSAGE_OFFSET = 0;
constexpr size_t AUTHOR_OFFSET = MESSAGE_OFFSET + SPEC_GBC_MAIL_MESSAGE_SIZE;
constexpr size_t MAILBOX_CAPACITY = 10;

// Japanese mail has no nationality, so its author ID follows the author.
static size_t author_id_offset(const spec_gbc_layout_t *layout) {
    size_t nationality_size = layout->has_mail_nationality ? SPEC_GBC_MAIL_NATIONALITY_SIZE : 0;
    return AUTHOR_OFFSET + layout->mail_author_size + nationality_size;
}

static size_t species_offset(const spec_gbc_layout_t *layout) {
    return author_id_offset(layout) + 2;
}

static bool is_holding_mail(const spec_gbc_save_t *save, size_t index) {
    return index < save->party_count && spec_gbc_is_mail(save->party[index].held_item);
}

static void decode_mail(spec_gbc_mail_t *mail, const uint8_t *bytes,
                        const spec_gbc_layout_t *layout) {
    *mail = (spec_gbc_mail_t){};
    memcpy(mail->message, &bytes[MESSAGE_OFFSET], SPEC_GBC_MAIL_MESSAGE_SIZE);
    memcpy(mail->author_name, &bytes[AUTHOR_OFFSET], layout->mail_author_size);
    if (layout->has_mail_nationality) {
        memcpy(mail->nationality, &bytes[AUTHOR_OFFSET + layout->mail_author_size],
               SPEC_GBC_MAIL_NATIONALITY_SIZE);
    }
    mail->author_id = spec_read_u16_be(&bytes[author_id_offset(layout)]);
    mail->species = bytes[species_offset(layout)];
    mail->type = bytes[species_offset(layout) + 1];
}

// As RestorePartyMonMail: loading takes the backup.
void spec_gbc_decode_party_mail(spec_gbc_save_t *save, const uint8_t *data,
                                const spec_gbc_layout_t *layout) {
    for (size_t index = 0; index < SPEC_GBC_PARTY_CAPACITY; ++index) {
        if (is_holding_mail(save, index)) {
            size_t offset = layout->party_mail_backup_offset + index * layout->mail_size;
            decode_mail(&save->party[index].party_data.mail, &data[offset], layout);
        }
    }
}

static size_t mailbox_size(const spec_gbc_layout_t *layout) {
    return 1 + MAILBOX_CAPACITY * layout->mail_size;
}

static void encode_mail(uint8_t *bytes, const spec_gbc_layout_t *layout,
                        const spec_gbc_mail_t *mail) {
    memcpy(&bytes[MESSAGE_OFFSET], mail->message, SPEC_GBC_MAIL_MESSAGE_SIZE);
    memcpy(&bytes[AUTHOR_OFFSET], mail->author_name, layout->mail_author_size);
    if (layout->has_mail_nationality) {
        memcpy(&bytes[AUTHOR_OFFSET + layout->mail_author_size], mail->nationality,
               SPEC_GBC_MAIL_NATIONALITY_SIZE);
    }
    spec_write_u16_be(&bytes[author_id_offset(layout)], mail->author_id);
    bytes[species_offset(layout)] = mail->species;
    bytes[species_offset(layout) + 1] = mail->type;
}

// Loading restores the mail from the backup and saving backs it up again; between the two, the
// party's mail is written.
void spec_gbc_encode_party_mail(uint8_t *data, const spec_gbc_layout_t *layout,
                                const spec_gbc_save_t *save) {
    size_t party_mail_size = SPEC_GBC_PARTY_CAPACITY * layout->mail_size;
    memcpy(&data[layout->party_mail_offset], &data[layout->party_mail_backup_offset],
           party_mail_size);
    memcpy(&data[layout->mailbox_offset], &data[layout->mailbox_backup_offset],
           mailbox_size(layout));
    for (size_t index = 0; index < SPEC_GBC_PARTY_CAPACITY; ++index) {
        if (is_holding_mail(save, index)) {
            const spec_gbc_mail_t *mail = &save->party[index].party_data.mail;
            encode_mail(&data[layout->party_mail_offset + index * layout->mail_size], layout, mail);
            encode_mail(&data[layout->party_mail_backup_offset + index * layout->mail_size], layout,
                        mail);
        }
    }
}

spec_error_t spec_gbc_check_mail(const spec_gbc_mail_t *mail, const spec_gbc_layout_t *layout) {
    size_t unused_author_size = SPEC_GBC_MAIL_AUTHOR_SIZE - layout->mail_author_size;
    if (!spec_is_all_zero(&mail->author_name[layout->mail_author_size], unused_author_size)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                         "mail's author_name is longer than this game's");
    }
    if (!layout->has_mail_nationality
        && !spec_is_all_zero(mail->nationality, SPEC_GBC_MAIL_NATIONALITY_SIZE)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "this game's mail has no nationality");
    }
    return SPEC_OK;
}
