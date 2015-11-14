#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <QHash>
#include <QPair>
#include <QSet>
#include <QString>
#include <QStringList>

namespace ide {

using i8 = int8_t;
using ui8 = uint8_t;
using i16 = int16_t;
using ui16 = uint16_t;
using i32 = int32_t;
using ui32 = uint32_t;
using i64 = int64_t;
using ui64 = uint64_t;

template <typename Signature>
using Fn = std::function<Signature>;

template <class U, class V>
using HashMap = QHash<U, V>;

template <class T>
using List = QList<T>;

template <class U, class V>
using Pair = QPair<U, V>;

template <class T>
using Set = QSet<T>;

using String = QString;
using StringList = QStringList;
using StringVector = std::vector<std::string>;

template <class T>
using UniquePtr = std::unique_ptr<T>;

constexpr const auto StdToStr = &String::fromStdString;

}  // namespace ide
