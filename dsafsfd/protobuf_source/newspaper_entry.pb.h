// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: newspaper_entry.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_newspaper_5fentry_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_newspaper_5fentry_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021008 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include "articles.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_newspaper_5fentry_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_newspaper_5fentry_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_newspaper_5fentry_2eproto;
namespace np2ps {
class LocalSerializedNewspaperEntry;
struct LocalSerializedNewspaperEntryDefaultTypeInternal;
extern LocalSerializedNewspaperEntryDefaultTypeInternal _LocalSerializedNewspaperEntry_default_instance_;
class NetworkSerializedNewspaperEntry;
struct NetworkSerializedNewspaperEntryDefaultTypeInternal;
extern NetworkSerializedNewspaperEntryDefaultTypeInternal _NetworkSerializedNewspaperEntry_default_instance_;
class NewspaperEntry;
struct NewspaperEntryDefaultTypeInternal;
extern NewspaperEntryDefaultTypeInternal _NewspaperEntry_default_instance_;
}  // namespace np2ps
PROTOBUF_NAMESPACE_OPEN
template<> ::np2ps::LocalSerializedNewspaperEntry* Arena::CreateMaybeMessage<::np2ps::LocalSerializedNewspaperEntry>(Arena*);
template<> ::np2ps::NetworkSerializedNewspaperEntry* Arena::CreateMaybeMessage<::np2ps::NetworkSerializedNewspaperEntry>(Arena*);
template<> ::np2ps::NewspaperEntry* Arena::CreateMaybeMessage<::np2ps::NewspaperEntry>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace np2ps {

// ===================================================================

class NewspaperEntry final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:np2ps.NewspaperEntry) */ {
 public:
  inline NewspaperEntry() : NewspaperEntry(nullptr) {}
  ~NewspaperEntry() override;
  explicit PROTOBUF_CONSTEXPR NewspaperEntry(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  NewspaperEntry(const NewspaperEntry& from);
  NewspaperEntry(NewspaperEntry&& from) noexcept
    : NewspaperEntry() {
    *this = ::std::move(from);
  }

  inline NewspaperEntry& operator=(const NewspaperEntry& from) {
    CopyFrom(from);
    return *this;
  }
  inline NewspaperEntry& operator=(NewspaperEntry&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const NewspaperEntry& default_instance() {
    return *internal_default_instance();
  }
  static inline const NewspaperEntry* internal_default_instance() {
    return reinterpret_cast<const NewspaperEntry*>(
               &_NewspaperEntry_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(NewspaperEntry& a, NewspaperEntry& b) {
    a.Swap(&b);
  }
  inline void Swap(NewspaperEntry* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(NewspaperEntry* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  NewspaperEntry* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<NewspaperEntry>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const NewspaperEntry& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const NewspaperEntry& from) {
    NewspaperEntry::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(NewspaperEntry* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "np2ps.NewspaperEntry";
  }
  protected:
  explicit NewspaperEntry(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kAuthoritiesFieldNumber = 4,
    kNewsNameFieldNumber = 2,
    kNewsIdFieldNumber = 1,
  };
  // repeated uint64 authorities = 4;
  int authorities_size() const;
  private:
  int _internal_authorities_size() const;
  public:
  void clear_authorities();
  private:
  uint64_t _internal_authorities(int index) const;
  const ::PROTOBUF_NAMESPACE_ID::RepeatedField< uint64_t >&
      _internal_authorities() const;
  void _internal_add_authorities(uint64_t value);
  ::PROTOBUF_NAMESPACE_ID::RepeatedField< uint64_t >*
      _internal_mutable_authorities();
  public:
  uint64_t authorities(int index) const;
  void set_authorities(int index, uint64_t value);
  void add_authorities(uint64_t value);
  const ::PROTOBUF_NAMESPACE_ID::RepeatedField< uint64_t >&
      authorities() const;
  ::PROTOBUF_NAMESPACE_ID::RepeatedField< uint64_t >*
      mutable_authorities();

  // string news_name = 2;
  void clear_news_name();
  const std::string& news_name() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_news_name(ArgT0&& arg0, ArgT... args);
  std::string* mutable_news_name();
  PROTOBUF_NODISCARD std::string* release_news_name();
  void set_allocated_news_name(std::string* news_name);
  private:
  const std::string& _internal_news_name() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_news_name(const std::string& value);
  std::string* _internal_mutable_news_name();
  public:

  // uint64 news_id = 1;
  void clear_news_id();
  uint64_t news_id() const;
  void set_news_id(uint64_t value);
  private:
  uint64_t _internal_news_id() const;
  void _internal_set_news_id(uint64_t value);
  public:

  // @@protoc_insertion_point(class_scope:np2ps.NewspaperEntry)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::RepeatedField< uint64_t > authorities_;
    mutable std::atomic<int> _authorities_cached_byte_size_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr news_name_;
    uint64_t news_id_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_newspaper_5fentry_2eproto;
};
// -------------------------------------------------------------------

class NetworkSerializedNewspaperEntry final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:np2ps.NetworkSerializedNewspaperEntry) */ {
 public:
  inline NetworkSerializedNewspaperEntry() : NetworkSerializedNewspaperEntry(nullptr) {}
  ~NetworkSerializedNewspaperEntry() override;
  explicit PROTOBUF_CONSTEXPR NetworkSerializedNewspaperEntry(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  NetworkSerializedNewspaperEntry(const NetworkSerializedNewspaperEntry& from);
  NetworkSerializedNewspaperEntry(NetworkSerializedNewspaperEntry&& from) noexcept
    : NetworkSerializedNewspaperEntry() {
    *this = ::std::move(from);
  }

  inline NetworkSerializedNewspaperEntry& operator=(const NetworkSerializedNewspaperEntry& from) {
    CopyFrom(from);
    return *this;
  }
  inline NetworkSerializedNewspaperEntry& operator=(NetworkSerializedNewspaperEntry&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const NetworkSerializedNewspaperEntry& default_instance() {
    return *internal_default_instance();
  }
  static inline const NetworkSerializedNewspaperEntry* internal_default_instance() {
    return reinterpret_cast<const NetworkSerializedNewspaperEntry*>(
               &_NetworkSerializedNewspaperEntry_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(NetworkSerializedNewspaperEntry& a, NetworkSerializedNewspaperEntry& b) {
    a.Swap(&b);
  }
  inline void Swap(NetworkSerializedNewspaperEntry* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(NetworkSerializedNewspaperEntry* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  NetworkSerializedNewspaperEntry* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<NetworkSerializedNewspaperEntry>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const NetworkSerializedNewspaperEntry& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const NetworkSerializedNewspaperEntry& from) {
    NetworkSerializedNewspaperEntry::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(NetworkSerializedNewspaperEntry* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "np2ps.NetworkSerializedNewspaperEntry";
  }
  protected:
  explicit NetworkSerializedNewspaperEntry(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kArticlesFieldNumber = 3,
    kEntryFieldNumber = 1,
  };
  // repeated .np2ps.Article articles = 3;
  int articles_size() const;
  private:
  int _internal_articles_size() const;
  public:
  void clear_articles();
  ::np2ps::Article* mutable_articles(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::np2ps::Article >*
      mutable_articles();
  private:
  const ::np2ps::Article& _internal_articles(int index) const;
  ::np2ps::Article* _internal_add_articles();
  public:
  const ::np2ps::Article& articles(int index) const;
  ::np2ps::Article* add_articles();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::np2ps::Article >&
      articles() const;

  // .np2ps.NewspaperEntry entry = 1;
  bool has_entry() const;
  private:
  bool _internal_has_entry() const;
  public:
  void clear_entry();
  const ::np2ps::NewspaperEntry& entry() const;
  PROTOBUF_NODISCARD ::np2ps::NewspaperEntry* release_entry();
  ::np2ps::NewspaperEntry* mutable_entry();
  void set_allocated_entry(::np2ps::NewspaperEntry* entry);
  private:
  const ::np2ps::NewspaperEntry& _internal_entry() const;
  ::np2ps::NewspaperEntry* _internal_mutable_entry();
  public:
  void unsafe_arena_set_allocated_entry(
      ::np2ps::NewspaperEntry* entry);
  ::np2ps::NewspaperEntry* unsafe_arena_release_entry();

  // @@protoc_insertion_point(class_scope:np2ps.NetworkSerializedNewspaperEntry)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::np2ps::Article > articles_;
    ::np2ps::NewspaperEntry* entry_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_newspaper_5fentry_2eproto;
};
// -------------------------------------------------------------------

class LocalSerializedNewspaperEntry final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:np2ps.LocalSerializedNewspaperEntry) */ {
 public:
  inline LocalSerializedNewspaperEntry() : LocalSerializedNewspaperEntry(nullptr) {}
  ~LocalSerializedNewspaperEntry() override;
  explicit PROTOBUF_CONSTEXPR LocalSerializedNewspaperEntry(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  LocalSerializedNewspaperEntry(const LocalSerializedNewspaperEntry& from);
  LocalSerializedNewspaperEntry(LocalSerializedNewspaperEntry&& from) noexcept
    : LocalSerializedNewspaperEntry() {
    *this = ::std::move(from);
  }

  inline LocalSerializedNewspaperEntry& operator=(const LocalSerializedNewspaperEntry& from) {
    CopyFrom(from);
    return *this;
  }
  inline LocalSerializedNewspaperEntry& operator=(LocalSerializedNewspaperEntry&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const LocalSerializedNewspaperEntry& default_instance() {
    return *internal_default_instance();
  }
  static inline const LocalSerializedNewspaperEntry* internal_default_instance() {
    return reinterpret_cast<const LocalSerializedNewspaperEntry*>(
               &_LocalSerializedNewspaperEntry_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    2;

  friend void swap(LocalSerializedNewspaperEntry& a, LocalSerializedNewspaperEntry& b) {
    a.Swap(&b);
  }
  inline void Swap(LocalSerializedNewspaperEntry* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(LocalSerializedNewspaperEntry* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  LocalSerializedNewspaperEntry* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<LocalSerializedNewspaperEntry>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const LocalSerializedNewspaperEntry& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const LocalSerializedNewspaperEntry& from) {
    LocalSerializedNewspaperEntry::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(LocalSerializedNewspaperEntry* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "np2ps.LocalSerializedNewspaperEntry";
  }
  protected:
  explicit LocalSerializedNewspaperEntry(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kArticlesFieldNumber = 2,
    kEntryFieldNumber = 1,
  };
  // repeated .np2ps.SerializedArticle articles = 2;
  int articles_size() const;
  private:
  int _internal_articles_size() const;
  public:
  void clear_articles();
  ::np2ps::SerializedArticle* mutable_articles(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::np2ps::SerializedArticle >*
      mutable_articles();
  private:
  const ::np2ps::SerializedArticle& _internal_articles(int index) const;
  ::np2ps::SerializedArticle* _internal_add_articles();
  public:
  const ::np2ps::SerializedArticle& articles(int index) const;
  ::np2ps::SerializedArticle* add_articles();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::np2ps::SerializedArticle >&
      articles() const;

  // .np2ps.NewspaperEntry entry = 1;
  bool has_entry() const;
  private:
  bool _internal_has_entry() const;
  public:
  void clear_entry();
  const ::np2ps::NewspaperEntry& entry() const;
  PROTOBUF_NODISCARD ::np2ps::NewspaperEntry* release_entry();
  ::np2ps::NewspaperEntry* mutable_entry();
  void set_allocated_entry(::np2ps::NewspaperEntry* entry);
  private:
  const ::np2ps::NewspaperEntry& _internal_entry() const;
  ::np2ps::NewspaperEntry* _internal_mutable_entry();
  public:
  void unsafe_arena_set_allocated_entry(
      ::np2ps::NewspaperEntry* entry);
  ::np2ps::NewspaperEntry* unsafe_arena_release_entry();

  // @@protoc_insertion_point(class_scope:np2ps.LocalSerializedNewspaperEntry)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::np2ps::SerializedArticle > articles_;
    ::np2ps::NewspaperEntry* entry_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_newspaper_5fentry_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// NewspaperEntry

// uint64 news_id = 1;
inline void NewspaperEntry::clear_news_id() {
  _impl_.news_id_ = uint64_t{0u};
}
inline uint64_t NewspaperEntry::_internal_news_id() const {
  return _impl_.news_id_;
}
inline uint64_t NewspaperEntry::news_id() const {
  // @@protoc_insertion_point(field_get:np2ps.NewspaperEntry.news_id)
  return _internal_news_id();
}
inline void NewspaperEntry::_internal_set_news_id(uint64_t value) {
  
  _impl_.news_id_ = value;
}
inline void NewspaperEntry::set_news_id(uint64_t value) {
  _internal_set_news_id(value);
  // @@protoc_insertion_point(field_set:np2ps.NewspaperEntry.news_id)
}

// string news_name = 2;
inline void NewspaperEntry::clear_news_name() {
  _impl_.news_name_.ClearToEmpty();
}
inline const std::string& NewspaperEntry::news_name() const {
  // @@protoc_insertion_point(field_get:np2ps.NewspaperEntry.news_name)
  return _internal_news_name();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void NewspaperEntry::set_news_name(ArgT0&& arg0, ArgT... args) {
 
 _impl_.news_name_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:np2ps.NewspaperEntry.news_name)
}
inline std::string* NewspaperEntry::mutable_news_name() {
  std::string* _s = _internal_mutable_news_name();
  // @@protoc_insertion_point(field_mutable:np2ps.NewspaperEntry.news_name)
  return _s;
}
inline const std::string& NewspaperEntry::_internal_news_name() const {
  return _impl_.news_name_.Get();
}
inline void NewspaperEntry::_internal_set_news_name(const std::string& value) {
  
  _impl_.news_name_.Set(value, GetArenaForAllocation());
}
inline std::string* NewspaperEntry::_internal_mutable_news_name() {
  
  return _impl_.news_name_.Mutable(GetArenaForAllocation());
}
inline std::string* NewspaperEntry::release_news_name() {
  // @@protoc_insertion_point(field_release:np2ps.NewspaperEntry.news_name)
  return _impl_.news_name_.Release();
}
inline void NewspaperEntry::set_allocated_news_name(std::string* news_name) {
  if (news_name != nullptr) {
    
  } else {
    
  }
  _impl_.news_name_.SetAllocated(news_name, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.news_name_.IsDefault()) {
    _impl_.news_name_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:np2ps.NewspaperEntry.news_name)
}

// repeated uint64 authorities = 4;
inline int NewspaperEntry::_internal_authorities_size() const {
  return _impl_.authorities_.size();
}
inline int NewspaperEntry::authorities_size() const {
  return _internal_authorities_size();
}
inline void NewspaperEntry::clear_authorities() {
  _impl_.authorities_.Clear();
}
inline uint64_t NewspaperEntry::_internal_authorities(int index) const {
  return _impl_.authorities_.Get(index);
}
inline uint64_t NewspaperEntry::authorities(int index) const {
  // @@protoc_insertion_point(field_get:np2ps.NewspaperEntry.authorities)
  return _internal_authorities(index);
}
inline void NewspaperEntry::set_authorities(int index, uint64_t value) {
  _impl_.authorities_.Set(index, value);
  // @@protoc_insertion_point(field_set:np2ps.NewspaperEntry.authorities)
}
inline void NewspaperEntry::_internal_add_authorities(uint64_t value) {
  _impl_.authorities_.Add(value);
}
inline void NewspaperEntry::add_authorities(uint64_t value) {
  _internal_add_authorities(value);
  // @@protoc_insertion_point(field_add:np2ps.NewspaperEntry.authorities)
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedField< uint64_t >&
NewspaperEntry::_internal_authorities() const {
  return _impl_.authorities_;
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedField< uint64_t >&
NewspaperEntry::authorities() const {
  // @@protoc_insertion_point(field_list:np2ps.NewspaperEntry.authorities)
  return _internal_authorities();
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedField< uint64_t >*
NewspaperEntry::_internal_mutable_authorities() {
  return &_impl_.authorities_;
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedField< uint64_t >*
NewspaperEntry::mutable_authorities() {
  // @@protoc_insertion_point(field_mutable_list:np2ps.NewspaperEntry.authorities)
  return _internal_mutable_authorities();
}

// -------------------------------------------------------------------

// NetworkSerializedNewspaperEntry

// .np2ps.NewspaperEntry entry = 1;
inline bool NetworkSerializedNewspaperEntry::_internal_has_entry() const {
  return this != internal_default_instance() && _impl_.entry_ != nullptr;
}
inline bool NetworkSerializedNewspaperEntry::has_entry() const {
  return _internal_has_entry();
}
inline void NetworkSerializedNewspaperEntry::clear_entry() {
  if (GetArenaForAllocation() == nullptr && _impl_.entry_ != nullptr) {
    delete _impl_.entry_;
  }
  _impl_.entry_ = nullptr;
}
inline const ::np2ps::NewspaperEntry& NetworkSerializedNewspaperEntry::_internal_entry() const {
  const ::np2ps::NewspaperEntry* p = _impl_.entry_;
  return p != nullptr ? *p : reinterpret_cast<const ::np2ps::NewspaperEntry&>(
      ::np2ps::_NewspaperEntry_default_instance_);
}
inline const ::np2ps::NewspaperEntry& NetworkSerializedNewspaperEntry::entry() const {
  // @@protoc_insertion_point(field_get:np2ps.NetworkSerializedNewspaperEntry.entry)
  return _internal_entry();
}
inline void NetworkSerializedNewspaperEntry::unsafe_arena_set_allocated_entry(
    ::np2ps::NewspaperEntry* entry) {
  if (GetArenaForAllocation() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.entry_);
  }
  _impl_.entry_ = entry;
  if (entry) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:np2ps.NetworkSerializedNewspaperEntry.entry)
}
inline ::np2ps::NewspaperEntry* NetworkSerializedNewspaperEntry::release_entry() {
  
  ::np2ps::NewspaperEntry* temp = _impl_.entry_;
  _impl_.entry_ = nullptr;
#ifdef PROTOBUF_FORCE_COPY_IN_RELEASE
  auto* old =  reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(temp);
  temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  if (GetArenaForAllocation() == nullptr) { delete old; }
#else  // PROTOBUF_FORCE_COPY_IN_RELEASE
  if (GetArenaForAllocation() != nullptr) {
    temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  }
#endif  // !PROTOBUF_FORCE_COPY_IN_RELEASE
  return temp;
}
inline ::np2ps::NewspaperEntry* NetworkSerializedNewspaperEntry::unsafe_arena_release_entry() {
  // @@protoc_insertion_point(field_release:np2ps.NetworkSerializedNewspaperEntry.entry)
  
  ::np2ps::NewspaperEntry* temp = _impl_.entry_;
  _impl_.entry_ = nullptr;
  return temp;
}
inline ::np2ps::NewspaperEntry* NetworkSerializedNewspaperEntry::_internal_mutable_entry() {
  
  if (_impl_.entry_ == nullptr) {
    auto* p = CreateMaybeMessage<::np2ps::NewspaperEntry>(GetArenaForAllocation());
    _impl_.entry_ = p;
  }
  return _impl_.entry_;
}
inline ::np2ps::NewspaperEntry* NetworkSerializedNewspaperEntry::mutable_entry() {
  ::np2ps::NewspaperEntry* _msg = _internal_mutable_entry();
  // @@protoc_insertion_point(field_mutable:np2ps.NetworkSerializedNewspaperEntry.entry)
  return _msg;
}
inline void NetworkSerializedNewspaperEntry::set_allocated_entry(::np2ps::NewspaperEntry* entry) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  if (message_arena == nullptr) {
    delete _impl_.entry_;
  }
  if (entry) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
        ::PROTOBUF_NAMESPACE_ID::Arena::InternalGetOwningArena(entry);
    if (message_arena != submessage_arena) {
      entry = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, entry, submessage_arena);
    }
    
  } else {
    
  }
  _impl_.entry_ = entry;
  // @@protoc_insertion_point(field_set_allocated:np2ps.NetworkSerializedNewspaperEntry.entry)
}

// repeated .np2ps.Article articles = 3;
inline int NetworkSerializedNewspaperEntry::_internal_articles_size() const {
  return _impl_.articles_.size();
}
inline int NetworkSerializedNewspaperEntry::articles_size() const {
  return _internal_articles_size();
}
inline ::np2ps::Article* NetworkSerializedNewspaperEntry::mutable_articles(int index) {
  // @@protoc_insertion_point(field_mutable:np2ps.NetworkSerializedNewspaperEntry.articles)
  return _impl_.articles_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::np2ps::Article >*
NetworkSerializedNewspaperEntry::mutable_articles() {
  // @@protoc_insertion_point(field_mutable_list:np2ps.NetworkSerializedNewspaperEntry.articles)
  return &_impl_.articles_;
}
inline const ::np2ps::Article& NetworkSerializedNewspaperEntry::_internal_articles(int index) const {
  return _impl_.articles_.Get(index);
}
inline const ::np2ps::Article& NetworkSerializedNewspaperEntry::articles(int index) const {
  // @@protoc_insertion_point(field_get:np2ps.NetworkSerializedNewspaperEntry.articles)
  return _internal_articles(index);
}
inline ::np2ps::Article* NetworkSerializedNewspaperEntry::_internal_add_articles() {
  return _impl_.articles_.Add();
}
inline ::np2ps::Article* NetworkSerializedNewspaperEntry::add_articles() {
  ::np2ps::Article* _add = _internal_add_articles();
  // @@protoc_insertion_point(field_add:np2ps.NetworkSerializedNewspaperEntry.articles)
  return _add;
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::np2ps::Article >&
NetworkSerializedNewspaperEntry::articles() const {
  // @@protoc_insertion_point(field_list:np2ps.NetworkSerializedNewspaperEntry.articles)
  return _impl_.articles_;
}

// -------------------------------------------------------------------

// LocalSerializedNewspaperEntry

// .np2ps.NewspaperEntry entry = 1;
inline bool LocalSerializedNewspaperEntry::_internal_has_entry() const {
  return this != internal_default_instance() && _impl_.entry_ != nullptr;
}
inline bool LocalSerializedNewspaperEntry::has_entry() const {
  return _internal_has_entry();
}
inline void LocalSerializedNewspaperEntry::clear_entry() {
  if (GetArenaForAllocation() == nullptr && _impl_.entry_ != nullptr) {
    delete _impl_.entry_;
  }
  _impl_.entry_ = nullptr;
}
inline const ::np2ps::NewspaperEntry& LocalSerializedNewspaperEntry::_internal_entry() const {
  const ::np2ps::NewspaperEntry* p = _impl_.entry_;
  return p != nullptr ? *p : reinterpret_cast<const ::np2ps::NewspaperEntry&>(
      ::np2ps::_NewspaperEntry_default_instance_);
}
inline const ::np2ps::NewspaperEntry& LocalSerializedNewspaperEntry::entry() const {
  // @@protoc_insertion_point(field_get:np2ps.LocalSerializedNewspaperEntry.entry)
  return _internal_entry();
}
inline void LocalSerializedNewspaperEntry::unsafe_arena_set_allocated_entry(
    ::np2ps::NewspaperEntry* entry) {
  if (GetArenaForAllocation() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.entry_);
  }
  _impl_.entry_ = entry;
  if (entry) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:np2ps.LocalSerializedNewspaperEntry.entry)
}
inline ::np2ps::NewspaperEntry* LocalSerializedNewspaperEntry::release_entry() {
  
  ::np2ps::NewspaperEntry* temp = _impl_.entry_;
  _impl_.entry_ = nullptr;
#ifdef PROTOBUF_FORCE_COPY_IN_RELEASE
  auto* old =  reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(temp);
  temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  if (GetArenaForAllocation() == nullptr) { delete old; }
#else  // PROTOBUF_FORCE_COPY_IN_RELEASE
  if (GetArenaForAllocation() != nullptr) {
    temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  }
#endif  // !PROTOBUF_FORCE_COPY_IN_RELEASE
  return temp;
}
inline ::np2ps::NewspaperEntry* LocalSerializedNewspaperEntry::unsafe_arena_release_entry() {
  // @@protoc_insertion_point(field_release:np2ps.LocalSerializedNewspaperEntry.entry)
  
  ::np2ps::NewspaperEntry* temp = _impl_.entry_;
  _impl_.entry_ = nullptr;
  return temp;
}
inline ::np2ps::NewspaperEntry* LocalSerializedNewspaperEntry::_internal_mutable_entry() {
  
  if (_impl_.entry_ == nullptr) {
    auto* p = CreateMaybeMessage<::np2ps::NewspaperEntry>(GetArenaForAllocation());
    _impl_.entry_ = p;
  }
  return _impl_.entry_;
}
inline ::np2ps::NewspaperEntry* LocalSerializedNewspaperEntry::mutable_entry() {
  ::np2ps::NewspaperEntry* _msg = _internal_mutable_entry();
  // @@protoc_insertion_point(field_mutable:np2ps.LocalSerializedNewspaperEntry.entry)
  return _msg;
}
inline void LocalSerializedNewspaperEntry::set_allocated_entry(::np2ps::NewspaperEntry* entry) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  if (message_arena == nullptr) {
    delete _impl_.entry_;
  }
  if (entry) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
        ::PROTOBUF_NAMESPACE_ID::Arena::InternalGetOwningArena(entry);
    if (message_arena != submessage_arena) {
      entry = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, entry, submessage_arena);
    }
    
  } else {
    
  }
  _impl_.entry_ = entry;
  // @@protoc_insertion_point(field_set_allocated:np2ps.LocalSerializedNewspaperEntry.entry)
}

// repeated .np2ps.SerializedArticle articles = 2;
inline int LocalSerializedNewspaperEntry::_internal_articles_size() const {
  return _impl_.articles_.size();
}
inline int LocalSerializedNewspaperEntry::articles_size() const {
  return _internal_articles_size();
}
inline ::np2ps::SerializedArticle* LocalSerializedNewspaperEntry::mutable_articles(int index) {
  // @@protoc_insertion_point(field_mutable:np2ps.LocalSerializedNewspaperEntry.articles)
  return _impl_.articles_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::np2ps::SerializedArticle >*
LocalSerializedNewspaperEntry::mutable_articles() {
  // @@protoc_insertion_point(field_mutable_list:np2ps.LocalSerializedNewspaperEntry.articles)
  return &_impl_.articles_;
}
inline const ::np2ps::SerializedArticle& LocalSerializedNewspaperEntry::_internal_articles(int index) const {
  return _impl_.articles_.Get(index);
}
inline const ::np2ps::SerializedArticle& LocalSerializedNewspaperEntry::articles(int index) const {
  // @@protoc_insertion_point(field_get:np2ps.LocalSerializedNewspaperEntry.articles)
  return _internal_articles(index);
}
inline ::np2ps::SerializedArticle* LocalSerializedNewspaperEntry::_internal_add_articles() {
  return _impl_.articles_.Add();
}
inline ::np2ps::SerializedArticle* LocalSerializedNewspaperEntry::add_articles() {
  ::np2ps::SerializedArticle* _add = _internal_add_articles();
  // @@protoc_insertion_point(field_add:np2ps.LocalSerializedNewspaperEntry.articles)
  return _add;
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::np2ps::SerializedArticle >&
LocalSerializedNewspaperEntry::articles() const {
  // @@protoc_insertion_point(field_list:np2ps.LocalSerializedNewspaperEntry.articles)
  return _impl_.articles_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace np2ps

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_newspaper_5fentry_2eproto
