#include "disk/builder.h"

#include <fcntl.h>

#include "common/option.h"
#include "disk/table_builder.h"
#include "disk/writable_file.h"
#include "logger.h"

namespace koishidb {

// Maybe we can use the option instead
// You need to check the file_meta->num to find if there is data in iter.
// The iter should be the memtable iterator for returning the memtable entry
// if something wrong happens, this function will automatically remove the file.
Status BuildTable(const std::string& file_name, FileMeta* file_meta,
                  Iterator* iter) {
  iter->SeekToFirst();
  if (!iter->Valid()) {
    return Status::OK();  // empty data
  }
  int fd = ::open(file_name.data(), O_WRONLY | O_CREAT, 0777);
  LOG_INFO("stared build sstable, name %s", file_name.data());
  if (fd == -1) {
    return Status::IOError("open file error, check the file name");
  }
  WritableFile writable_file(fd, file_name);
  if (iter->Valid()) {
    Slice internal_key = iter->Key();
    file_meta->smallest_key = TransToInternalKey(internal_key);
  }
  const Option opt(new InternalKeyComparator());
  TableBuilder builder(&opt, &writable_file);
  while (iter->Valid()) {
    Slice internal_key = iter->Key(), value = iter->Value();
    builder.Add(internal_key, value);
    // update the file_meta->largest_key
    file_meta->largest_key = TransToInternalKey(internal_key);

    // Tidy up the memory data
    // memory_leak here, need to delete the memtable_key -> TODO later
    iter->Next();
  }
  Status s = builder.Finish();
  if (!s.ok()) {
    // build table error, we need to handle the
    Remove(file_name);
    return s;
  }
  file_meta->file_size = builder.FileSize();
  PrintFileMeta(*file_meta);

  // opt automatically delete the InternalKeyComparator
  return Status::OK();
}
};  // namespace koishidb