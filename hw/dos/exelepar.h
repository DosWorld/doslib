
#pragma pack(push,1)
// parsed form, in parser struct
struct exe_le_header_parseinfo_object_page_table_entry {
    uint32_t        page_data_offset;               // +0x00 offset from preload page in bytes
    uint16_t        data_size;                      // +0x04 data size (from LX, or synthesized from LE)
    uint16_t        flags;                          // +0x06 flags
};                                                  // =0x08
#pragma pack(pop)

#pragma pack(push,1)
struct le_header_entry_table_entry {
    uint8_t                                                 type;
    uint16_t                                                object;
    uint32_t                                                offset;         // into raw
};
#pragma pack(pop)

struct le_header_entry_table {
    unsigned char*                                          raw;
    size_t                                                  raw_length;
    struct le_header_entry_table_entry*                     table;
    size_t                                                  length;
};

struct le_header_parseinfo {
    struct exe_ne_header_name_entry_table                   le_resident_names;
    struct exe_ne_header_name_entry_table                   le_nonresident_names;
    struct exe_le_header_parseinfo_object_page_table_entry* le_object_page_map_table;           /* [number_of_memory_pages] entries */
    struct exe_le_header_object_table_entry*                le_object_table;                    /* [object_table_entries] entries */
    uint32_t*                                               le_fixup_page_table;                /* [number_of_memory_pages + 1] entries */
    uint32_t                                                le_header_offset;
    struct le_header_entry_table                            le_entry_table;
    struct exe_le_header                                    le_header;
};

struct le_vmap_trackio {
    uint32_t                file_ofs;       // file offset of page
    uint32_t                page_number;    // page number we are on
    uint16_t                page_ofs;       // offset within page
    uint16_t                page_size;      // size of page
    uint32_t                object;         // object we're reading
    uint32_t                offset;         // offset within object
};

int le_segofs_to_trackio(struct le_vmap_trackio * const io,const uint16_t object,const uint32_t offset,const struct le_header_parseinfo * const lep);
int le_trackio_read(unsigned char *buf,int len,const int fd,struct le_vmap_trackio * const io,const struct le_header_parseinfo * const lep);

uint32_t le_exe_header_entry_table_size(struct exe_le_header * const h);
void le_header_entry_table_free_table(struct le_header_entry_table *t);
void le_header_entry_table_free_raw(struct le_header_entry_table *t);
void le_header_entry_table_free(struct le_header_entry_table *t);
unsigned char *le_header_entry_table_get_raw_entry(struct le_header_entry_table *t,size_t i);
unsigned char *le_header_entry_table_alloc(struct le_header_entry_table *t,size_t sz);
void le_header_entry_table_parse(struct le_header_entry_table * const t);

