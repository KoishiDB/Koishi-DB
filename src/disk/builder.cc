#include "disk/builder.h"

namespace koishidb {

    void BuildTable(const std::string& db_name, FileMeta* file_meta, Iterator* iter) {
//        CreateFile(db_name, file_meta->number);
//        iter->SeekToFirst();
//        table_builder = new TableBuilder();
//
//        file_meta->smallest_key = TransToInternalKey(iter->Key());
//        for (; iter->Valid(); iter->Next()) {
//            table_builder->Add(iter->Key(), iter->Value());
//        }
//        file_meta->largest_key = TransToInternalKey(iter->Key());
//        table_builder->Finish();
//        file_meta->file_size = table_builder->FileSize();
//        delete table_builder;
    }
};