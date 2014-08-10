#pragma once

#include <project/project.pb.h>

#include <QString>

#include <iterator>

namespace ide {

class FileIterator : public std::iterator<std::forward_iterator_tag, QString> {
 public:
  using RepeatedString = google::protobuf::RepeatedPtrField<const std::string>;

  FileIterator(const RepeatedString::const_iterator& iterator)
      : it_(iterator) {}

  FileIterator& operator++() {
    ++it_;
    return *this;
  }
  QString operator*() const { return QString::fromStdString(*it_); }
  bool operator!=(const FileIterator& other) const { return it_ != other.it_; }

 private:
  RepeatedString::const_iterator it_;
};

}  // namespace ide
